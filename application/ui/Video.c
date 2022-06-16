/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-12 18:49:59
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-06-16 13:27:46
 * @FilePath: /gui/application/ui/Video.c
 * @Description: None
 * @other: None
 */
#include <stdlib.h>
#include "Video.h"
#include "MediaCom.h"
#include "MediaFile.h"

#ifdef HOST_GCC
uint32_t played_time_host;
lv_ffmpeg_player_cmd_t play_state;
#elif defined(HCCHIP_GCC)
media_handle_t *VideoHandler = NULL;
#endif

lv_obj_t* VideoScreen;
lv_obj_t* PreScreen;

lv_group_t* default_group;
lv_group_t* Player_Group;
lv_timer_t* PlayBar_Timer;
lv_timer_t* PlayState_Timer;
lv_obj_t* ui_Play_Bar_Panel;

typedef enum {
    Progress,
    Previous,
    Play,
    Next,
    PlayedTime,
    TotalTime,
    PlayBarNumber
} PlayBarList;

static void PlayBar_Timer_cb(lv_timer_t * timer);
static void play_bar_event_handler(lv_event_t* event);
static void ShowPlayedState(lv_timer_t * timer);
static void CreateMoveBarPanel(lv_obj_t* parent);
void CreatVideoArray(void);
int LocateVideo(char * file_name);

extern lv_indev_t* keypad_indev;
LV_IMG_DECLARE(ui_img_move_pause_png);    // assets\move_pause.png
LV_IMG_DECLARE(ui_img_move_next_png);    // assets\move_next.png
LV_IMG_DECLARE(ui_img_move_previous_png);    // assets\move_previous.png
LV_IMG_DECLARE(ui_img_move_play_png);    // assets\move_play.png
LV_FONT_DECLARE(ui_font_MyFont30);
LV_FONT_DECLARE(ui_font_MyFont34);
LV_FONT_DECLARE(ui_font_MyFont38);

