/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-12 18:50:08
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-08-17 23:35:44
 * @FilePath: /gui/application/ui/media/Video.h
 * @Description: None
 * @other: None
 */
#ifndef __VIDEO_H__
#define __VIDEO_H__

#include "application/ui/ui_com.h"
#ifdef HCCHIP_GCC
#include "hcapi/media_player.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

lv_obj_t* creat_video_window(char* file_name);
void close_video_window(void);
void PlayVideo(char * file_name);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
