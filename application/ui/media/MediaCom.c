/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-13 13:31:24
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-07-19 17:37:25
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
#include "id3v2lib/include/id3v2lib.h"
#include "application/ui/ui_com.h"
#include "application/ui/Volume.h"
#include "application/key_map.h"

char current_path[100];
int current_path_size = sizeof(current_path);
lv_obj_t* PlayBar;
lv_obj_t* PlayListPanel;
lv_obj_t* CurrentMediaScreen;
MediaType CurrentPlayingType;
PlayListMode CurrentPlayMode;
MediaHandle* current_media_hdl;
MediaList* media_list[MEDIA_MAX];
#ifdef HOST_GCC
uint32_t played_time_host;
lv_ffmpeg_player_cmd_t play_state;
#endif
file_name_t* media_file_name_array;
int current_playing_index;
bool PlayingAnimation_Flag;

lv_group_t* Old_Group;
lv_group_t* Player_Group;
lv_timer_t* PlayBar_Timer;
lv_timer_t* PlayState_Timer;

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

#ifdef HCCHIP_GCC
static void MediaMsgProc(media_handle_t *media_hld, HCPlayerMsg *msg);
#endif
MediaList* GetMediaList(MediaType media_type);
DLNode * GetNextMediaNode(MediaList* list, PlayListMode mode);
DLNode * GetPreMediaNode(MediaList* list, PlayListMode mode);
/************动画*************/
static void anim_callback_set_y(lv_anim_t * a, int32_t v);
static int32_t anim_callback_get_y(lv_anim_t * a);
static void ShowUpAnimation(lv_obj_t * TargetObject, int delay);
static void ShowDownAnimation(lv_obj_t * TargetObject, int delay);
static void ShowOnPlayList(lv_obj_t *screen, file_name_t* name_list, int file_number);
static void ShowOffPlayList(void);
static void SetTotalTimeAndProgress(uint32_t total_time);

void MediaComInit(MediaType media_type, MediaHandle* media_hdl, lv_group_t* old_group)
{
    CurrentPlayingType = media_type;
    current_media_hdl = media_hdl;
    CurrentPlayMode = RandPlay;
    #ifdef HCCHIP_GCC
    //MediaMonitorInit(current_media_hdl);
    #endif
    srand(time(0));
    PlayingAnimation_Flag = false;

    //设置组
    Old_Group = old_group;
    Player_Group = lv_group_create();
    set_group_activity(Player_Group);
}

void MediaComDeinit(void)
{
    //step3 恢复默认组
    set_group_activity(Old_Group);
    lv_group_del(Player_Group);

    //step4 清除定时器
    if (CurrentPlayingType == MEDIA_VIDEO || CurrentPlayingType == MEDIA_PHOTO)
        lv_timer_del(PlayBar_Timer);
    lv_timer_del(PlayState_Timer);
}

MediaList* CreateMediaList(MediaType media_type)
{
    media_list[media_type] = (MediaList*) malloc(sizeof(MediaList));
    InitDList(media_list[media_type]);
    return media_list[media_type];
}

