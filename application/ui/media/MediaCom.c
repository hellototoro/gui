/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-13 13:31:24
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-09-11 21:22:40
 * @FilePath: /gui/application/ui/media/MediaCom.c
 * @Description: None
 * @other: None
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "MediaCom.h"
#include "Video.h"
#include "Music.h"
#include "Photo.h"
#ifdef HCCHIP_GCC
#include <pthread.h>
#include <sys/msg.h>
#include <ffplayer.h>
#include <hcuapi/codec_id.h>
#include "hcapi/com_api.h"
#include "hcapi/os_api.h"
#endif
#include "application/ui/Volume.h"
#include "application/key_map.h"

/* 全局变量 */
char current_path[100];
int current_path_size = sizeof(current_path);

/* 文件内全局变量 */
static lv_obj_t* PlayBar;
static lv_obj_t* PlayListPanel;
static MediaType CurrentPlayingType = MEDIA_MAX;
static PlayListMode CurrentPlayMode;
static MediaHandle* current_media_hdl;
static MediaList* media_list[MEDIA_MAX];
#ifdef HOST_GCC
static uint32_t played_time_host;
static lv_ffmpeg_player_cmd_t play_state;
#endif
static file_name_t* media_file_name_array;
static int current_playing_index;
static bool PlayingAnimation_Flag;

static lv_group_t* MainGroup;
static lv_timer_t* PlayBar_Timer;
static lv_timer_t* PlayState_Timer;

static const lv_img_dsc_t* play_mode_image_src[PlayModeNumber] = {
    &ui_img_play_repeat_mode_new_png,
    &ui_img_play_order_mode_new_png,
    &ui_img_play_one_repeat_mode_new_png,
    &ui_img_play_rand_mode_new_png
};

static const lv_img_dsc_t* play_list_image_src[MEDIA_MAX] = {
    NULL,
    &ui_img_movie_list_png,
    &ui_img_music_list_png,
    &ui_img_photo_list_png,
    NULL
};

extern pthread_mutex_t lvgl_task_mutex;

static MediaList* GetMediaList(MediaType media_type);
static DLNode * GetNextMediaNode(MediaList* list, PlayListMode mode);
static DLNode * GetPreMediaNode(MediaList* list, PlayListMode mode);
static void ShowOnPlayList(lv_obj_t *screen, file_name_t* name_list, int file_number);
static void ShowOffPlayList(void);
static void SetTotalTimeAndProgress(uint32_t total_time);
static MediaList* CreateMediaList(MediaType media_type);
static void DestroyMediaList(MediaType media_type);
static uint16_t GetMediaArraySize(MediaType media_type);
static void SetMediaIndex(int index);
static char* GetPreMediaName(MediaType media_type, PlayListMode mode);
static char* GetNextMediaName(MediaType media_type, PlayListMode mode, GetNextMode next_mode);
static file_name_t* GetMediaArray(void);
/************动画*************/
static void ShowUpAnimation(lv_obj_t * TargetObject, int delay);
static void ShowDownAnimation(lv_obj_t * TargetObject, int delay);

void MediaComInit(MediaType media_type, MediaHandle* media_hdl)
{
    CurrentPlayingType = media_type;
    current_media_hdl = media_hdl;
    //CurrentPlayMode = RandPlay;
    srand(time(0));
    PlayingAnimation_Flag = false;

    //设置组
    MainGroup = create_new_group();
    set_group_activity(MainGroup);
}

void MediaComDeinit(void)
{
    current_media_hdl = NULL;
    //step3 恢复默认组
    delete_group(MainGroup);

    //step4 清除定时器
    if (CurrentPlayingType == MEDIA_VIDEO || CurrentPlayingType == MEDIA_PHOTO)
        lv_timer_del(PlayBar_Timer);
    CurrentPlayingType = MEDIA_MAX;
    lv_timer_del(PlayState_Timer);

    CloseLoadingScreen();
}

static MediaList* CreateMediaList(MediaType media_type)
{
    media_list[media_type] = (MediaList*) malloc(sizeof(MediaList));
    InitDList(media_list[media_type]);
    return media_list[media_type];
}

void AddToMediaList(MediaType media_type, char * media_name)
{
    if (media_list[media_type] == NULL)
        CreateMediaList(media_type);
    DListAppend(media_list[media_type], media_name);
}

