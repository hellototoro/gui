/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-05-22 18:12:45
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-08-18 17:32:28
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

#ifdef __cplusplus
extern "C" {
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
	void (*ScreenInit) (void);
    void (*ScreenLoad) (void);
}window;


extern ActiveScreen CurrentScreen;
extern ActiveScreen DefaultScreen;
extern window *Windows[];

extern void WindowsManager(void);

#ifdef __cplusplus
}
#endif

#endif