lv_obj_t* creat_video_window(lv_obj_t* foucsed_obj)
{
    PreScreen = lv_scr_act();
    #ifdef HOST_GCC
    VideoScreen = lv_ffmpeg_player_create(NULL);
    lv_ffmpeg_player_set_auto_restart(VideoScreen, true);
    lv_obj_center(VideoScreen);
    #elif defined(HCCHIP_GCC)
    VideoHandler = media_open(MEDIA_VIDEO);
    MediaMonitorInit(VideoHandler);
    VideoScreen = lv_obj_create(NULL);
    lv_obj_set_style_bg_opa(VideoScreen, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(VideoScreen, LV_OBJ_FLAG_SCROLLABLE);
    #endif
    lv_obj_clear_flag(VideoScreen, LV_OBJ_FLAG_SCROLLABLE);
    default_group = lv_group_get_default();
    Player_Group = lv_group_create();
    lv_group_set_default(Player_Group);
    lv_indev_set_group(keypad_indev, Player_Group);
    lv_disp_load_scr(VideoScreen);
    CreatMediaArray(MEDIA_VIDEO);
    LocateMediaIndex(((FileStr *)(foucsed_obj->user_data))->name);
    PlayVideo(GetCurrentMediaName());
    CreateMoveBarPanel(VideoScreen);
    return VideoScreen;

}

void close_video_window(lv_obj_t* video_window)
{
    //step1 停止播放
    #ifdef HOST_GCC
    lv_ffmpeg_player_set_cmd(VideoScreen, LV_FFMPEG_PLAYER_CMD_STOP);
    #elif defined(HCCHIP_GCC)
    media_stop(VideoHandler);
    MediaMonitorDeinit(VideoHandler);
    media_close(VideoHandler);
    VideoHandler = NULL;
    #endif

    //step2 清理播放列表
    DestroyMediaArray();

    //step3 恢复默认组
    lv_group_set_default(default_group);
    lv_indev_set_group(keypad_indev, default_group);
    lv_group_remove_all_objs(Player_Group);
    lv_group_del(Player_Group);

    //step4 清除定时器
    lv_timer_del(PlayBar_Timer);
    lv_timer_del(PlayState_Timer);

    //step4 关闭窗口
    lv_disp_load_scr(PreScreen);
    lv_obj_del_async(video_window);
}

static void ShowPlayedState(lv_timer_t * timer)
{
    #ifdef HOST_GCC
    if (play_state == LV_FFMPEG_PLAYER_CMD_START)
        played_time_host++;
    #endif
    if (!lv_obj_has_flag(ui_Play_Bar_Panel, LV_OBJ_FLAG_HIDDEN)) {
        #ifdef HOST_GCC
        uint32_t played_time = played_time_host;
        #elif defined(HCCHIP_GCC)
        uint32_t played_time = 0;
        if (VideoHandler != NULL)
            played_time = media_get_playtime(VideoHandler);
        #endif
        lv_label_set_text_fmt(lv_obj_get_child(ui_Play_Bar_Panel, PlayedTime), "%02"LV_PRIu32":%02"LV_PRIu32":%02"LV_PRIu32, played_time / 3600, (played_time % 3600) / 60, played_time % 60);
        lv_slider_set_value(lv_obj_get_child(ui_Play_Bar_Panel, Progress), played_time, LV_ANIM_ON);
    }
}

static void CreateMoveBarPanel(lv_obj_t* parent)
{
    static const lv_coord_t img_area[][2] = {//{ x, y}
        { -100, 20},
        {    0, 20},
        {  100, 20}
    };
    static const lv_img_dsc_t* image_src[] = {
        & ui_img_move_previous_png,
        & ui_img_move_pause_png,
        & ui_img_move_next_png
    };

    // ui_Play_Bar_Panel
    ui_Play_Bar_Panel = lv_obj_create(parent);
    lv_obj_set_width(ui_Play_Bar_Panel, 1260);
    lv_obj_set_height(ui_Play_Bar_Panel, 140);
    lv_obj_set_x(ui_Play_Bar_Panel, 0);
    lv_obj_set_y(ui_Play_Bar_Panel, 300);
    lv_obj_set_align(ui_Play_Bar_Panel, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_Play_Bar_Panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_Play_Bar_Panel, lv_color_hex(0x303030), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Play_Bar_Panel, LV_OPA_90, LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_obj_set_style_bg_opa(ui_Play_Bar_Panel, LV_OPA_TRANSP, 50);
    lv_obj_set_style_border_color(ui_Play_Bar_Panel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_Play_Bar_Panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_group_add_obj(Player_Group, ui_Play_Bar_Panel);

    // ui_Progress_Bar
    lv_obj_t* ui_Progress_Bar = lv_slider_create(ui_Play_Bar_Panel);
    lv_slider_set_value(ui_Progress_Bar, 0, LV_ANIM_OFF);
    if(lv_slider_get_mode(ui_Progress_Bar) == LV_SLIDER_MODE_RANGE) lv_slider_set_left_value(ui_Progress_Bar, 0, LV_ANIM_OFF);
    lv_slider_set_range(ui_Progress_Bar, 0, 100);
    lv_obj_set_width(ui_Progress_Bar, 900);
    lv_obj_set_height(ui_Progress_Bar, 28);
    lv_obj_set_x(ui_Progress_Bar, 0);
    lv_obj_set_y(ui_Progress_Bar, -35);
    lv_obj_set_align(ui_Progress_Bar, LV_ALIGN_CENTER);
    SetMediaProgress(ui_Progress_Bar);
    //lv_group_add_obj(Player_Group, ui_Progress_Bar);

    lv_group_remove_all_objs(Player_Group);
    for (int i = 0; i < Next; i++) {
        // ui_Move_Pause
        lv_obj_t* ui_Move_Pause = lv_img_create(ui_Play_Bar_Panel);
        lv_img_set_src(ui_Move_Pause, image_src[i]);
        lv_obj_set_width(ui_Move_Pause, LV_SIZE_CONTENT);
        lv_obj_set_height(ui_Move_Pause, LV_SIZE_CONTENT);
        lv_obj_set_x(ui_Move_Pause, img_area[i][0]);
        lv_obj_set_y(ui_Move_Pause, img_area[i][1]);
        lv_obj_set_align(ui_Move_Pause, LV_ALIGN_CENTER);
        lv_obj_add_flag(ui_Move_Pause, LV_OBJ_FLAG_ADV_HITTEST);
        lv_obj_clear_flag(ui_Move_Pause, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_radius(ui_Move_Pause, 30, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_bg_color(ui_Move_Pause, lv_color_hex(0x08AED2), LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_bg_opa(ui_Move_Pause, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_group_add_obj(Player_Group, ui_Move_Pause);
        lv_obj_add_event_cb(ui_Move_Pause, play_bar_event_handler, LV_EVENT_ALL, NULL);
    }

    // ui_Lab_Played_Time
    lv_obj_t* ui_Lab_Played_Time = lv_label_create(ui_Play_Bar_Panel);
    lv_obj_set_width(ui_Lab_Played_Time, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_Lab_Played_Time, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_Lab_Played_Time, -550);
    lv_obj_set_y(ui_Lab_Played_Time, -35);
    lv_obj_set_align(ui_Lab_Played_Time, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Lab_Played_Time, "00:00:00");
    lv_obj_set_style_text_color(ui_Lab_Played_Time, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_Lab_Played_Time, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_Lab_Total_Time
    lv_obj_t* ui_Lab_Total_Time = lv_label_create(ui_Play_Bar_Panel);
    lv_obj_set_width(ui_Lab_Total_Time, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_Lab_Total_Time, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_Lab_Total_Time, 540);
    lv_obj_set_y(ui_Lab_Total_Time, -35);
    lv_obj_set_align(ui_Lab_Total_Time, LV_ALIGN_CENTER);
    lv_obj_set_style_text_color(ui_Lab_Total_Time, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text(ui_Lab_Total_Time, "00:00:00");
    lv_obj_set_style_text_font(ui_Lab_Total_Time, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);
    SetMediaTotalTime(ui_Lab_Total_Time);

    lv_group_focus_obj(lv_obj_get_child(ui_Play_Bar_Panel, Play));
    PlayBar_Timer = lv_timer_create(PlayBar_Timer_cb, 5*1000, NULL);

    PlayState_Timer = lv_timer_create(ShowPlayedState, 1000, NULL);
    lv_timer_set_repeat_count(PlayState_Timer, -1);
}

void PlayVideo(char * file_name)
{
    if (file_name != NULL) {
        char file_path[100] = {0};
        strcat(file_path, current_path);
        strcat(file_path, "/");
        strcat(file_path,  file_name);
        #ifdef HOST_GCC
        lv_ffmpeg_player_set_src(VideoScreen, file_path);
        lv_ffmpeg_player_set_cmd(VideoScreen, LV_FFMPEG_PLAYER_CMD_START);
        play_state = LV_FFMPEG_PLAYER_CMD_START;
        played_time_host = 0;
        #elif defined(HCCHIP_GCC)
        media_play(VideoHandler, file_path);
        #endif
    }
}

static void PlayBar_Timer_cb(lv_timer_t * timer)
{
    lv_obj_add_flag(ui_Play_Bar_Panel, LV_OBJ_FLAG_HIDDEN);// | LV_OBJ_FLAG_ADV_HITTEST
    lv_timer_reset(timer);
    lv_timer_pause(timer);
}

static void play_bar_event_handler(lv_event_t* event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t* target = lv_event_get_target(event);
    //lv_obj_t* parents = lv_obj_get_parent(target);
    if (LV_EVENT_KEY == code) {
        uint32_t value = lv_indev_get_key(lv_indev_get_act());
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
                        media_state_t state = VideoHandler->state;
                        #define __MEDIA_PLAY    MEDIA_PLAY
                        #define __MEDIA_PAUSE   MEDIA_PAUSE
                        #endif
                        if (state == __MEDIA_PLAY) {
                            lv_img_set_src(target, &ui_img_move_play_png);
                            #ifdef HOST_GCC
                            play_state = __MEDIA_PAUSE;
                            lv_ffmpeg_player_set_cmd(VideoScreen, __MEDIA_PAUSE);
                            #elif defined(HCCHIP_GCC)
                            VideoHandler->state = __MEDIA_PAUSE;
                            media_pause(VideoHandler);
                            #endif
                        }
                        else if (state == __MEDIA_PAUSE) {
                            lv_img_set_src(target, &ui_img_move_pause_png);
                            #ifdef HOST_GCC
                            play_state = __MEDIA_PLAY;
                            lv_ffmpeg_player_set_cmd(VideoScreen, LV_FFMPEG_PLAYER_CMD_RESUME);
                            #elif defined(HCCHIP_GCC)
                            VideoHandler->state = __MEDIA_PLAY;
                            media_resume(VideoHandler);
                            #endif
                        }
                    }
                    break;
                    case Previous:
                        
                        break;
                    case Next:
                        PlayVideo(GetNextMediaName(CyclePlay));
                        break;
                    
                    default:
                        break;
                }
                break;
        
            case LV_KEY_LEFT:
                lv_group_focus_prev(Player_Group);
                break;
            case LV_KEY_RIGHT:
                lv_group_focus_next(Player_Group);
                break;
            case LV_KEY_ESC:
                //lv_event_send(PreScreen, LV_EVENT_KEY, NULL);
                close_video_window(VideoScreen);
                VideoScreen = NULL;
                return;
                break;
                
            default:
                break;
        }
        if (!lv_obj_has_flag(ui_Play_Bar_Panel, LV_OBJ_FLAG_HIDDEN)) {
            lv_timer_reset(PlayBar_Timer);
        }
        else {
            lv_obj_clear_flag(ui_Play_Bar_Panel, LV_OBJ_FLAG_HIDDEN);
            lv_timer_resume(PlayBar_Timer);
        }

    }
}
