/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-05-22 18:12:45
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-09-11 22:46:06
 * @FilePath: /gui/application/windows.h
 * @Description: None
 * @other: None
 */
#ifndef __APPLICATION_WINDOWS_H__
#define __APPLICATION_WINDOWS_H__

#include "lvgl/lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum  {
    LanguageScreen,
    HomeScreen,
    MediaScreen,
    SyncScreen,
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
