/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-13 13:31:24
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
#include "Text.h"

/* 全局变量 */
char current_path[100];
int current_path_size = sizeof(current_path);

/* 文件内全局变量 */
static lv_obj_t* PlayBar;
static lv_obj_t* PlayListPanel;
static MediaType CurrentPlayingType = MEDIA_MAX;
static PlayListMode CurrentPlayMode[MEDIA_MAX];
static MediaHandle* current_media_hdl;
static MediaList* media_list[MEDIA_MAX];
static uint32_t played_time_host;
static lv_ffmpeg_player_cmd_t play_state;
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
    &ui_img_photo_list_png
};
static const char* PlayModeName[] = {
    NULL,//0
    "media.VideoPlayMode",//1
    "media.MusicPlayMode",//2
    "media.PhotoPlayMode"//3 
};

extern pthread_mutex_t lvgl_task_mutex;
extern void WriteConfigFile_I(const char* ConfigName, int value);
extern int ReadConfigFile_I(const char* ConfigName);

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
    CurrentPlayMode[CurrentPlayingType] = ReadConfigFile_I(PlayModeName[CurrentPlayingType < 4 ? CurrentPlayingType : 0]);
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
    if (CurrentPlayingType != MEDIA_MUSIC)
        lv_timer_del(PlayBar_Timer);
    if (CurrentPlayingType != MEDIA_TEXT) 
        lv_timer_del(PlayState_Timer);
    CurrentPlayingType = MEDIA_MAX;

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
        lv_ffmpeg_player_set_src(current_media_hdl, file_path);
        lv_ffmpeg_player_set_cmd(current_media_hdl, LV_FFMPEG_PLAYER_CMD_START);
        play_state = LV_FFMPEG_PLAYER_CMD_START;
        played_time_host = 0;
        if(CurrentPlayingType == MEDIA_VIDEO || CurrentPlayingType == MEDIA_PHOTO ) {
            //CreateLoadingScreen(lv_scr_act());
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
            case LV_KEY_LEFT:
                if (CurrentPlayingType == MEDIA_TEXT){
                    Text_PrePage();
                    return;
                }
            case LV_KEY_RIGHT:
                if (CurrentPlayingType == MEDIA_TEXT){
                    Text_NextPage();
                    return;
                }
            case LV_KEY_ENTER:
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
                    lv_ffmpeg_player_cmd_t state = play_state;
                    #define __MEDIA_PLAY    LV_FFMPEG_PLAYER_CMD_START
                    #define __MEDIA_PAUSE   LV_FFMPEG_PLAYER_CMD_PAUSE
                    if (state == __MEDIA_PLAY) {
                        //lv_img_set_src(target, &ui_img_play_start_png);
                        lv_obj_set_style_bg_img_src(target, &ui_img_play_start_png, LV_PART_MAIN | LV_STATE_DEFAULT);
                        play_state = __MEDIA_PAUSE;
                        lv_ffmpeg_player_set_cmd(current_media_hdl, __MEDIA_PAUSE);
                    }
                    else if (state == __MEDIA_PAUSE) {
                        //lv_img_set_src(target, &ui_img_play_pause_png);
                        lv_obj_set_style_bg_img_src(target, &ui_img_play_pause_png, LV_PART_MAIN | LV_STATE_DEFAULT);
                        play_state = __MEDIA_PLAY;
                        lv_ffmpeg_player_set_cmd(current_media_hdl, LV_FFMPEG_PLAYER_CMD_RESUME);
                    }
                }
                break;
                case Previous:
                    if (CurrentPlayingType != MEDIA_TEXT)
                        PlayMedia(GetPreMediaName(CurrentPlayingType, CurrentPlayMode[CurrentPlayingType]));
                    else
                        Text_PrePage();
                    break;
                case Next:
                    if (CurrentPlayingType != MEDIA_TEXT)
                        PlayMedia(GetNextMediaName(CurrentPlayingType, CurrentPlayMode[CurrentPlayingType], ManualPlay));
                    else
                        Text_NextPage();
                    break;
                case PlayList:
                    ShowOnPlayList(NULL, GetMediaArray(), GetMediaArraySize(CurrentPlayingType));
                    break;
                case PlayMode:
                    CurrentPlayMode[CurrentPlayingType]++;
                    CurrentPlayMode[CurrentPlayingType] %= PlayModeNumber;
                    if (CurrentPlayingType != MEDIA_TEXT)
                        WriteConfigFile_I(PlayModeName[CurrentPlayingType], CurrentPlayMode[CurrentPlayingType]);
                    lv_obj_set_style_bg_img_src(lv_obj_get_child(PlayBar, PlayMode), play_mode_image_src[CurrentPlayMode[CurrentPlayingType]], LV_PART_MAIN | LV_STATE_DEFAULT);
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
                case MEDIA_TEXT:
                    close_text_window();
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
    if (CurrentPlayingType != MEDIA_MUSIC) {
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
            if (CurrentPlayingType == MEDIA_TEXT)
                ReLoadText(GetCurrentMediaName());
            else
                PlayMedia(GetCurrentMediaName());
            ShowOffPlayList();
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

static void PlayBarTimer_cb(lv_timer_t * timer)
{
    lv_obj_add_flag(PlayBar, LV_OBJ_FLAG_HIDDEN);// | LV_OBJ_FLAG_ADV_HITTEST
    lv_timer_pause(timer);
    lv_timer_reset(timer);
}

static void PlayedStateTimer_cb(lv_timer_t * timer)
{
    uint32_t played_time = 0;
    if (play_state == LV_FFMPEG_PLAYER_CMD_START)
        played_time_host++;
    played_time = played_time_host;
    if (played_time >= lv_slider_get_max_value(lv_obj_get_child(PlayBar, ProgressSlider))) {
        lv_timer_pause(timer);
        #ifdef HOST_GCC
        PlayMedia(GetNextMediaName(CurrentPlayingType, CurrentPlayMode[CurrentPlayingType], AutoPlay));
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

lv_obj_t* CreatePlayBar(lv_obj_t* parent)
{
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
    lv_obj_set_size(PlayBar, LV_HOR_RES * 0.98, 150);
    lv_obj_set_pos(PlayBar, 0, 300);
    lv_obj_set_align(PlayBar, LV_ALIGN_CENTER);
    lv_obj_clear_flag(PlayBar, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* lv_obj = lv_label_create(PlayBar);
    lv_obj_set_size(lv_obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_pos(lv_obj, -550, -50);
    lv_obj_set_align(lv_obj, LV_ALIGN_CENTER);
    lv_label_set_text(lv_obj, "00:00:00");
    lv_obj_set_style_text_font(lv_obj, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj = lv_slider_create(PlayBar);
    lv_slider_set_value(lv_obj, 0, LV_ANIM_OFF);
    if(lv_slider_get_mode(lv_obj) == LV_SLIDER_MODE_RANGE) lv_slider_set_left_value(lv_obj, 0, LV_ANIM_OFF);
    lv_obj_set_size(lv_obj, 900, 7);
    lv_obj_set_pos(lv_obj, 0, -50);
    lv_obj_set_align(lv_obj, LV_ALIGN_CENTER);

    lv_obj = lv_label_create(PlayBar);
    lv_obj_set_size(lv_obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_pos(lv_obj, 540, -50);
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
    lv_obj_set_style_bg_img_src(lv_obj_get_child(PlayBar, PlayMode), play_mode_image_src[CurrentPlayMode[CurrentPlayingType]], LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(lv_obj_get_child(PlayBar, PlayList), play_list_image_src[CurrentPlayingType], LV_PART_MAIN | LV_STATE_DEFAULT);
    if (CurrentPlayingType == MEDIA_TEXT)
        lv_group_focus_obj(lv_obj_get_child(PlayBar, Previous));
    else
        lv_group_focus_obj(lv_obj_get_child(PlayBar, Play));
    if (CurrentPlayingType != MEDIA_MUSIC) {
        PlayBar_Timer = lv_timer_create(PlayBarTimer_cb, (5+1)*1000, NULL);
    }
    if (CurrentPlayingType != MEDIA_TEXT) {
        PlayState_Timer = lv_timer_create(PlayedStateTimer_cb, 1000, NULL);
        lv_timer_set_repeat_count(PlayState_Timer, -1);
        lv_timer_pause(PlayState_Timer);
    }
    #ifdef HOST_GCC
    if (CurrentPlayingType != MEDIA_TEXT)
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
    lv_obj_set_size(PlayListPanel, LV_HOR_RES, 682);
    lv_obj_set_pos(PlayListPanel, 0, 705);
    lv_obj_set_align(PlayListPanel, LV_ALIGN_CENTER);
    lv_obj_clear_flag(PlayListPanel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(PlayListPanel, 40, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* PlayListInfo = lv_obj_create(PlayListPanel);
    lv_obj_set_size(PlayListInfo, 647, 64);
    lv_obj_set_pos(PlayListInfo, -300, -300);
    lv_obj_set_align(PlayListInfo, LV_ALIGN_CENTER);
    lv_obj_clear_flag(PlayListInfo, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(PlayListInfo, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(PlayListInfo, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* PlayListMode_IMG = lv_img_create(PlayListInfo);
    lv_img_set_src(PlayListMode_IMG, play_mode_image_src[CurrentPlayMode[CurrentPlayingType]]);
    lv_obj_set_size(PlayListMode_IMG, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_pos(PlayListMode_IMG, 0, 0);
    lv_obj_set_align(PlayListMode_IMG, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(PlayListMode_IMG, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(PlayListMode_IMG, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* PlayListMode_LAB = lv_label_create(PlayListInfo);
    lv_obj_set_size(PlayListMode_LAB, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_pos(PlayListMode_LAB, 79, 0);
    lv_obj_set_align(PlayListMode_LAB, LV_ALIGN_LEFT_MID);

    lv_label_set_text_fmt(PlayListMode_LAB, _p("songs", file_number), file_number);
    lv_obj_set_style_text_font(PlayListMode_LAB, &ui_font_MyFont34, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* FileListPanel = lv_obj_create(PlayListPanel);
    lv_obj_set_size(FileListPanel, LV_HOR_RES, 535);
    lv_obj_set_pos(FileListPanel, 0, -5);
    lv_obj_set_align(FileListPanel, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(FileListPanel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_bg_opa(FileListPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(FileListPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    MainGroup = create_new_group();//设置组
    set_group_activity(MainGroup);
    for(int i = 0; i < file_number; i++) {
        lv_obj_t* file_panel = lv_obj_create(FileListPanel);
        lv_obj_set_size(file_panel, 1140, 75);
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
        lv_obj_set_size(file_name, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_pos(file_name, -12, 0);
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
