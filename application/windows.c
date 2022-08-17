/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-05-22 18:12:58
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-08-17 08:57:08
 * @FilePath: /gui/application/windows.c
 * @Description: None
 * @other: None
 */
#include "windows.h"

extern window LanguageWindow;
extern window HomeWindow;
extern window MediaWindow;

ActiveScreen CurrentScreen;
ActiveScreen DefaultScreen;

window *CurrentWindow;

window *Windows[] = {
    &LanguageWindow,
    &HomeWindow,
    &MediaWindow
};

void WindowsManager(void)
{
    static window *LastWindow = NULL;
    CurrentWindow = Windows[CurrentScreen];
    if (LastWindow != CurrentWindow) {
        if(CurrentWindow) {
            CurrentWindow->ScreenInit(NULL, NULL);
            CurrentWindow->ScreenLoad();
            if(LastWindow)
                LastWindow->ScreenClose();
        }
        LastWindow = CurrentWindow;
    }
}