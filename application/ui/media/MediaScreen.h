/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-05-23 13:51:29
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-06-11 11:21:07
 * @FilePath: /gui/application/ui/MediaScreen.h
 * @Description: None
 * @other: None
 */
#ifndef __UI_MEDIA_SCREEN_H__
#define __UI_MEDIA_SCREEN_H__

#include "lvgl/lvgl.h"
#include "hcapi/com_api.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PLAYER_BAR_H    160
#define PLAYER_BAR_W    (OSD_MAX_WIDTH-80)
#define PLAYER_BAR_X    (OSD_MAX_WIDTH-PLAYER_BAR_W)/2
#define PLAYER_BAR_Y    (OSD_MAX_HEIGHT-PLAYER_BAR_H-20)

LV_IMG_DECLARE(ui_img_arrow_right_png);    // assets\arrow_right.png
LV_IMG_DECLARE(ui_img_delivery_png);    // assets\delivery.png
LV_IMG_DECLARE(ui_img_drive_png);    // assets\drive.png
LV_IMG_DECLARE(ui_img_folder2_png);    // assets\folder2.png
LV_IMG_DECLARE(ui_img_filetype_mp4_png);    // assets\filetype_mp4.png
LV_IMG_DECLARE(ui_img_filetype_mp3_png);    // assets\filetype_mp3.png
LV_IMG_DECLARE(ui_img_filetype_jpg_png);    // assets\filetype_jpg.png
LV_IMG_DECLARE(ui_img_filetype_text_png);    // assets\filetype_text.png
LV_IMG_DECLARE(ui_img_filetype_other_png);    // assets\filetype_other.png
LV_IMG_DECLARE(ui_img_move_pause_png);    // assets\move_pause.png
LV_IMG_DECLARE(ui_img_move_next_png);    // assets\move_next.png
LV_IMG_DECLARE(ui_img_move_previous_png);    // assets\move_previous.png
LV_IMG_DECLARE(ui_img_move_play_png);    // assets\move_play.png

LV_FONT_DECLARE(ui_font_MyFont30);
LV_FONT_DECLARE(ui_font_MyFont34);
LV_FONT_DECLARE(ui_font_MyFont38);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
