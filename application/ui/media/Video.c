/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-12 18:49:59
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-09-11 19:55:25
 * @FilePath: /gui/application/ui/media/Video.c
 * @Description: None
 * @other: None
 */
#include <stdlib.h>
#include "Video.h"
#include "MediaCom.h"
#include "MediaFile.h"

static MediaHandle* VideoHandler;
static lv_obj_t* VideoWindow;
static lv_obj_t* BackWindow;
static lv_obj_t* PlayBar;

static void event_handler(lv_event_t* event);
static void SetStyleForPlayBar(lv_obj_t* bar);
static void UdiskStatus_event_handler(lv_event_t * event);

void creat_video_window(lv_obj_t* parent, char* file_name)
{
    BackWindow = parent;
    #ifdef HOST_GCC
    VideoWindow = lv_ffmpeg_player_create(parent);
    lv_obj_clear_flag(VideoWindow, LV_OBJ_FLAG_SCROLLABLE);
    lv_ffmpeg_player_set_auto_restart(VideoWindow, true);
    lv_obj_center(VideoWindow);
    VideoHandler = VideoWindow;
    #elif defined(HCCHIP_GCC)
    VideoHandler = media_open(MEDIA_TYPE_VIDEO, (void*)MediaMsgProc);
    VideoWindow = lv_obj_create(parent);
    lv_obj_set_style_bg_opa(VideoWindow, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    #endif
    lv_obj_add_flag(BackWindow, LV_OBJ_FLAG_HIDDEN);

    MediaComInit(MEDIA_VIDEO, VideoHandler);
    CreateMediaArray();
    LocateMediaIndex(file_name);
    PlayMedia(file_name);

    PlayBar = CreatePlayBar(lv_scr_act());
    SetStyleForPlayBar(PlayBar);
}

void close_video_window(void)
{
    //step1 停止播放
    #ifdef HOST_GCC
    lv_ffmpeg_player_set_cmd(VideoWindow, LV_FFMPEG_PLAYER_CMD_STOP);
    #elif defined(HCCHIP_GCC)
    media_stop(VideoHandler);
    media_close(VideoHandler);
    VideoHandler = NULL;
    #endif
    
    //step2 清理播放列表
    DestroyMediaArray();

    MediaComDeinit();

    //step4 关闭窗口
    lv_obj_del_async(PlayBar);
    lv_obj_del_async(VideoWindow);
    lv_obj_clear_flag(BackWindow, LV_OBJ_FLAG_HIDDEN);
}

static void SetStyleForPlayBar(lv_obj_t* bar)
{
    lv_obj_set_style_bg_color(bar, lv_color_hex(0x303030), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(bar, LV_OPA_90, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(bar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(lv_obj_get_child(bar, PlayedTime), lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(lv_obj_get_child(bar, TotalTime), lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    #ifdef USB_PLUG_TEST
    USB_PlugTest(bar);
    #endif
}


