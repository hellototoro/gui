/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-26 11:13:55
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-09-18 23:36:39
 * @FilePath: /gui/application/ui/SettingScreen.h
 * @Description: None
 * @other: None
 */
#ifndef __SETTING_SCREEN_H__
#define __SETTING_SCREEN_H__

#include "application/ui/ui_com.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void CreateSettingScreen(lv_obj_t* parent);
extern void SetOsdTime(int time);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
