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

#ifdef __cplusplus
extern "C" {
#endif

typedef struct wifi_ap_info{
	//struct list_head node;
	char 	ssid[32];     // AP's SSID
	//WIFI_ENCRYPT_MODE	encrypt_mode;   // Encrypt mode. frank:the old mode, not used in linux, must check again
	char 	key_index;                     // for WEP key index
	char	pwd[128];      // Password
	char 	pwd_count;        		    // the count of characters for password
	int 	quality;	//rssi strength
	short   enctypt_mode_support; //frank add.

	int network_id;// the ID is created from wpa command "add_network"
	char 	ap_index; //the index of the wifi ap list
}wifi_ap_info_t;

typedef void (*NetWorkTaskCallBack)(void);

extern int NetWorkInit(void);
extern void NetWorkDeInit(void);
extern int WiFi_GetAPCount(void);
extern wifi_ap_info_t* WiFi_GetAPList(void);
extern void WiFi_Connect(wifi_ap_info_t *wifi_info);
extern void WiFi_Scan(NetWorkTaskCallBack cb);
extern bool WiFi_IsConnected(void);

#ifdef __cplusplus
}
#endif

#endif