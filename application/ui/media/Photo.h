/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-07-01 18:56:24
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-07-01 18:56:24
 * @FilePath: /gui/application/ui/Photo.h
 * @Description: None
 * @other: None
 */
#ifndef __PHOTO_H__
#define __PHOTO_H__

#ifdef HCCHIP_GCC
#include "hcapi/lvgl/lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif
#ifdef HCCHIP_GCC
#include "hcapi/media_player.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

lv_obj_t* creat_photo_window(lv_obj_t* foucsed_obj);
void close_photo_window(void);
void PlayPhoto(char * file_name);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
