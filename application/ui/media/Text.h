/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-07-05 11:13:52
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-07-14 12:55:29
 * @FilePath: /gui/application/ui/media/Text.h
 * @Description: None
 * @other: None
 */
#ifndef __TEXT_H__
#define __TEXT_H__

#include "application/ui/resource/ui.h"

#ifdef __cplusplus
extern "C" {
#endif

extern lv_obj_t* creat_text_window(lv_obj_t* foucsed_obj);
extern void LoadText(char* file_name);
extern void RefreshText(const char* text);

#ifdef __cplusplus
} /*extern "C"*/
#endif


#endif
