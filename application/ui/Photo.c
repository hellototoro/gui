/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-07-01 18:57:15
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-07-01 19:39:22
 * @FilePath: /gui/application/ui/Photo.c
 * @Description: None
 * @other: None
 */
#include <stdlib.h>
#include "Photo.h"
#include "MediaCom.h"
#include "MediaFile.h"

MediaHandle* PhotoHandler;
lv_obj_t* PhotoScreen;
lv_obj_t* PreScreen;

static void event_handler(lv_event_t* event);
static void SetStyleForPlayBar(lv_obj_t* bar);

lv_obj_t* creat_photo_window(lv_obj_t* foucsed_obj)
{
    PreScreen = lv_scr_act();
    #ifdef HOST_GCC
    PhotoScreen = lv_ffmpeg_player_create(NULL);
    lv_ffmpeg_player_set_auto_restart(PhotoScreen, true);
    lv_obj_center(PhotoScreen);
    PhotoHandler = PhotoScreen;
    #elif defined(HCCHIP_GCC)
    PhotoHandler = media_open(MEDIA_PHOTO);
    PhotoScreen = lv_obj_create(NULL);
    lv_obj_set_style_bg_opa(PhotoScreen, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    #endif
    lv_obj_add_event_cb(PhotoScreen, event_handler, LV_EVENT_KEY, NULL);
    lv_obj_clear_flag(PhotoScreen, LV_OBJ_FLAG_SCROLLABLE);

    lv_group_t* old_group = (lv_group_t*)lv_obj_get_group(foucsed_obj);
    MediaComInit(MEDIA_PHOTO, PhotoHandler, old_group);
    CreateMediaArray(MEDIA_PHOTO);
    LocateMediaIndex(MEDIA_PHOTO, ((FileStr *)(foucsed_obj->user_data))->name);
    PlayMedia(PhotoHandler, GetCurrentMediaName());

    SetStyleForPlayBar(CreatePlayBar(PhotoScreen));
    lv_disp_load_scr(PhotoScreen);
    return PhotoScreen;
}

void close_photo_window(void)
{
    //step1 停止播放
    #ifdef HOST_GCC
    lv_ffmpeg_player_set_cmd(PhotoScreen, LV_FFMPEG_PLAYER_CMD_STOP);
    #elif defined(HCCHIP_GCC)
    media_stop(PhotoHandler);
    MediaMonitorDeinit(PhotoHandler);
    media_close(PhotoHandler);
    PhotoHandler = NULL;
    #endif

    //step2 清理播放列表
    DestroyMediaArray();

    MediaComDeinit();

    //step4 关闭窗口
    lv_disp_load_scr(PreScreen);
    lv_obj_del_async(PhotoScreen);
}

static void event_handler(lv_event_t* event)
{
    (void)event;
    close_photo_window();
}

static void SetStyleForPlayBar(lv_obj_t* bar)
{
    lv_obj_set_height(bar, 80);
    lv_obj_set_y(bar, 320);
    lv_obj_set_style_bg_color(bar, lv_color_hex(0x303030), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(bar, LV_OPA_90, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(bar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(lv_obj_get_child(bar, PlayedTime), lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(lv_obj_get_child(bar, TotalTime), lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_y(lv_obj_get_child(bar, PlayMode), 0);
    lv_obj_set_y(lv_obj_get_child(bar, Previous), 0);
    lv_obj_set_y(lv_obj_get_child(bar, Play), 0);
    lv_obj_set_y(lv_obj_get_child(bar, Next), 0);
    lv_obj_set_y(lv_obj_get_child(bar, PlayList), 0);
    lv_obj_add_flag(lv_obj_get_child(bar, PlayedTime), LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(lv_obj_get_child(bar, ProgressSlider), LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(lv_obj_get_child(bar, TotalTime), LV_OBJ_FLAG_HIDDEN);
}


