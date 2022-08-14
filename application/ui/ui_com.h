/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-21 12:32:26
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-08-15 00:45:32
 * @FilePath: /gui/application/ui/ui_com.h
 * @Description: None
 * @other: None
 */
#ifndef __UI_COM_H__
#define __UI_COM_H__

#ifdef HCCHIP_GCC
#include "hcapi/lvgl/lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif


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
extern void refresh_all_lable_text(lv_obj_t* parent);


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
