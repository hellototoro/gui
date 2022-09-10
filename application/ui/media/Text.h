/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-07-05 11:13:52
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-09-11 04:52:54
 * @FilePath: /gui/application/ui/media/Text.h
 * @Description: None
 * @other: None
 */
#ifndef __TEXT_H__
#define __TEXT_H__

#include "application/ui/ui_com.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void creat_text_window(lv_obj_t* parent, char* file_name);
extern void close_text_window(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif


#endif
