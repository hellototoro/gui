/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-05-19 00:48:40
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-07-21 14:12:25
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
#ifdef HCCHIP_GCC
#include "hcapi/lvgl/lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif
#include "application/windows.h"
#ifdef HCCHIP_GCC
#include "hcapi/com_api.h"
#include "hcapi/key.h"
#include "hcapi/wifi_api.h"
#endif
#include "application/ui/ui_com.h"

#ifdef HOST_GCC
extern int sdl_init_2(void);
#endif

#define HOR_RES         1280
#define VER_RES         720

pthread_mutex_t lvgl_task_mutex;

#ifdef HCCHIP_GCC
static char m_wifi_module_name[32];
#endif

static void exit_console(int signo);

int main(int argc, char *argv[])
{
    ActiveScreen DefaultScreen;

    #ifdef HCCHIP_GCC
    if (argc == 2){
        strncpy(m_wifi_module_name, argv[1], sizeof(m_wifi_module_name)-1);
        wifi_api_set_module(m_wifi_module_name);
        printf("please modprobe %s!\n", m_wifi_module_name);
    }
    #endif

    if (pthread_mutex_init(&lvgl_task_mutex, NULL) != 0) {
        perror("Mutex init failed");
        exit(EXIT_FAILURE);
    }

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
        WindowsManager();
        pthread_mutex_lock(&lvgl_task_mutex);
        lv_task_handler();
        usleep(5000);
        pthread_mutex_unlock(&lvgl_task_mutex);
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
