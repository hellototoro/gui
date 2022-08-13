#ifndef __WIFI_API_H__
#define __WIFI_API_H__

#include "./wpa_funs/wpa_wifi.h"
#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WIFI_MODULE_NAME    "8188fu"

#define WIFI_MAX_SSID_LEN	32
#define WIFI_MAX_PWD_LEN	136

#define ENCRYPT_NONE "NONE"
#define ENCRYPT_WEP "WEP"
#define ENCRYPT_WPA_TKIP "WPAPSK/TKIP"
#define ENCRYPT_WPA_AES "WPAPSK/AES"
#define ENCRYPT_WPA2_TKIP "WPA2PSK/TKIP"
#define ENCRYPT_WPA2_AES "WPA2PSK/AES"
#define ENCRYPT_WPA_Mixed_1 "WPAPSK/TKIPAES"
#define ENCRYPT_WPA_Mixed_2 "WPA2PSK/TKIPAES"
#define ENCRYPT_WPA_Mixed_3 "WPA1PSKWPA2PSK/TKIP"
#define ENCRYPT_WPA_Mixed_4 "WPA1PSKWPA2PSK/AES"
#define ENCRYPT_WPA_Mixed_5 "WPA1PSKWPA2PSK/TKIPAES"


typedef enum
{
	WIFI_ENCRYPT_MODE_NONE = 0,
	WIFI_ENCRYPT_MODE_OPEN_WEP,
	WIFI_ENCRYPT_MODE_SHARED_WEP,
	WIFI_ENCRYPT_MODE_WPAPSK_TKIP,
	WIFI_ENCRYPT_MODE_WPAPSK_AES,
	WIFI_ENCRYPT_MODE_WPA2PSK_TKIP,
	WIFI_ENCRYPT_MODE_WPA2PSK_AES,
} WIFI_ENCRYPT_MODE;

#define ENCRYPT_SIMPLE_MODE_WEP		(1 << 0)
#define ENCRYPT_SIMPLE_MODE_WPA 	(1 << 1)
#define ENCRYPT_SIMPLE_MODE_WPA2	(1 << 2)
#define ENCRYPT_SIMPLE_MODE_WPA3	(1 << 3)

typedef struct wifi_ap_info{
	struct list_head node;
	char 	ssid[WIFI_MAX_SSID_LEN];     // AP's SSID
	WIFI_ENCRYPT_MODE	encrypt_mode;   // Encrypt mode. frank:the old mode, not used in linux, must check again
	char 	key_index;                     // for WEP key index
	char	pwd[WIFI_MAX_PWD_LEN];      // Password
	char 	pwd_count;        		    // the count of characters for password
	int 	quality;	//rssi strength
	short   enctypt_mode_support; //frank add.

	int network_id;// the ID is created from wpa command "add_network"
	char 	ap_index; //the index of the wifi ap list
}wifi_ap_info_t;

int wifi_sys_ap_init_list();
void *wifi_wpa_open(char *ifname);
int wifi_wpa_close(void *wifi_handle);

int wifi_sys_ap_count_get();
int wifi_ap_scan(void *wifi_handle);

wifi_ap_info_t *wifi_choose_ap_from_scan(void *wifi_handle);
int wifi_ap_list_get(void *wifi_handle, wifi_ap_info_t **wifi_info, int *ap_list_cnt);
int wifi_ap_connect(void *wifi_handle, wifi_ap_info_t *wifi_info);
int wifi_ap_disconnect(void *wifi_handle, wifi_ap_info_t *wifi_info);
bool wifi_ap_is_connected(void *wifi_handle);
int wifi_disconnect_current_ap(void *wifi_handle);
int wifi_connected_ap_save(void *wifi_handle, wifi_ap_info_t *wifi_info);
wifi_ap_info_t *wifi_get_current_ap(void *wifi_handle);
int wifi_driver_module_init();
void *wifi_wpa_get_handle(char *ifname);

#ifdef __cplusplus
} /*extern "C"*/
#endif


#endif