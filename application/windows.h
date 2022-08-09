/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-05-22 18:12:45
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-07-21 21:10:46
 * @FilePath: /gui/application/windows.h
 * @Description: None
 * @other: None
 */
#ifndef __APPLICATION_WINDOWS_H__
#define __APPLICATION_WINDOWS_H__

#ifdef HCCHIP_GCC
#include "hcapi/lvgl/lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

typedef enum  {
    LanguageScreen,
    HomeScreen,
    MediaScreen,
    HdmiScreen,
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