bool MediaListIsEmpty(MediaType media_type)
{
    return (media_list[media_type]->len ==0 ) ? true : false;
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

void DestroyMediaList(MediaType media_type)
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

void CreateMediaArray(MediaType media_type)
{
    int n = GetMediaListSize(media_type);
    if (n == 0) return;
    DestroyMediaArray();
    current_playing_index = 0;
    media_file_name_array = (file_name_t*) malloc(sizeof(file_name_t) * n);
    GetNextMediaNode(NULL, OrderPlay);
    for (int i = 0; i < n; i++) {
        media_file_name_array[i] = (char*) (GetNextMediaNode(GetMediaList(media_type), OrderPlay))->data;
    }
}

uint16_t GetMediaArraySize(MediaType media_type)
{
    return GetMediaListSize(media_type);
}

uint16_t LocateMediaIndex(MediaType media_type, char * file_name)
{
    int i;
    uint16_t n = GetMediaArraySize(media_type);
    for (i = 0; i < n; i++) {
        if (strcmp(media_file_name_array[i], file_name) == 0) {
            current_playing_index = i;
            break;
        }
    }
    return current_playing_index;
}

void SetMediaIndex(int index)
{
    if (index < GetMediaArraySize(CurrentPlayingType))
        current_playing_index = index;
}

int GetMediaIndex(void)
{
    return current_playing_index;
}

char* GetCurrentMediaName(void)
{
    return media_file_name_array[current_playing_index];
}

char* GetNextMediaName(MediaType media_type, PlayListMode mode, GetNextMode next_mode)
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

char* GetPreMediaName(MediaType media_type, PlayListMode mode)
{
    (void)mode;
    --current_playing_index;
    if (current_playing_index < 0)
        current_playing_index = GetMediaArraySize(media_type) - 1;
    return media_file_name_array[current_playing_index];
}

file_name_t* GetMediaArray(void)
{
    return media_file_name_array;
}

void DestroyMediaArray(void)
{
    if (media_file_name_array != NULL) {
        free(media_file_name_array);
        media_file_name_array = NULL;
    }
}

void PlayMedia(MediaHandle* media_hal, char * file_name)
{
    if (file_name != NULL) {
        char file_path[100] = {0};
        strcat(file_path, current_path);
        strcat(file_path, "/");
        strcat(file_path,  file_name);
        #ifdef HOST_GCC
        #if 0
        ID3v2_tag* tag = load_tag(file_path); // Load the full tag from the file
        if(tag == NULL)
        {
            tag = new_tag();
        }
        else {
            // Load the fields from the tag
            ID3v2_frame* artist_frame = tag_get_artist(tag); // Get the full artist frame
            // We need to parse the frame content to make readable
            ID3v2_frame_text_content* artist_content = parse_text_frame_content(artist_frame);
            if (artist_content)
                printf("ARTIST: %s\n", artist_content->data); // Show the artist info

            ID3v2_frame* title_frame = tag_get_title(tag);
            ID3v2_frame_text_content* title_content = parse_text_frame_content(title_frame);
            if (title_content) {
                printf("TITLE: %s\n", title_content->data);
                SetCurrentMusicTitle(title_content->data);
            }

            ID3v2_frame* cover_frame = tag_get_album_cover(tag);
            ID3v2_frame_text_content* cover_content = parse_text_frame_content(cover_frame);
            if (cover_frame) {
                lv_img_dsc_t ui_img_cover = {
                    .header.always_zero = 0,
                    .header.w = 256,
                    .header.h = 256,
                    .data_size = cover_frame->size,
                    .header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
                    .data = cover_frame->data
                };
                SetCurrentMusicCover(&ui_img_cover);
                printf("size: %d\n", cover_frame->size);
            }
        }
        #else
        lv_ffmpeg_player_set_src(media_hal, file_path);
        lv_ffmpeg_player_set_cmd(media_hal, LV_FFMPEG_PLAYER_CMD_START);
        #endif
        play_state = LV_FFMPEG_PLAYER_CMD_START;
        played_time_host = 0;
        #elif defined(HCCHIP_GCC)
        if (media_hal->state == MEDIA_PLAY)
            media_stop(media_hal);
        media_play(media_hal, file_path);
        #endif
        if(CurrentPlayingType == MEDIA_MUSIC) {
            SetCurrentMusicTitle(file_name);
            LoadLyric(file_name);
        }
    }
}

//公共ui部分
static void key_event_handler(lv_event_t* event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t* target = lv_event_get_target(event);
    if (LV_EVENT_KEY == code) {
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
                        PlayMedia(current_media_hdl, GetPreMediaName(CurrentPlayingType, CurrentPlayMode));
                        break;
                    case Next:
                        PlayMedia(current_media_hdl, GetNextMediaName(CurrentPlayingType, CurrentPlayMode, ManualPlay));
                        break;
                    case PlayList:
                        ShowOnPlayList(CurrentMediaScreen, GetMediaArray(), GetMediaArraySize(CurrentPlayingType));
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
                    lv_event_send(CurrentMediaScreen, LV_EVENT_KEY, NULL);
                    return;
                }
                break;

            case LV_KEY_VOLUME_UP:
            case LV_KEY_VOLUME_DOWN:
                SetVolume(value);
                return;

            default:
                break;
        }
        if (CurrentPlayingType == MEDIA_VIDEO || CurrentPlayingType == MEDIA_PHOTO) {
            if (!lv_obj_has_flag(PlayBar, LV_OBJ_FLAG_HIDDEN)) {
                lv_timer_reset(PlayBar_Timer);
            }
        }
    }
}

