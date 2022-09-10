/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-05-22 18:12:58
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-09-10 22:38:56
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
            CurrentWindow->ScreenInit();
            CurrentWindow->ScreenLoad();
        }
        LastWindow = CurrentWindow;
    }
}
