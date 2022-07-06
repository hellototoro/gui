/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-07-05 11:13:52
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-07-05 19:40:04
 * @FilePath: /gui/application/ui/media/Text.h
 * @Description: None
 * @other: None
 */
#ifndef __TEXT_H__
#define __TEXT_H__

#ifdef HCCHIP_GCC
#include "hcapi/lvgl/lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

lv_obj_t* creat_text_window(lv_obj_t* foucsed_obj);
void LoadText(char* file_name);

#ifdef __cplusplus
} /*extern "C"*/
#endif


#endif
