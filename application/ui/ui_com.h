/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-21 12:32:26
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-08-18 09:47:53
 * @FilePath: /gui/application/ui/ui_com.h
 * @Description: None
 * @other: None
 */
#ifndef __UI_COM_H__
#define __UI_COM_H__

#include "application/ui/resource/ui.h"
#include "lv_i18n/src/lv_i18n.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*MsgBoxFunc_t)(void);

extern lv_indev_t* keypad_indev;

extern void group_init(void);
extern lv_group_t* create_new_group(lv_group_t* old_group);
extern void set_group_activity(lv_group_t* group);
extern lv_group_t* get_activity_group(void);
extern lv_group_t* delete_group(lv_group_t* group);
extern lv_indev_t* get_keypad_indev(void);
extern bool group_stack_is_empty(void);
extern void refresh_all_lable_text(lv_obj_t* parent);
extern lv_obj_t* CreateMsgBox(lv_obj_t* parent, const char* title, MsgBoxFunc_t func);
extern void anim_callback_set_x(lv_anim_t * a, int32_t v);
extern int32_t anim_callback_get_x(lv_anim_t * a);
extern void anim_callback_set_y(lv_anim_t * a, int32_t v);
extern int32_t anim_callback_get_y(lv_anim_t * a);
extern int32_t anim_callback_get_opacity(lv_anim_t * a);
extern void anim_callback_set_opacity(lv_anim_t * a, int32_t v);


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