MediaList* GetMediaList(MediaType media_type)
{
    return media_list[media_type];
}

uint16_t GetMediaListSize(MediaType media_type)
{
    if (media_list[media_type] != NULL)
        return media_list[media_type]->len;
    else
        return 0;
}

DLNode* GetNextMediaNode(MediaList* list, PlayListMode mode)
{
    static DLNode *next = NULL;
    static DLNode *head = NULL;
    static MediaList* last_list = NULL;

    if (list == NULL) {//清除获取记录
        last_list = NULL;
        next = NULL;
        head = NULL;
        return NULL;
    }

    if ((next == head) && (head != NULL)) { //一个循环结束
        if (mode == CyclePlay)
            next = head->next;
        else
            return NULL;
    }

    if (last_list != list) {
        last_list = list;
        head = list->head;
        next = head->next;
    }
    else {
        next = next->next;
    }
    return next;
}

DLNode * GetPreMediaNode(MediaList* list, PlayListMode mode)
{
    static DLNode *pre = NULL;
    static DLNode *head = NULL;
    static MediaList* last_list = NULL;

    if (list == NULL) {//清除获取记录
        last_list = NULL;
        pre = NULL;
        head = NULL;
        return NULL;
    }

    if ((pre == head) && (head != NULL)) { //一个循环结束
        if (mode == CyclePlay)
            pre = head->pre;
        else
            return NULL;
    }

    if (last_list != list) {
        last_list = list;
        head = list->head;
        pre = head->pre;
    }
    else {
        pre = pre->pre;
    }
    return pre;
}

static void DestroyMediaList(MediaType media_type)
{
    if (media_list[media_type] != NULL) {
        DestroyDList(media_list[media_type]);
        media_list[media_type] = NULL;
    }
}

void DestroyAllMediaList(void)
{
    for (int i = 0; i < MEDIA_MAX; i++) {
        DestroyMediaList(i);
    }
}

void CreateMediaArray(void)
{
    int n = GetMediaListSize(CurrentPlayingType);
    if (n == 0) return;
    DestroyMediaArray();
    current_playing_index = 0;
    media_file_name_array = (file_name_t*) malloc(sizeof(file_name_t) * n);
    GetNextMediaNode(NULL, OrderPlay);
    for (int i = 0; i < n; i++) {
        media_file_name_array[i] = (char*) (GetNextMediaNode(GetMediaList(CurrentPlayingType), OrderPlay))->data;
    }
}

static uint16_t GetMediaArraySize(MediaType media_type)
{
    return GetMediaListSize(media_type);
}

uint16_t LocateMediaIndex(char * file_name)
{
    int i;
    uint16_t n = GetMediaArraySize(CurrentPlayingType);
    for (i = 0; i < n; i++) {
        if(file_name == NULL) break;
        if (strcmp(media_file_name_array[i], file_name) == 0) {
            current_playing_index = i;
            break;
        }
    }
    return current_playing_index;
}

static void SetMediaIndex(int index)
{
    if (index < GetMediaArraySize(CurrentPlayingType))
        current_playing_index = index;
}

char* GetCurrentMediaName(void)
{
    return media_file_name_array[current_playing_index];
}

static char* GetNextMediaName(MediaType media_type, PlayListMode mode, GetNextMode next_mode)
{
    switch (next_mode) {
        case ManualPlay:
            ++current_playing_index;
            current_playing_index %= GetMediaArraySize(media_type);
            break;

        case AutoPlay:
            switch (mode) {
                case CyclePlay:
                    ++current_playing_index;
                    current_playing_index %= GetMediaArraySize(media_type);
                    break;

                case OrderPlay:
                    if (current_playing_index < GetMediaArraySize(media_type)-1)
                        ++current_playing_index;
                    else
                        return NULL;
                    break;

                case OnlyOnePlay:
                    break;

                case RandPlay:
                    current_playing_index = rand() % GetMediaArraySize(media_type);
                    break;
                
                default:
                    break;
            }
        default:
            break;
    }
    return media_file_name_array[current_playing_index];
}

static char* GetPreMediaName(MediaType media_type, PlayListMode mode)
{
    (void)mode;
    --current_playing_index;
    if (current_playing_index < 0)
        current_playing_index = GetMediaArraySize(media_type) - 1;
    return media_file_name_array[current_playing_index];
}

static file_name_t* GetMediaArray(void)
{
    return media_file_name_array;
}

