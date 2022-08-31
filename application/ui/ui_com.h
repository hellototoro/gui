/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-21 12:32:26
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-08-31 14:45:49
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

typedef struct ObjNavigation
{
    lv_key_t dir;
    lv_obj_t* obj[4];
}ObjNavigation_t;

typedef void (*MsgBoxFunc_t)(void);

extern lv_indev_t* keypad_indev;

extern void group_init(void);
extern lv_group_t* create_new_group(void);
extern void set_group_activity(lv_group_t* group);
extern lv_group_t* get_activity_group(void);
extern lv_group_t* delete_group(lv_group_t* group);
extern void delete_all_group(void);
extern lv_indev_t* get_keypad_indev(void);
extern void refresh_all_lable_text(lv_obj_t* parent);
extern lv_obj_t* CreateMsgBox(lv_obj_t* parent, const char* title, MsgBoxFunc_t func);
extern lv_obj_t* CreateSpinBox(lv_obj_t* parent, const char* title, int time_s, MsgBoxFunc_t func);
extern void anim_callback_set_x(lv_anim_t * a, int32_t v);
extern int32_t anim_callback_get_x(lv_anim_t * a);
extern void anim_callback_set_y(lv_anim_t * a, int32_t v);
extern int32_t anim_callback_get_y(lv_anim_t * a);
extern int32_t anim_callback_get_opacity(lv_anim_t * a);
extern void anim_callback_set_opacity(lv_anim_t * a, int32_t v);
extern void anim_callback_set_image_angle(void * var, int32_t v);
extern int32_t anim_callback_get_image_angle(lv_anim_t * a);
extern void SpinAnimation(lv_obj_t * TargetObject, int delay, int time);
extern void SpinAnimationStop(lv_obj_t * TargetObject);


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
