/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-12 18:50:08
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-06-18 17:31:07
 * @FilePath: /gui/application/ui/Video.h
 * @Description: None
 * @other: None
 */
#ifndef __VIDEO_H__
#define __VIDEO_H__

#include "lvgl/lvgl.h"
#ifdef HCCHIP_GCC
#include "hcapi/media_player.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

lv_obj_t* creat_video_window(lv_obj_t* foucsed_obj);
void close_video_window(lv_obj_t* video_window);
void PlayVideo(char * file_name);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
