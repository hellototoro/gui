/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-07-01 18:56:24
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-09-11 05:19:08
 * @FilePath: /gui/application/ui/media/Photo.h
 * @Description: None
 * @other: None
 */
#ifndef __PHOTO_H__
#define __PHOTO_H__

#include "application/ui/ui_com.h"
#ifdef HCCHIP_GCC
#include "hcscreen/media_player.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern void creat_photo_window(lv_obj_t* parent, char* file_name);
extern void close_photo_window(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
