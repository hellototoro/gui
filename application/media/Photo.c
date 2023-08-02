/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-07-01 18:57:15
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-09-11 15:27:06
 * @FilePath: /gui/application/ui/media/Photo.c
 * @Description: None
 * @other: None
 */
#include <stdlib.h>
#include "Photo.h"
#include "MediaCom.h"
#include "MediaFile.h"

static MediaHandle* PhotoHandler;
static lv_obj_t* PhotoWindow;
static lv_obj_t* BackWindow;
static lv_obj_t* PlayBar;

static void event_handler(lv_event_t* event);
static void SetStyleForPlayBar(lv_obj_t* bar);

void creat_photo_window(lv_obj_t* parent, char* file_name)
{
    BackWindow = parent;
    // PhotoWindow = lv_ffmpeg_player_create(parent);
    // lv_obj_clear_flag(PhotoWindow, LV_OBJ_FLAG_SCROLLABLE);
    // lv_ffmpeg_player_set_auto_restart(PhotoWindow, true);
    // lv_obj_center(PhotoWindow);
    // PhotoHandler = PhotoWindow;

    MediaComInit(MEDIA_PHOTO, PhotoHandler);
    CreateMediaArray();
    LocateMediaIndex(file_name);
    PlayMedia(file_name);

    PlayBar = CreatePlayBar(lv_scr_act());
    SetStyleForPlayBar(PlayBar);
}

void close_photo_window(void)
{
    //step1 停止播放
    // lv_ffmpeg_player_set_cmd(PhotoWindow, LV_FFMPEG_PLAYER_CMD_STOP);

    //step2 清理播放列表
    DestroyMediaArray();

    MediaComDeinit();

    //step4 关闭窗口
    lv_obj_del_async(PlayBar);
    lv_obj_del_async(PhotoWindow);
    lv_obj_clear_flag(BackWindow, LV_OBJ_FLAG_HIDDEN);
}

static void SetStyleForPlayBar(lv_obj_t* bar)
{
    lv_obj_set_height(bar, 100);
    lv_obj_set_y(bar, 310);
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


