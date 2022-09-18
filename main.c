/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-05-19 00:48:40
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-09-17 22:46:51
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
#include "hcapi/data_mgr.h"
#include "hcapi/tv_sys.h"
#include "hcapi/hotplug_mgr.h"
#include "hcapi/network_api.h"
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
int hcscreen(void);
#endif

static void exit_console(int signo);

int main(int argc, char *argv[])
{
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
    app_ffplay_init();
    //api_logo_show(NULL);

    data_mgr_load();
    //tv_sys_app_start_set(1); 
    hotplug_init();
    network_init();
    api_lvgl_init(OSD_MAX_WIDTH, OSD_MAX_HEIGHT);
    key_init();
    network_connect();
    #endif

    #ifdef HOST_GCC
    sdl_init_2();
    #endif
    group_init();
    get_keypad_indev();

    SystemInit();
    while(1) {
        WindowsManager();
        pthread_mutex_lock(&lvgl_task_mutex);
        lv_task_handler();
        pthread_mutex_unlock(&lvgl_task_mutex);
        ProcessSysMsg();
        usleep(5000);
    }
    return 0;
}

#ifdef USB_PLUG_TEST
int status = 1;
void set_plug_status(void)
{
    status = status == 0 ? 1 : 0;
}

int hotplug_usb_plugout(void)
{
    return status;
}

static void plug_status_event_handler(lv_event_t* event)
{
    set_plug_status();
    lv_msg_send(MSG_HOTPLUG, &status);
    if(status == 1)
        lv_msg_send(MSG_HOTPLUG_OUT, NULL);
    else
        lv_msg_send(MSG_HOTPLUG_IN, NULL);
}

void USB_PlugTest(lv_obj_t* parent)
{
    lv_obj_t* plug_test = lv_btn_create(parent);
    lv_obj_set_size(plug_test, 50, 50);
    lv_obj_set_pos(plug_test, 0, 0);
    lv_obj_set_align(plug_test, LV_ALIGN_TOP_LEFT);
    lv_obj_add_event_cb(plug_test, plug_status_event_handler, LV_EVENT_CLICKED, NULL);
}
#endif

void exit_console(int signo)
{
    printf("%s(), signo: %d, error: %s\n", __FUNCTION__, signo, strerror(errno));
    #ifdef HCCHIP_GCC
    api_gpio_deinit();
    #endif
    exit(0);
}
