/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-05-23 13:51:29
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-06-04 13:18:30
 * @FilePath: /gui/application/ui/LanguageScreen.h
 * @Description: None
 * @other: None
 */
#ifndef __UI_LANGUAGE_SCREEN_H__
#define __UI_LANGUAGE_SCREEN_H__

#ifdef HCCHIP_GCC
#include "hcapi/lvgl/lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

LV_IMG_DECLARE(ui_img_install_bg_spa_png);    // assets\install_bg_eng.png
LV_IMG_DECLARE(ui_img_install_bg_gem_png);    // assets\install_bg_eng.png
LV_IMG_DECLARE(ui_img_install_bg_eng_png);    // assets\install_bg_eng.png
LV_IMG_DECLARE(ui_img_install_bg_ita_png);    // assets\install_bg_eng.png
LV_IMG_DECLARE(ui_img_install_bg_fra_png);    // assets\install_bg_eng.png
LV_IMG_DECLARE(ui_img_install_bg_chn_png);    // assets\install_bg_eng.png
LV_IMG_DECLARE(ui_img_img_arrow_down2_png);    // assets\IMG_arrow_down2.png
LV_IMG_DECLARE(ui_img_img_arrow_up2_png);    // assets\IMG_arrow_up2.png
LV_IMG_DECLARE(ui_img_install_norm_png);    // assets\install_norm.png
LV_IMG_DECLARE(ui_img_install_selected_png);    // assets\install_selected.png

LV_FONT_DECLARE(ui_font_MyFont30);
LV_FONT_DECLARE(ui_font_MyFont34);
LV_FONT_DECLARE(ui_font_MyFont38);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