MediaType GetPlayingMediaType(void)
{
    return CurrentPlayingType;
}

void DestroyMediaArray(void)
{
    if (media_file_name_array != NULL) {
        free(media_file_name_array);
        media_file_name_array = NULL;
    }
}

void PlayMedia(char * file_name)
{
    if (file_name != NULL) {
        char file_path[100] = {0};
        strcat(file_path, current_path);
        strcat(file_path, "/");
        strcat(file_path,  file_name);
        #ifdef HOST_GCC
        lv_ffmpeg_player_set_src(current_media_hdl, file_path);
        lv_ffmpeg_player_set_cmd(current_media_hdl, LV_FFMPEG_PLAYER_CMD_START);
        play_state = LV_FFMPEG_PLAYER_CMD_START;
        played_time_host = 0;
        #elif defined(HCCHIP_GCC)
        if (current_media_hdl->state == MEDIA_PLAY)
            media_stop(current_media_hdl);
        media_play(current_media_hdl, file_path);
        #endif
        if(CurrentPlayingType == MEDIA_VIDEO || CurrentPlayingType == MEDIA_PHOTO ) {
            #ifdef HCCHIP_GCC
            CreateLoadingScreen(lv_scr_act());
            #endif
        }
        else if(CurrentPlayingType == MEDIA_MUSIC) {
            SetCurrentMusicTitle(file_name);
            LoadLyric(file_name);
            MusicCoverSpinRun();
        }
    }
}

//公共ui部分
static void key_event_handler(lv_event_t* event)
{
    lv_obj_t* target = lv_event_get_current_target(event);
    uint32_t value = lv_indev_get_key(lv_indev_get_act());
    lv_group_t* group = (lv_group_t*)lv_obj_get_group(target);
    if (lv_obj_has_flag(PlayBar, LV_OBJ_FLAG_HIDDEN)) {
        switch (value)
        {
            case LV_KEY_ENTER:
            case LV_KEY_LEFT:
            case LV_KEY_RIGHT:
                    lv_obj_clear_flag(PlayBar, LV_OBJ_FLAG_HIDDEN);
                    lv_timer_reset(PlayBar_Timer);
                    lv_timer_resume(PlayBar_Timer);
                return;
            default:
            break;
        }
    }
    switch (value)
    {
        case LV_KEY_ENTER:
            switch (lv_obj_get_index(target))
            {
                case Play:
                {
                    #ifdef HOST_GCC
                    lv_ffmpeg_player_cmd_t state = play_state;
                    #define __MEDIA_PLAY    LV_FFMPEG_PLAYER_CMD_START
                    #define __MEDIA_PAUSE   LV_FFMPEG_PLAYER_CMD_PAUSE
                    #elif defined(HCCHIP_GCC)
                    media_state_t state = current_media_hdl->state;
                    #define __MEDIA_PLAY    MEDIA_PLAY
                    #define __MEDIA_PAUSE   MEDIA_PAUSE
                    #endif
                    if (state == __MEDIA_PLAY) {
                        //lv_img_set_src(target, &ui_img_play_start_png);
                        lv_obj_set_style_bg_img_src(target, &ui_img_play_start_png, LV_PART_MAIN | LV_STATE_DEFAULT);
                        #ifdef HOST_GCC
                        play_state = __MEDIA_PAUSE;
                        lv_ffmpeg_player_set_cmd(current_media_hdl, __MEDIA_PAUSE);
                        #elif defined(HCCHIP_GCC)
                        current_media_hdl->state = __MEDIA_PAUSE;
                        media_pause(current_media_hdl);
                        #endif
                    }
                    else if (state == __MEDIA_PAUSE) {
                        //lv_img_set_src(target, &ui_img_play_pause_png);
                        lv_obj_set_style_bg_img_src(target, &ui_img_play_pause_png, LV_PART_MAIN | LV_STATE_DEFAULT);
                        #ifdef HOST_GCC
                        play_state = __MEDIA_PLAY;
                        lv_ffmpeg_player_set_cmd(current_media_hdl, LV_FFMPEG_PLAYER_CMD_RESUME);
                        #elif defined(HCCHIP_GCC)
                        current_media_hdl->state = __MEDIA_PLAY;
                        media_resume(current_media_hdl);
                        #endif
                    }
                }
                break;
                case Previous:
                    PlayMedia(GetPreMediaName(CurrentPlayingType, CurrentPlayMode));
                    break;
                case Next:
                    PlayMedia(GetNextMediaName(CurrentPlayingType, CurrentPlayMode, ManualPlay));
                    break;
                case PlayList:
                    ShowOnPlayList(NULL, GetMediaArray(), GetMediaArraySize(CurrentPlayingType));
                    break;
                case PlayMode:
                    CurrentPlayMode++;
                    CurrentPlayMode %= PlayModeNumber;
                    //lv_img_set_src(lv_obj_get_child(PlayBar, PlayMode), play_mode_image_src[CurrentPlayMode]);
                    lv_obj_set_style_bg_img_src(lv_obj_get_child(PlayBar, PlayMode), play_mode_image_src[CurrentPlayMode], LV_PART_MAIN | LV_STATE_DEFAULT);
                    //CyclePlay
                    break;
                default:
                    break;
            }
            break;
    
        case LV_KEY_LEFT:
            lv_group_focus_prev(group);
            break;
        case LV_KEY_RIGHT:
            lv_group_focus_next(group);
            break;
        case LV_KEY_ESC:
            if (!PlayingAnimation_Flag) {
                switch (CurrentPlayingType)
                {
                case MEDIA_VIDEO:
                    close_video_window();
                    break;
                case MEDIA_MUSIC:
                    close_music_window();
                    break;
                case MEDIA_PHOTO:
                    close_photo_window();
                    break;
                
                default:
                    break;
                }
                return;
            }
            break;

        default:
            base_event_handler(event);
            break;
    }
    if (CurrentPlayingType == MEDIA_VIDEO || CurrentPlayingType == MEDIA_PHOTO) {
        if (!lv_obj_has_flag(PlayBar, LV_OBJ_FLAG_HIDDEN)) {
            lv_timer_reset(PlayBar_Timer);
        }
    }
}

