/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-05-19 00:48:40
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
#include "application/ui/ui_com.h"
#include "application/windows.h"
#include "application/SystemInit.h"
#include "application/NetWorkApi.h"

extern int sdl_init_2(void);

pthread_mutex_t lvgl_task_mutex;

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

    sdl_init_2();
    group_init();

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

void exit_console(int signo)
{
    printf("%s(), signo: %d, error: %s\n", __FUNCTION__, signo, strerror(errno));
    exit(0);
}
