/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-13 13:31:24
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-06-18 17:49:49
 * @FilePath: /gui/application/ui/MediaCom.c
 * @Description: None
 * @other: None
 */
#include <stdlib.h>
#include <string.h>
#include "MediaCom.h"
#ifdef HCCHIP_GCC
#include <sys/msg.h>
#include <ffplayer.h>
#include <hcuapi/codec_id.h>
#include "hcapi/com_api.h"
#include "hcapi/os_api.h"
#endif

char current_path[100];
lv_obj_t* PlayBar;
lv_obj_t* CurrentMediaScreen;
MediaType CurrentPlayingType;
MediaHandle* current_media_hdl;
MediaList* media_list[MEDIA_MAX];
#ifdef HOST_GCC
uint32_t played_time_host;
lv_ffmpeg_player_cmd_t play_state;
#endif
file_name* media_file_name_array;
int current_playing_index;

lv_group_t* default_group;
lv_group_t* Player_Group;
lv_timer_t* PlayBar_Timer;
lv_timer_t* PlayState_Timer;

extern lv_indev_t* keypad_indev;
LV_FONT_DECLARE(ui_font_MyFont30);
LV_FONT_DECLARE(ui_font_MyFont34);
LV_FONT_DECLARE(ui_font_MyFont38);
LV_IMG_DECLARE(ui_img_move_pause_png);    // assets\move_pause.png
LV_IMG_DECLARE(ui_img_move_next_png);    // assets\move_next.png
LV_IMG_DECLARE(ui_img_move_previous_png);    // assets\move_previous.png
LV_IMG_DECLARE(ui_img_move_play_png);    // assets\move_play.png
LV_IMG_DECLARE(ui_img_music_order_mode_png);    // assets\move_play.png
LV_IMG_DECLARE(ui_img_music_list_png);    // assets\move_play.png

#ifdef HCCHIP_GCC
static void MediaMsgProc(media_handle_t *media_hld, HCPlayerMsg *msg);
#endif

void MediaComInit(MediaType media_type, MediaHandle* media_hdl)
{
    CurrentPlayingType = media_type;
    current_media_hdl = media_hdl;
    #ifdef HCCHIP_GCC
    MediaMonitorInit(current_media_hdl);
    #endif
}