static void play_list_event_handler(lv_event_t* event)
{
    lv_obj_t* target = lv_event_get_current_target(event);
    lv_group_t* group = (lv_group_t*)lv_obj_get_group(target);
    uint32_t value = lv_indev_get_key(lv_indev_get_act());
    switch (value)
    {
        case LV_KEY_ENTER:
            SetMediaIndex(lv_obj_get_index(target));
            PlayMedia(GetCurrentMediaName());
            break;
        case LV_KEY_UP:
            lv_group_focus_prev(group);
            break;
        case LV_KEY_DOWN:
            lv_group_focus_next(group);
            break;
        case LV_KEY_ESC:
            if (!PlayingAnimation_Flag) {
                if (lv_obj_is_valid(PlayListPanel)) {
                    ShowOffPlayList();
                }
            }
            break;
        default:
            base_event_handler(event);
            break;
    }
}

static void PlayBar_Timer_cb(lv_timer_t * timer)
{
    lv_obj_add_flag(PlayBar, LV_OBJ_FLAG_HIDDEN);// | LV_OBJ_FLAG_ADV_HITTEST
    lv_timer_pause(timer);
    lv_timer_reset(timer);
}

static void ShowPlayedState(lv_timer_t * timer)
{
    #ifdef HOST_GCC
    if (play_state == LV_FFMPEG_PLAYER_CMD_START)
        played_time_host++;
    #endif
    if (!lv_obj_has_flag(PlayBar, LV_OBJ_FLAG_HIDDEN)) {
        #ifdef HOST_GCC
        uint32_t played_time = played_time_host;
        #elif defined(HCCHIP_GCC)
        uint32_t played_time = 0;
        if (current_media_hdl != NULL)
            played_time = media_get_playtime(current_media_hdl);
        #endif
        if (played_time >= lv_slider_get_max_value(lv_obj_get_child(PlayBar, ProgressSlider))) {
            lv_timer_pause(timer);
            #ifdef HOST_GCC
            PlayMedia(GetNextMediaName(CurrentPlayingType, CurrentPlayMode, AutoPlay));
            if (played_time_host == 0)
                SetTotalTimeAndProgress(20);
            #endif
            return;
        }
        if(CurrentPlayingType == MEDIA_MUSIC) { //刷新歌词
            RefreshLyric(played_time);
        }
        lv_label_set_text_fmt(lv_obj_get_child(PlayBar, PlayedTime), "%02"LV_PRIu32":%02"LV_PRIu32":%02"LV_PRIu32, played_time / 3600, (played_time % 3600) / 60, played_time % 60);
        lv_slider_set_value(lv_obj_get_child(PlayBar, ProgressSlider), played_time, LV_ANIM_ON);
    }
}

