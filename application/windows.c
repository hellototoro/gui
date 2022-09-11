/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-05-22 18:12:58
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-09-11 22:42:04
 * @FilePath: /gui/application/windows.c
 * @Description: None
 * @other: None
 */
#include "windows.h"

extern window LanguageWindow;
extern window HomeWindow;
extern window MediaWindow;
extern window SyncWindow;

ActiveScreen CurrentScreen;
ActiveScreen DefaultScreen;

window *CurrentWindow;

window *Windows[] = {
    &LanguageWindow,
    &HomeWindow,
    &MediaWindow,
    &SyncWindow,
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
