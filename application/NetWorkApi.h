/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-08-19 15:46:16
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-08-19 17:53:29
 * @FilePath: /gui/application/NetWorkApi.h
 * @Description: None
 * @other: None
 */
#ifndef NET_WORK_APT_H
#define NET_WORK_APT_H

#ifdef HCCHIP_GCC
#include "hcapi/wifi_api.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HCCHIP_GCC
extern int NetWorkInit(void);
extern void WiFi_Scan(void);
extern void WiFi_GetAPList(wifi_ap_info_t **wifi_list, int *ap_count);
extern void WiFi_Connect(wifi_ap_info_t *wifi_info);
#endif


#ifdef __cplusplus
}
#endif

#endif