static void play_list_event_handler(lv_event_t* event)
{
    lv_obj_t* target = lv_event_get_target(event);
    lv_group_t* group = (lv_group_t*)lv_obj_get_group(target);
    uint32_t value = lv_indev_get_key(lv_indev_get_act());
    switch (value)
    {
        case LV_KEY_ENTER:
            SetMediaIndex(lv_obj_get_index(target));
            PlayMedia(current_media_hdl, GetCurrentMediaName());
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
        if (played_time > lv_slider_get_max_value(lv_obj_get_child(PlayBar, ProgressSlider))) {
            lv_timer_pause(timer);
            PlayMedia(current_media_hdl, GetNextMediaName(CurrentPlayingType, CurrentPlayMode, AutoPlay));
            #ifdef HOST_GCC
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

    // PlayBar
    PlayBar = lv_obj_create(parent);
    lv_obj_set_width(PlayBar, 1260);
    lv_obj_set_height(PlayBar, 150);
    lv_obj_set_x(PlayBar, 0);
    lv_obj_set_y(PlayBar, 300);
    lv_obj_set_align(PlayBar, LV_ALIGN_CENTER);
    lv_obj_clear_flag(PlayBar, LV_OBJ_FLAG_SCROLLABLE);

    // PlayedTime
    lv_obj = lv_label_create(PlayBar);
    lv_obj_set_width(lv_obj, LV_SIZE_CONTENT);
    lv_obj_set_height(lv_obj, LV_SIZE_CONTENT);
    lv_obj_set_x(lv_obj, -550);
    lv_obj_set_y(lv_obj, -50);
    lv_obj_set_align(lv_obj, LV_ALIGN_CENTER);
    lv_label_set_text(lv_obj, "00:00:00");
    lv_obj_set_style_text_font(lv_obj, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ProgressSlider
    lv_obj = lv_slider_create(PlayBar);
    //lv_slider_set_range(ProgressSlider, 0, 100);
    lv_slider_set_value(lv_obj, 0, LV_ANIM_OFF);
    if(lv_slider_get_mode(lv_obj) == LV_SLIDER_MODE_RANGE) lv_slider_set_left_value(lv_obj, 0, LV_ANIM_OFF);
    lv_obj_set_width(lv_obj, 900);
    lv_obj_set_height(lv_obj, 7);
    lv_obj_set_x(lv_obj, 0);
    lv_obj_set_y(lv_obj, -50);
    lv_obj_set_align(lv_obj, LV_ALIGN_CENTER);

    // TotalTime
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

        lv_group_add_obj(Player_Group, ctrl_bar);
        lv_obj_add_event_cb(ctrl_bar, key_event_handler, LV_EVENT_ALL, NULL);
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
    CurrentMediaScreen = parent;
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
    // PlayListPanel
    PlayListPanel = lv_obj_create(parent);
    lv_obj_set_width(PlayListPanel, 1280);
    lv_obj_set_height(PlayListPanel, 682);
    lv_obj_set_x(PlayListPanel, 0);
    lv_obj_set_y(PlayListPanel, 705);
    lv_obj_set_align(PlayListPanel, LV_ALIGN_CENTER);
    //lv_obj_add_flag(PlayListPanel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(PlayListPanel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(PlayListPanel, 40, LV_PART_MAIN | LV_STATE_DEFAULT);

    // PlayListInfo
    lv_obj_t* PlayListInfo = lv_obj_create(PlayListPanel);
    lv_obj_set_width(PlayListInfo, 647);
    lv_obj_set_height(PlayListInfo, 64);
    lv_obj_set_x(PlayListInfo, -300);
    lv_obj_set_y(PlayListInfo, -300);
    lv_obj_set_align(PlayListInfo, LV_ALIGN_CENTER);
    lv_obj_clear_flag(PlayListInfo, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(PlayListInfo, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(PlayListInfo, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // PlayListMode_IMG
    lv_obj_t* PlayListMode_IMG = lv_img_create(PlayListInfo);
    //lv_img_set_src(PlayListMode_IMG, &ui_img_music_order_mode_png);
    lv_img_set_src(PlayListMode_IMG, play_mode_image_src[CurrentPlayMode]);
    lv_obj_set_width(PlayListMode_IMG, LV_SIZE_CONTENT);
    lv_obj_set_height(PlayListMode_IMG, LV_SIZE_CONTENT);
    lv_obj_set_x(PlayListMode_IMG, 0);
    lv_obj_set_y(PlayListMode_IMG, 0);
    lv_obj_set_align(PlayListMode_IMG, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(PlayListMode_IMG, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(PlayListMode_IMG, LV_OBJ_FLAG_SCROLLABLE);

    // PlayListMode_LAB
    lv_obj_t* PlayListMode_LAB = lv_label_create(PlayListInfo);
    lv_obj_set_width(PlayListMode_LAB, LV_SIZE_CONTENT);
    lv_obj_set_height(PlayListMode_LAB, LV_SIZE_CONTENT);
    lv_obj_set_x(PlayListMode_LAB, 79);
    lv_obj_set_y(PlayListMode_LAB, 0);
    lv_obj_set_align(PlayListMode_LAB, LV_ALIGN_LEFT_MID);
    //lv_label_set_text(PlayListMode_LAB, "顺序播放（276首）");
    lv_label_set_text_fmt(PlayListMode_LAB, "顺序播放（%d首）", file_number);
    lv_obj_set_style_text_font(PlayListMode_LAB, &ui_font_MyFont34, LV_PART_MAIN | LV_STATE_DEFAULT);

    // FileListPanel
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
    Player_Group = create_new_group(Player_Group);
    set_group_activity(Player_Group);
    for(int i = 0; i < file_number; i++) {
        // file_panel
        lv_obj_t* file_panel = lv_obj_create(FileListPanel);
        lv_obj_set_width(file_panel, 1140);
        lv_obj_set_height(file_panel, 75);
        //lv_obj_set_x(file_panel, 0);
        //lv_obj_set_y(file_panel, -275 + i * 90);
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
        lv_group_add_obj(Player_Group, file_panel);
        lv_obj_add_event_cb(file_panel, play_list_event_handler, LV_EVENT_KEY, NULL);

        // file_name
        lv_obj_t* file_name = lv_label_create(file_panel);
        lv_obj_set_width(file_name, LV_SIZE_CONTENT);
        lv_obj_set_height(file_name, LV_SIZE_CONTENT);
        lv_obj_set_x(file_name, -12);
        lv_obj_set_y(file_name, 0);
        lv_label_set_text(file_name, name_list[i]);
        lv_obj_set_style_text_font(file_name, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);

        // file_info
        /*lv_obj_t* file_info = lv_label_create(file_panel);
        lv_obj_set_width(file_info, LV_SIZE_CONTENT);
        lv_obj_set_height(file_info, LV_SIZE_CONTENT);
        lv_obj_set_x(file_info, -10);
        lv_obj_set_y(file_info, 20);
        lv_obj_set_align(file_info, LV_ALIGN_BOTTOM_LEFT);
        lv_label_set_text(file_info, "");
        lv_obj_set_style_text_color(file_info, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(file_info, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(file_info, &ui_font_MyFont24, LV_PART_MAIN | LV_STATE_DEFAULT);*/
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
static void anim_callback_set_y(lv_anim_t * a, int32_t v)
{
    lv_obj_set_y((lv_obj_t *)a->user_data, v);
}

static int32_t anim_callback_get_y(lv_anim_t * a)
{
    return lv_obj_get_y_aligned((lv_obj_t *)a->user_data);
}

static void anim_callback_delete_obj(struct _lv_anim_t *a)
{
    Player_Group = delete_group(Player_Group);
    lv_obj_del_async((lv_obj_t *)a->user_data);
    PlayingAnimation_Flag = false;
}

static void ShowUpAnimation(lv_obj_t * TargetObject, int delay)
{
    lv_anim_t PropertyAnimation;
    lv_anim_init(&PropertyAnimation);
    lv_anim_set_time(&PropertyAnimation, 500);
    lv_anim_set_user_data(&PropertyAnimation, TargetObject);
    lv_anim_set_custom_exec_cb(&PropertyAnimation, anim_callback_set_y);
    lv_anim_set_values(&PropertyAnimation, lv_obj_get_y(TargetObject), -605);
    lv_anim_set_path_cb(&PropertyAnimation, lv_anim_path_ease_in_out);
    lv_anim_set_delay(&PropertyAnimation, delay + 0);
    lv_anim_set_early_apply(&PropertyAnimation, false);
    lv_anim_set_get_value_cb(&PropertyAnimation, &anim_callback_get_y);
    lv_anim_start(&PropertyAnimation);
}

static void ShowDownAnimation(lv_obj_t * TargetObject, int delay)
{
    lv_anim_t PropertyAnimation;
    lv_anim_init(&PropertyAnimation);
    lv_anim_set_time(&PropertyAnimation, 500);
    lv_anim_set_user_data(&PropertyAnimation, TargetObject);
    lv_anim_set_custom_exec_cb(&PropertyAnimation, anim_callback_set_y);
    lv_anim_set_values(&PropertyAnimation, lv_obj_get_y(TargetObject), 605);
    lv_anim_set_path_cb(&PropertyAnimation, lv_anim_path_overshoot);
    lv_anim_set_delay(&PropertyAnimation, delay + 0);
    lv_anim_set_early_apply(&PropertyAnimation, false);
    lv_anim_set_get_value_cb(&PropertyAnimation, &anim_callback_get_y);
    lv_anim_set_deleted_cb(&PropertyAnimation, anim_callback_delete_obj);
    lv_anim_start(&PropertyAnimation);
}

//SetTotalTimeAndProgress(media_get_totaltime(media_hld));
