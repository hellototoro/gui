/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-05-22 18:12:58
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-06-30 14:35:45
 * @FilePath: /gui/application/windows.c
 * @Description: None
 * @other: None
 */
#include "windows.h"

extern window LanguageWindow;
extern window HomeWindow;
extern window MediaWindow;

ActiveScreen CurrentScreen;

window *CurrentWindow;
window *LastWindow;

window *Windows[] = {
    &LanguageWindow,
    &HomeWindow,
    &MediaWindow
};

void WindowsManager(void)
{
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