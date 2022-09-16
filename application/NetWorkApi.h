/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-08-19 15:46:16
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-09-11 21:51:37
 * @FilePath: /gui/application/NetWorkApi.h
 * @Description: None
 * @other: None
 */
#ifndef NET_WORK_APT_H
#define NET_WORK_APT_H

#include <stdbool.h>
#ifdef HCCHIP_GCC
#include "hcapi/wifi_api.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*NetWorkTaskCallBack)(void);

#ifdef HCCHIP_GCC
extern int NetWorkInit(void);
extern void NetWorkDeInit(void);
extern int WiFi_GetAPCount(void);
extern wifi_ap_info_t* WiFi_GetAPList(void);
extern void WiFi_Connect(wifi_ap_info_t *wifi_info);
#endif
extern void WiFi_Scan(NetWorkTaskCallBack cb);
extern bool WiFi_IsConnected(void);


#ifdef __cplusplus
}
#endif

#endif