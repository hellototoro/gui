/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-21 12:32:26
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-06-21 13:24:29
 * @FilePath: /gui/application/ui/ui_com.h
 * @Description: None
 * @other: None
 */
#ifndef __UI_COM_H__
#define __UI_COM_H__

#include "lvgl/lvgl.h"


#ifdef __cplusplus
extern "C" {
#endif

lv_group_t* create_new_group(lv_group_t* old_group);
void set_group_activity(lv_group_t* group, lv_indev_t* keypad_indev);
lv_group_t* delete_group(lv_group_t* group, lv_indev_t* keypad_indev);


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