void MediaComDeinit(void)
{
    //step3 恢复默认组
    lv_group_set_default(default_group);
    lv_indev_set_group(keypad_indev, default_group);
    lv_group_remove_all_objs(Player_Group);
    lv_group_del(Player_Group);

    //step4 清除定时器
    if (CurrentPlayingType == MEDIA_VIDEO)
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

DLNode* GetNextMediaNode(MediaList* media_list, PlayListMode mode)
{
    static DLNode *next = NULL;
    static DLNode *head = NULL;
    static MediaList* last_list = NULL;

    if (media_list == NULL) {//清除获取记录
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

    if (last_list != media_list) {
        last_list = media_list;
        head = media_list->head;
        next = head->next;
    }
    else {
        next = next->next;
    }
    return next;
}

DLNode * GetPreMediaNode(MediaList* media_list, PlayListMode mode)
{
    static DLNode *pre = NULL;
    static DLNode *head = NULL;
    static MediaList* last_list = NULL;

    if (media_list == NULL) {//清除获取记录
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

    if (last_list != media_list) {
        last_list = media_list;
        head = media_list->head;
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
    media_file_name_array = (file_name*) malloc(sizeof(file_name) * n);
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

char* GetCurrentMediaName(void)
{
    return media_file_name_array[current_playing_index];
}

char* GetNextMediaName(MediaType media_type, PlayListMode mode)
{
    ++current_playing_index;
    switch (mode)
    {
    case CyclePlay:
        if (current_playing_index >= GetMediaArraySize(media_type))
            current_playing_index = 0;
        break;
    case OrderPlay:
        if (current_playing_index >= GetMediaArraySize(media_type))
            return NULL;
        break;
    case OnlyOnePlay:
        /* code */
        break;
    case RandPlay:
        /* code */
        break;
    
    default:
        break;
    }
    return media_file_name_array[current_playing_index];
}

char* GetPreMediaName(MediaType media_type, PlayListMode mode)
{
    --current_playing_index;
    switch (mode)
    {
    case CyclePlay:
        if (current_playing_index < 0)
            current_playing_index = GetMediaArraySize(media_type) - 1;
        break;
    case OrderPlay:
        if (current_playing_index < 0)
            return NULL;
        break;
    case OnlyOnePlay:
        /* code */
        break;
    case RandPlay:
        /* code */
        break;
    
    default:
        break;
    }
    return media_file_name_array[current_playing_index];
}

file_name* GetMediaArray(void)
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
        lv_ffmpeg_player_set_src(media_hal, file_path);
        lv_ffmpeg_player_set_cmd(media_hal, LV_FFMPEG_PLAYER_CMD_START);
        play_state = LV_FFMPEG_PLAYER_CMD_START;
        played_time_host = 0;
        #elif defined(HCCHIP_GCC)
        media_play(media_hal, file_path);
        #endif
    }
}

//公共ui部分
static void play_bar_event_handler(lv_event_t* event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t* target = lv_event_get_target(event);
    if (LV_EVENT_KEY == code) {
        uint32_t value = lv_indev_get_key(lv_indev_get_act());
        lv_group_t* group = (lv_group_t*)lv_obj_get_group(target);
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
                            lv_img_set_src(target, &ui_img_move_play_png);
                            #ifdef HOST_GCC
                            play_state = __MEDIA_PAUSE;
                            lv_ffmpeg_player_set_cmd(current_media_hdl, __MEDIA_PAUSE);
                            #elif defined(HCCHIP_GCC)
                            current_media_hdl->state = __MEDIA_PAUSE;
                            media_pause(current_media_hdl);
                            #endif
                        }
                        else if (state == __MEDIA_PAUSE) {
                            lv_img_set_src(target, &ui_img_move_pause_png);
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
                        
                        break;
                    case Next:
                        PlayMedia(current_media_hdl, GetNextMediaName(MEDIA_VIDEO, CyclePlay));
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
                //close_video_window(CurrentMediaScreen);
                //CurrentMediaScreen = NULL;
                lv_event_send(CurrentMediaScreen, LV_EVENT_KEY, NULL);
                return;
                break;
                
            default:
                break;
        }
        if (CurrentPlayingType == MEDIA_VIDEO) {
            if (!lv_obj_has_flag(PlayBar, LV_OBJ_FLAG_HIDDEN)) {
                lv_timer_reset(PlayBar_Timer);
            }
            else {
                lv_obj_clear_flag(PlayBar, LV_OBJ_FLAG_HIDDEN);
                lv_timer_resume(PlayBar_Timer);
            }
        }
    }
}

static void PlayBar_Timer_cb(lv_timer_t * timer)
{
    lv_obj_add_flag(PlayBar, LV_OBJ_FLAG_HIDDEN);// | LV_OBJ_FLAG_ADV_HITTEST
    lv_timer_reset(timer);
    lv_timer_pause(timer);
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
        lv_label_set_text_fmt(lv_obj_get_child(PlayBar, PlayedTime), "%02"LV_PRIu32":%02"LV_PRIu32":%02"LV_PRIu32, played_time / 3600, (played_time % 3600) / 60, played_time % 60);
        lv_slider_set_value(lv_obj_get_child(PlayBar, ProgressSlider), played_time, LV_ANIM_ON);
    }
}

lv_obj_t* CreatePlayBar(lv_obj_t* parent)
{
    static const lv_coord_t img_area[][2] = {//{ x, y}
        { -200, 15},
        { -100, 15},
        {    0, 15},
        {  100, 15},
        {  200, 15}
    };
    static const lv_img_dsc_t* image_src[] = {
        & ui_img_music_order_mode_png,
        & ui_img_move_previous_png,
        & ui_img_move_pause_png,
        & ui_img_move_next_png,
        & ui_img_music_list_png
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
    lv_obj_t* PlayedTime = lv_label_create(PlayBar);
    lv_obj_set_width(PlayedTime, LV_SIZE_CONTENT);
    lv_obj_set_height(PlayedTime, LV_SIZE_CONTENT);
    lv_obj_set_x(PlayedTime, -550);
    lv_obj_set_y(PlayedTime, -50);
    lv_obj_set_align(PlayedTime, LV_ALIGN_CENTER);
    lv_label_set_text(PlayedTime, "00:00:00");
    lv_obj_set_style_text_font(PlayedTime, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ProgressSlider
    lv_obj_t* ProgressSlider = lv_slider_create(PlayBar);
    //lv_slider_set_range(ProgressSlider, 0, 100);
    lv_slider_set_value(ProgressSlider, 0, LV_ANIM_OFF);
    if(lv_slider_get_mode(ProgressSlider) == LV_SLIDER_MODE_RANGE) lv_slider_set_left_value(ProgressSlider, 0, LV_ANIM_OFF);
    lv_obj_set_width(ProgressSlider, 900);
    lv_obj_set_height(ProgressSlider, 7);
    lv_obj_set_x(ProgressSlider, 0);
    lv_obj_set_y(ProgressSlider, -50);
    lv_obj_set_align(ProgressSlider, LV_ALIGN_CENTER);

    // TotalTime
    lv_obj_t* TotalTime = lv_label_create(PlayBar);
    lv_obj_set_width(TotalTime, LV_SIZE_CONTENT);
    lv_obj_set_height(TotalTime, LV_SIZE_CONTENT);
    lv_obj_set_x(TotalTime, 540);
    lv_obj_set_y(TotalTime, -50);
    lv_obj_set_align(TotalTime, LV_ALIGN_CENTER);
    lv_label_set_text(TotalTime, "00:00:00");
    lv_obj_set_style_text_font(TotalTime, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);

    //设置组
    default_group = lv_group_get_default();
    Player_Group = lv_group_create();
    lv_group_set_default(Player_Group);
    lv_indev_set_group(keypad_indev, Player_Group);
    lv_group_remove_all_objs(Player_Group);
    for (int i = 0; i < PlayBarNumber - PlayMode; i++) {
        // ctrl_bar
        lv_obj_t* ctrl_bar = lv_img_create(PlayBar);
        lv_img_set_src(ctrl_bar, image_src[i]);
        lv_obj_set_width(ctrl_bar, LV_SIZE_CONTENT);
        lv_obj_set_height(ctrl_bar, LV_SIZE_CONTENT);
        lv_obj_set_x(ctrl_bar, img_area[i][0]);
        lv_obj_set_y(ctrl_bar, img_area[i][1]);
        lv_obj_set_align(ctrl_bar, LV_ALIGN_CENTER);
        lv_obj_add_flag(ctrl_bar, LV_OBJ_FLAG_ADV_HITTEST);
        lv_obj_clear_flag(ctrl_bar, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_radius(ctrl_bar, 30, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_bg_color(ctrl_bar, lv_color_hex(0x08AED2), LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_bg_opa(ctrl_bar, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_group_add_obj(Player_Group, ctrl_bar);
        lv_obj_add_event_cb(ctrl_bar, play_bar_event_handler, LV_EVENT_ALL, NULL);
    }
    lv_group_focus_obj(lv_obj_get_child(PlayBar, Play));

    if (CurrentPlayingType == MEDIA_VIDEO) {
        PlayBar_Timer = lv_timer_create(PlayBar_Timer_cb, 5*1000, NULL);
    }
    PlayState_Timer = lv_timer_create(ShowPlayedState, 1000, NULL);
    lv_timer_set_repeat_count(PlayState_Timer, -1);
    CurrentMediaScreen = parent;
    return PlayBar;
}

static void SetTotalTimeAndProgress(uint32_t total_time)
{
    if (total_time == 0) total_time = 100;
    lv_label_set_text_fmt(lv_obj_get_child(PlayBar, TotalTime), "%02"LV_PRIu32":%02"LV_PRIu32":%02"LV_PRIu32, (total_time) / 3600, ((total_time) % 3600) / 60, (total_time) % 60);
    lv_slider_set_range(lv_obj_get_child(PlayBar, ProgressSlider), 0, total_time);
}

#ifdef HCCHIP_GCC
int MediaMonitorInit(media_handle_t *media_hld)
{
    //current_media_hdl = media_hld;
    media_hld->msg_id = api_message_create(CTL_MSG_COUNT, sizeof(HCPlayerMsg));
    return API_SUCCESS;
}

int MediaMonitorDeinit(media_handle_t *media_hld)
{
    if (INVALID_ID == media_hld->msg_id)
        return API_SUCCESS;
    api_message_delete(media_hld->msg_id);
    media_hld->msg_id = INVALID_ID;
    media_hld->exit = 1;
    media_hld->msg_id = INVALID_ID;
    current_media_hdl = NULL;
    return API_SUCCESS;
}

void MediaMonitorTask(void)
{
    if (current_media_hdl != NULL) {
        if (!current_media_hdl->exit) {
            HCPlayerMsg msg;
            if (msgrcv(current_media_hdl->msg_id, (void *)&msg, sizeof(HCPlayerMsg) - sizeof(msg.type), 0, 0) != -1)
            {
                MediaMsgProc(current_media_hdl, &msg);
            }
        }
    }
}

static void MediaMsgProc(media_handle_t *media_hld, HCPlayerMsg *msg)
{
    if (!media_hld || !msg) return;
    switch (msg->type)
    {
    case HCPLAYER_MSG_STATE_EOS:
        printf (">> app get eos, normal play end!\n");
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
        SetTotalTimeAndProgress(media_get_totaltime(media_hld));
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