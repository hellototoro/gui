/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-05-19 00:48:40
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-06-22 14:12:18
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
static void* lgvl_task(void* arg);

pthread_mutex_t* draw_mutex;

int main(void)
{
    int res;
    pthread_t thread_id = 0;
    pthread_attr_t attr;
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

    draw_mutex = (pthread_mutex_t* ) malloc(sizeof(pthread_mutex_t));
    res = pthread_mutex_init(draw_mutex, NULL);
    if (res != 0) {
        perror("Mutex init failed");
        exit(EXIT_FAILURE);
    }
    res = pthread_attr_init(&attr);
    if (res != 0) {
        perror("Attribute creation failed");
        exit(EXIT_FAILURE);
    }
    res = pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
    if (res != 0) {
        perror("Setting detached attribute failed");
        exit(EXIT_FAILURE);
    }
    res = pthread_create(&thread_id, &attr, lgvl_task, NULL);
    if (res != 0) {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    }

    DefaultScreen = HomeScreen;
    CurrentScreen = DefaultScreen;
    while(1) {
        CurrentWindow = Windows[CurrentScreen];
        if(CurrentWindow) {
            pthread_mutex_lock(draw_mutex);
            CurrentWindow->ScreenInit(NULL, NULL);
            CurrentWindow->ScreenLoad();
            if(LastWindow)
                LastWindow->ScreenClose();
            pthread_mutex_unlock(draw_mutex);
            CurrentWindow->ScreenWait();
            LastWindow = CurrentWindow;
        }
        else {
            usleep(1000);
        }
    }

    return 0;
}

void exit_console(int signo)
{
    printf("%s(), signo: %d, error: %s\n", __FUNCTION__, signo, strerror(errno));
    exit(0);
}

/* lvgl 线程 */
void* lgvl_task(void* arg)
{
    (void)arg;
    /*Handle LitlevGL tasks (tickless mode)*/
    while(1) {
        pthread_mutex_lock(draw_mutex);
        lv_timer_handler();
        pthread_mutex_unlock(draw_mutex);
        usleep(5000);
    }
    return NULL;
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