lv_obj_t* CreatePlayBar(lv_obj_t* parent)
{
    lv_obj_t* lv_obj;
    static const lv_coord_t img_area[][2] = {//{ x, y}
        { -200, 10},
        { -100, 10},
        {    0, 10},
        {  100, 10},
        {  200, 10}
    };
    static const lv_img_dsc_t* image_src[] = {
        & ui_img_play_rand_mode_new_png,
        & ui_img_play_pre_png,
        & ui_img_play_pause_png,
        & ui_img_play_next_png,
        & ui_img_music_list_png,
    };

    PlayBar = lv_obj_create(parent);
    lv_obj_set_width(PlayBar, 1260);
    lv_obj_set_height(PlayBar, 150);
    lv_obj_set_x(PlayBar, 0);
    lv_obj_set_y(PlayBar, 300);
    lv_obj_set_align(PlayBar, LV_ALIGN_CENTER);
    lv_obj_clear_flag(PlayBar, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj = lv_label_create(PlayBar);
    lv_obj_set_width(lv_obj, LV_SIZE_CONTENT);
    lv_obj_set_height(lv_obj, LV_SIZE_CONTENT);
    lv_obj_set_x(lv_obj, -550);
    lv_obj_set_y(lv_obj, -50);
    lv_obj_set_align(lv_obj, LV_ALIGN_CENTER);
    lv_label_set_text(lv_obj, "00:00:00");
    lv_obj_set_style_text_font(lv_obj, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj = lv_slider_create(PlayBar);
    //lv_slider_set_range(ProgressSlider, 0, 100);
    lv_slider_set_value(lv_obj, 0, LV_ANIM_OFF);
    if(lv_slider_get_mode(lv_obj) == LV_SLIDER_MODE_RANGE) lv_slider_set_left_value(lv_obj, 0, LV_ANIM_OFF);
    lv_obj_set_width(lv_obj, 900);
    lv_obj_set_height(lv_obj, 7);
    lv_obj_set_x(lv_obj, 0);
    lv_obj_set_y(lv_obj, -50);
    lv_obj_set_align(lv_obj, LV_ALIGN_CENTER);

    lv_obj = lv_label_create(PlayBar);
    lv_obj_set_width(lv_obj, LV_SIZE_CONTENT);
    lv_obj_set_height(lv_obj, LV_SIZE_CONTENT);
    lv_obj_set_x(lv_obj, 540);
    lv_obj_set_y(lv_obj, -50);
    lv_obj_set_align(lv_obj, LV_ALIGN_CENTER);
    lv_label_set_text(lv_obj, "00:00:00");
    lv_obj_set_style_text_font(lv_obj, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);
    for (int i = 0; i < PlayBarNumber - PlayMode; i++) {
        lv_obj_t* ctrl_bar = lv_btn_create(PlayBar);
        lv_obj_set_size(ctrl_bar, 75, 75);
        lv_obj_set_pos(ctrl_bar, img_area[i][0], img_area[i][1]);
        lv_obj_set_align(ctrl_bar, LV_ALIGN_CENTER);
        lv_obj_set_style_radius(ctrl_bar, 35, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_radius(ctrl_bar, 35, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_bg_color(ctrl_bar, lv_color_hex(0x08AED2), LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_bg_opa(ctrl_bar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(ctrl_bar, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_border_opa(ctrl_bar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_opa(ctrl_bar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_img_src(ctrl_bar, image_src[i], LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_img_recolor(ctrl_bar, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_img_recolor_opa(ctrl_bar, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_group_add_obj(MainGroup, ctrl_bar);
        lv_obj_add_event_cb(ctrl_bar, key_event_handler, LV_EVENT_KEY, NULL);
    }
    lv_obj_set_style_bg_img_src(lv_obj_get_child(PlayBar, PlayMode), play_mode_image_src[CurrentPlayMode], LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(lv_obj_get_child(PlayBar, PlayList), play_list_image_src[CurrentPlayingType], LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_group_focus_obj(lv_obj_get_child(PlayBar, Play));
    if (CurrentPlayingType == MEDIA_VIDEO || CurrentPlayingType == MEDIA_PHOTO) {
        PlayBar_Timer = lv_timer_create(PlayBar_Timer_cb, 5*1000, NULL);
    }
    PlayState_Timer = lv_timer_create(ShowPlayedState, 1000, NULL);
    lv_timer_set_repeat_count(PlayState_Timer, -1);
    lv_timer_pause(PlayState_Timer);
    #ifdef HOST_GCC
    SetTotalTimeAndProgress(20);
    #endif
    return PlayBar;
}

static void SetTotalTimeAndProgress(uint32_t total_time)
{
    if (total_time == 0) total_time = 100;
    lv_label_set_text_fmt(lv_obj_get_child(PlayBar, TotalTime), "%02"LV_PRIu32":%02"LV_PRIu32":%02"LV_PRIu32, (total_time) / 3600, ((total_time) % 3600) / 60, (total_time) % 60);
    lv_slider_set_range(lv_obj_get_child(PlayBar, ProgressSlider), 0, total_time);
    lv_timer_reset(PlayState_Timer);
    lv_timer_resume(PlayState_Timer);
}

static void CreatePlayListPanel(lv_obj_t* parent, file_name_t* name_list, int file_number)
{
    PlayListPanel = lv_obj_create(lv_scr_act());
    lv_obj_set_width(PlayListPanel, 1280);
    lv_obj_set_height(PlayListPanel, 682);
    lv_obj_set_x(PlayListPanel, 0);
    lv_obj_set_y(PlayListPanel, 705);
    lv_obj_set_align(PlayListPanel, LV_ALIGN_CENTER);
    lv_obj_clear_flag(PlayListPanel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(PlayListPanel, 40, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* PlayListInfo = lv_obj_create(PlayListPanel);
    lv_obj_set_width(PlayListInfo, 647);
    lv_obj_set_height(PlayListInfo, 64);
    lv_obj_set_x(PlayListInfo, -300);
    lv_obj_set_y(PlayListInfo, -300);
    lv_obj_set_align(PlayListInfo, LV_ALIGN_CENTER);
    lv_obj_clear_flag(PlayListInfo, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(PlayListInfo, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(PlayListInfo, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* PlayListMode_IMG = lv_img_create(PlayListInfo);
    lv_img_set_src(PlayListMode_IMG, play_mode_image_src[CurrentPlayMode]);
    lv_obj_set_width(PlayListMode_IMG, LV_SIZE_CONTENT);
    lv_obj_set_height(PlayListMode_IMG, LV_SIZE_CONTENT);
    lv_obj_set_x(PlayListMode_IMG, 0);
    lv_obj_set_y(PlayListMode_IMG, 0);
    lv_obj_set_align(PlayListMode_IMG, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(PlayListMode_IMG, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(PlayListMode_IMG, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* PlayListMode_LAB = lv_label_create(PlayListInfo);
    lv_obj_set_width(PlayListMode_LAB, LV_SIZE_CONTENT);
    lv_obj_set_height(PlayListMode_LAB, LV_SIZE_CONTENT);
    lv_obj_set_x(PlayListMode_LAB, 79);
    lv_obj_set_y(PlayListMode_LAB, 0);
    lv_obj_set_align(PlayListMode_LAB, LV_ALIGN_LEFT_MID);

    //char buf[64];
    //sprintf(buf, _p("songs", file_number), file_number);
    //lv_label_set_text(PlayListMode_LAB, buf);
    lv_label_set_text_fmt(PlayListMode_LAB, _p("songs", file_number), file_number);
    lv_obj_set_style_text_font(PlayListMode_LAB, &ui_font_MyFont34, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* FileListPanel = lv_obj_create(PlayListPanel);
    lv_obj_set_width(FileListPanel, 1280);
    lv_obj_set_height(FileListPanel, 535);
    lv_obj_set_x(FileListPanel, 0);
    lv_obj_set_y(FileListPanel, -5);
    lv_obj_set_align(FileListPanel, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(FileListPanel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_bg_opa(FileListPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(FileListPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    //设置组
    MainGroup = create_new_group();
    set_group_activity(MainGroup);
    for(int i = 0; i < file_number; i++) {
        lv_obj_t* file_panel = lv_obj_create(FileListPanel);
        lv_obj_set_width(file_panel, 1140);
        lv_obj_set_height(file_panel, 75);
        lv_obj_set_align(file_panel, LV_ALIGN_CENTER);
        lv_obj_clear_flag(file_panel, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_flag(file_panel, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
        lv_obj_set_style_radius(file_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(file_panel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_opa(file_panel, 90, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_side(file_panel, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_radius(file_panel, 15, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_bg_color(file_panel, lv_color_hex(0xA5CAC3), LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_bg_opa(file_panel, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_border_opa(file_panel, 0, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_group_add_obj(MainGroup, file_panel);
        lv_obj_add_event_cb(file_panel, play_list_event_handler, LV_EVENT_KEY, NULL);

        lv_obj_t* file_name = lv_label_create(file_panel);
        lv_obj_set_width(file_name, LV_SIZE_CONTENT);
        lv_obj_set_height(file_name, LV_SIZE_CONTENT);
        lv_obj_set_x(file_name, -12);
        lv_obj_set_y(file_name, 0);
        lv_label_set_text(file_name, name_list[i]);
        lv_obj_set_style_text_font(file_name, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    lv_group_focus_obj(lv_obj_get_child(FileListPanel, current_playing_index));
}

static void ShowOnPlayList(lv_obj_t *screen, file_name_t* name_list, int file_number)
{
    CreatePlayListPanel(screen, name_list, file_number);
    ShowUpAnimation(PlayListPanel, 300);
}

static void ShowOffPlayList(void)
{
    ShowDownAnimation(PlayListPanel, 300);
    PlayingAnimation_Flag = true;
}

/************动画*************/
static void anim_callback_delete_obj(struct _lv_anim_t *a)
{
    MainGroup = delete_group(MainGroup);
    lv_obj_del_async((lv_obj_t *)a->var);
    PlayingAnimation_Flag = false;
}

static void ShowUpAnimation(lv_obj_t * TargetObject, int delay)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_time(&a, 500);
    lv_anim_set_var(&a, TargetObject);
    lv_anim_set_exec_cb(&a, anim_callback_set_y);
    lv_anim_set_values(&a, lv_obj_get_y(TargetObject), -605);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
    lv_anim_set_delay(&a, delay + 0);
    lv_anim_set_early_apply(&a, false);
    lv_anim_set_get_value_cb(&a, &anim_callback_get_y);
    lv_anim_start(&a);
}

static void ShowDownAnimation(lv_obj_t * TargetObject, int delay)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_time(&a, 500);
    lv_anim_set_var(&a, TargetObject);
    lv_anim_set_exec_cb(&a, anim_callback_set_y);
    lv_anim_set_values(&a, lv_obj_get_y(TargetObject), 605);
    lv_anim_set_path_cb(&a, lv_anim_path_overshoot);
    lv_anim_set_delay(&a, delay + 0);
    lv_anim_set_early_apply(&a, false);
    lv_anim_set_get_value_cb(&a, &anim_callback_get_y);
    lv_anim_set_deleted_cb(&a, anim_callback_delete_obj);
    lv_anim_start(&a);
}

//SetTotalTimeAndProgress(media_get_totaltime(media_hld));
#ifdef HCCHIP_GCC
void MediaMsgProc(media_handle_t *media_hld, HCPlayerMsg *msg)
{
    if (!media_hld || !msg) return;
    switch (msg->type)
    {
    case HCPLAYER_MSG_STATE_EOS:
        printf (">> app get eos, normal play end!\n");
        pthread_mutex_lock(&lvgl_task_mutex);
        PlayMedia(GetNextMediaName(CurrentPlayingType, CurrentPlayMode, AutoPlay));
        pthread_mutex_unlock(&lvgl_task_mutex);
        //api_control_send_key(V_KEY_NEXT);
        break;
    case HCPLAYER_MSG_STATE_TRICK_EOS:
        printf (">> app get trick eos, fast play to end\n");
        //api_control_send_key(V_KEY_NEXT);
        break;
    case HCPLAYER_MSG_STATE_TRICK_BOS:
        printf (">> app get trick bos, fast back play to begining!\n");
        //api_control_send_key(V_KEY_PLAY);
        break;
    case HCPLAYER_MSG_OPEN_FILE_FAILED:
        printf (">> open file fail\n");
        break;
    case HCPLAYER_MSG_ERR_UNDEFINED:
        printf (">> error unknow\n");
        break;
    case HCPLAYER_MSG_UNSUPPORT_FORMAT:
        printf (">> unsupport format\n");
        break;
    case HCPLAYER_MSG_BUFFERING:
        printf(">> buffering %d\n", msg->val);
        break;
    case HCPLAYER_MSG_STATE_PLAYING:
        printf(">> player playing\n");
        break;
    case HCPLAYER_MSG_STATE_PAUSED:
        printf(">> player paused\n");
        break;
    case HCPLAYER_MSG_STATE_READY:
        printf(">> player ready\n");
        pthread_mutex_lock(&lvgl_task_mutex);
        SetTotalTimeAndProgress(media_get_totaltime(media_hld));
        CloseLoadingScreen();
        pthread_mutex_unlock(&lvgl_task_mutex);
        break;
    case HCPLAYER_MSG_READ_TIMEOUT:
        printf(">> player read timeout\n");
        break;
    case HCPLAYER_MSG_UNSUPPORT_ALL_AUDIO:
        printf(">> no audio track/or no supported audio track\n");
        break;
    case HCPLAYER_MSG_UNSUPPORT_ALL_VIDEO:
        printf(">> no video track/or no supported video track\n");
        break;
    case HCPLAYER_MSG_UNSUPPORT_VIDEO_TYPE:
        {
            HCPlayerVideoInfo video_info;
            char *video_type = "unknow";
            if (!hcplayer_get_nth_video_stream_info (media_hld->player, msg->val, &video_info)) {
                /* only a simple sample, app developers use a static struct to mapping them. */
                if (video_info.codec_id == HC_AVCODEC_ID_HEVC) {
                    video_type = "h265";
                } 
            }
            printf("unsupport video type %s\n", video_type);
        }
        break;
    case HCPLAYER_MSG_UNSUPPORT_AUDIO_TYPE:
        {
            HCPlayerAudioInfo audio_info;
            char *audio_type = "unknow";
            if (!hcplayer_get_nth_audio_stream_info (media_hld->player, msg->val, &audio_info)) {
                /* only a simple sample, app developers use a static struct to mapping them. */
                if (audio_info.codec_id < 0x11000) {
                    audio_type = "pcm";
                } else if (audio_info.codec_id < 0x12000) {
                    audio_type = "adpcm";
                } else if (audio_info.codec_id == HC_AVCODEC_ID_DTS) {
                    audio_type = "dts";
                } else if (audio_info.codec_id == HC_AVCODEC_ID_EAC3) {
                    audio_type = "eac3";
                } else if (audio_info.codec_id == HC_AVCODEC_ID_APE) {
                    audio_type = "ape";
                } 
            }
            printf("unsupport audio type %s\n", audio_type);
        }
        break;
    case HCPLAYER_MSG_AUDIO_DECODE_ERR:
        {
            printf("audio dec err, audio idx %d\n", msg->val);
            /* check if it is the last audio track, if not, then change to next one. */
            if (media_hld->player) {
                int total_audio_num = -1;
                total_audio_num = hcplayer_get_audio_streams_count(media_hld->player);
                if (msg->val >= 0 && total_audio_num > (msg->val + 1)) {
                    HCPlayerAudioInfo audio_info;
                    if (!hcplayer_get_cur_audio_stream_info(media_hld->player, &audio_info)) {
                        if (audio_info.index == msg->val) {
                            int idx = audio_info.index + 1;
                            while (hcplayer_change_audio_track(media_hld->player, idx)) {
                                idx++;
                                if (idx >= total_audio_num) {
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
        break;
    case HCPLAYER_MSG_VIDEO_DECODE_ERR:
        {
            printf("video dec err, video idx %d\n", msg->val);
            /* check if it is the last video track, if not, then change to next one. */
            if (media_hld->player) {
                int total_video_num = -1;
                total_video_num = hcplayer_get_video_streams_count(media_hld->player);
                if (msg->val >= 0 && total_video_num > (msg->val + 1)) {
                    HCPlayerVideoInfo video_info;
                    if (!hcplayer_get_cur_video_stream_info(media_hld->player, &video_info)) {
                        if (video_info.index == msg->val) {
                            int idx = video_info.index + 1;
                            while (hcplayer_change_video_track(media_hld->player, idx)) {
                                idx++;
                                if (idx >= total_video_num) {
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
        break;
    default:
        break;
    }
}
#endif
