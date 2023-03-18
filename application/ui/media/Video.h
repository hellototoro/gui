/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-12 18:50:08
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-09-11 04:15:22
 * @FilePath: /gui/application/ui/media/Video.h
 * @Description: None
 * @other: None
 */
#ifndef __VIDEO_H__
#define __VIDEO_H__

#include "application/ui/ui_com.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void creat_video_window(lv_obj_t* parent, char* file_name);
extern void close_video_window(void);
extern void PlayVideo(char * file_name);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
