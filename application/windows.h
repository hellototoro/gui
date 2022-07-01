/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-05-22 18:12:45
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-06-30 14:36:30
 * @FilePath: /gui/application/windows.h
 * @Description: None
 * @other: None
 */
#ifndef __APPLICATION_WINDOWS_H__
#define __APPLICATION_WINDOWS_H__

#include "lvgl/lvgl.h"

typedef enum  {
    LanguageScreen,
    HomeScreen,
    MediaScreen,
    SettingScreen,
    ScreenNumber
} ActiveScreen;

typedef struct window
{
	void (*ScreenInit) (lv_obj_t* parent, void *param);
    void (*ScreenLoad) (void);
    void (*ScreenClose) (void);
    void *param;
}window;


extern ActiveScreen CurrentScreen;
extern window *CurrentWindow;
extern window *Windows[];

extern void WindowsManager(void);

#endif
