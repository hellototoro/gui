/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-05-19 00:48:40
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-06-29 17:00:03
 * @FilePath: /gui/main.c
 * @Description: None
 * @other: None
 */
#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "lvgl/lvgl.h"
#include "application/windows.h"
#ifdef HCCHIP_GCC
#include "hcapi/com_api.h"
#include "hcapi/key/key.h"
#endif
#include "application/ui/ui_com.h"

#ifdef HOST_GCC
extern int sdl_init_2(void);
#endif

#define HOR_RES         1280
#define VER_RES         720

static void exit_console(int signo);

int main(void)
{
    ActiveScreen DefaultScreen;
    window *LastWindow = NULL;

    signal(SIGTERM, exit_console); //kill signal
    signal(SIGINT, exit_console); //Ctrl+C signal
    signal(SIGSEGV, exit_console);
    signal(SIGBUS, exit_console);

    #ifdef HCCHIP_GCC
    api_system_init();
    api_video_init();
    api_audio_init();
    //api_logo_show(BACK_LOGO);
    app_ffplay_init();

    //lvgl_init();
    api_lvgl_init(HOR_RES, VER_RES);
    key_init();
    //key_regist(NULL);
    #endif

    #ifdef HOST_GCC
    sdl_init_2();
    #endif
    get_keypad_indev();

    DefaultScreen = HomeScreen;
    CurrentScreen = DefaultScreen;
    while(1) {
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
        lv_task_handler();
        usleep(1000);
    }
    return 0;
}

void exit_console(int signo)
{
    printf("%s(), signo: %d, error: %s\n", __FUNCTION__, signo, strerror(errno));
    exit(0);
}

/*Set in lv_conf.h as `LV_TICK_CUSTOM_SYS_TIME_EXPR`*/
uint32_t custom_tick_get(void)
{
    static uint64_t start_ms = 0;
    if(start_ms == 0) {
        struct timeval tv_start;
        gettimeofday(&tv_start, NULL);
        start_ms = (tv_start.tv_sec * 1000000 + tv_start.tv_usec) / 1000;
    }

    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    uint64_t now_ms;
    now_ms = (tv_now.tv_sec * 1000000 + tv_now.tv_usec) / 1000;

    uint32_t time_ms = now_ms - start_ms;
    return time_ms;
}
