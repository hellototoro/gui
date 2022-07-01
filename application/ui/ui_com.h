/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-21 12:32:26
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-06-29 18:58:54
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

extern lv_indev_t* keypad_indev;

extern void group_init(void);
extern lv_group_t* create_new_group(lv_group_t* old_group);
extern void set_group_activity(lv_group_t* group);
extern lv_group_t* delete_group(lv_group_t* group);
extern lv_indev_t* get_keypad_indev(void);
extern bool group_stack_is_empty(void);


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
