/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-05-19 00:48:40
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-08-19 17:18:18
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
#include "hcapi/com_api.h"
#include "hcapi/key.h"
#include "hcapi/wifi_api.h"
#endif
#include "application/ui/ui_com.h"
#include "application/windows.h"
#include "application/SystemInit.h"
#include "application/NetWorkApi.h"

#ifdef HOST_GCC
extern int sdl_init_2(void);
#endif

pthread_mutex_t lvgl_task_mutex;

#ifdef HCCHIP_GCC
extern int wifi_api_set_module(char *wifi_module);
#endif

#ifdef HCCHIP_GCC
static char m_wifi_module_name[32];
#endif

static void exit_console(int signo);

int main(int argc, char *argv[])
{
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
    api_lvgl_init(OSD_MAX_WIDTH, OSD_MAX_HEIGHT);
    key_init();
    //key_regist(NULL);
    NetWorkInit();
    #endif

    #ifdef HOST_GCC
    sdl_init_2();
    #endif
    //group_init();
    get_keypad_indev();

    SystemInit();
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
