/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-12 18:49:59
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-06-23 14:05:30
 * @FilePath: /gui/application/ui/Video.c
 * @Description: None
 * @other: None
 */
#include <stdlib.h>
#include "Video.h"
#include "MediaCom.h"
#include "MediaFile.h"

MediaHandle* VideoHandler;
lv_obj_t* VideoScreen;
lv_obj_t* PreScreen;

static void event_handler(lv_event_t* event);
static void SetStyleForPlayBar(lv_obj_t* bar);

lv_obj_t* creat_video_window(lv_obj_t* foucsed_obj)
{
    PreScreen = lv_scr_act();
    #ifdef HOST_GCC
    VideoScreen = lv_ffmpeg_player_create(NULL);
    lv_ffmpeg_player_set_auto_restart(VideoScreen, true);
    lv_obj_center(VideoScreen);
    VideoHandler = VideoScreen;
    #elif defined(HCCHIP_GCC)
    VideoHandler = media_open(MEDIA_VIDEO);
    VideoScreen = lv_obj_create(NULL);
    lv_obj_set_style_bg_opa(VideoScreen, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    #endif
    lv_obj_add_event_cb(VideoScreen, event_handler, LV_EVENT_KEY, NULL);
    lv_obj_clear_flag(VideoScreen, LV_OBJ_FLAG_SCROLLABLE);

    lv_group_t* old_group = (lv_group_t*)lv_obj_get_group(foucsed_obj);
    MediaComInit(MEDIA_VIDEO, VideoHandler, old_group);
    CreateMediaArray(MEDIA_VIDEO);
    LocateMediaIndex(MEDIA_VIDEO, ((FileStr *)(foucsed_obj->user_data))->name);
    PlayMedia(VideoHandler, GetCurrentMediaName());

    SetStyleForPlayBar(CreatePlayBar(VideoScreen));
    lv_disp_load_scr(VideoScreen);
    return VideoScreen;
}

void close_video_window(void)
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

    MediaComDeinit();

    //step4 关闭窗口
    lv_disp_load_scr(PreScreen);
    lv_obj_del_async(VideoScreen);
}

static void event_handler(lv_event_t* event)
{
    (void)event;
    close_video_window();
}

static void SetStyleForPlayBar(lv_obj_t* bar)
{
    lv_obj_set_style_bg_color(bar, lv_color_hex(0x303030), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(bar, LV_OPA_90, LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_obj_set_style_bg_opa(bar, LV_OPA_TRANSP, 50);
    lv_obj_set_style_border_color(bar, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(bar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_text_color(lv_obj_get_child(bar, PlayedTime), lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(lv_obj_get_child(bar, TotalTime), lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
}


