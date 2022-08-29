/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-08-28 22:21:36
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-08-29 09:10:59
 * @FilePath: /gui/application/DetectHotPlug.cpp
 * @Description: None
 * @other: None
 */
#include "DetectHotPlug.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/netlink.h>
#include <iostream>

#define UEVENT_BUFFER_SIZE  2048

int DetectHotPlugInit(void)
{
    struct sockaddr_nl client;
    const int BuffSize = 1024;
    int HotPlugSock = socket(AF_NETLINK, SOCK_RAW, NETLINK_KOBJECT_UEVENT);
    if (HotPlugSock == -1) {
        printf("socket open error\n");
        return -1;
    }
    memset(&client, 0, sizeof(client));
    client.nl_family = AF_NETLINK;
    client.nl_pid = getpid();
    client.nl_groups = 1;
    setsockopt(HotPlugSock, SOL_SOCKET, SO_RCVBUF, &BuffSize, sizeof(BuffSize));
    if (bind(HotPlugSock, (struct sockaddr*)&client, sizeof(client)) < 0) {
        printf("bind failed\n");
        close(HotPlugSock);
        return -1;
    }
    return HotPlugSock;
}

static void* DetectHotPlugTask(void* arg)
{
    int HotPlugSock = DetectHotPlugInit();
    while (1) {
        char buff[UEVENT_BUFFER_SIZE] = {0};
        if (recv(HotPlugSock, &buff, sizeof(buff), 0) > 0 )
            std::cout << "usb:" << buff << std::endl;
    }
    close(HotPlugSock);
    pthread_exit(NULL);
}

static void* DetectHotPlugTask2(void* arg)
{
    int ret, rcvlen;
    fd_set fds;
    struct timeval tv;
    int HotPlugSock = DetectHotPlugInit();
    while (1) {
        char buff[UEVENT_BUFFER_SIZE] = {0};
        FD_ZERO(&fds);
        FD_SET(HotPlugSock, &fds);
        tv.tv_sec = 0;
        tv.tv_usec = 100 * 1000;
        ret = select(HotPlugSock + 1, &fds, NULL, NULL, &tv);
        if (ret < 0) continue;
        rcvlen = recv(HotPlugSock, &buff, sizeof(buff), 0); 
        if (rcvlen > 0)
            std::cout << "usb:" << buff << std::endl;
    }
    close(HotPlugSock);
    pthread_exit(NULL);
}

void CreateDetectTask(void)
{
    int res;
    pthread_t thread_id = 0;
    pthread_attr_t attr;
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
    res = pthread_create(&thread_id, &attr, DetectHotPlugTask, nullptr);
    if (res != 0) {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    }
}
