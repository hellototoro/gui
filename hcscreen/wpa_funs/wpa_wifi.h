#ifndef WPA_WIFI_H
#define WPA_WIFI_H

/**
 * @file wpa_wifi.h
 * @author your name (you@domain.com)
 * @brief connect to WPA-PSK network via libwpa_client.h
 * @version 0.1
 * @date 2022-01-10
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <unistd.h>
#include <wpa_ctrl.h>
#include "define_callback_list.h"

#ifdef __cplusplus
#define EXTERNC extern "C" 
#else
#define EXTERNC 
#endif

#define WPA_EVENT_ID_STATUS     0x10

enum WifiEventType {
    WIFI_EVENT_OTHER = 0,
    WIFI_EVENT_SCAN_FINISHED,
    WIFI_EVENT_SCAN_RESULTS_READY,
    WIFI_EVENT_CONNECTED,
    WIFI_EVENT_DISCONNECTED,
    WIFI_EVENT_INVALID_PASSWD
};
typedef void (*WifiEventReceiver)(enum WifiEventType event_type, const char* raw_msg, size_t raw_msg_len, void* userdata);

DECLARE_CALLBACK_MAINTAIN_FUNCTIONS(wpa, wifi_event, WifiEventReceiver);
EXTERNC int wpa_wifi_event_stop(void *ctrl);
EXTERNC void *wpa_wifi_event_start(char *ifname);


extern int wpa_enable_reset_config_on_init_fail;
EXTERNC int wpa_ctrl_deinit(void *ctrl);
EXTERNC void *wpa_ctrl_init(char *ifname);
EXTERNC int wpa_reset_config();
EXTERNC int wpa_save_config(void *ctrl);
EXTERNC int wpa_reload_config(void *ctrl);


typedef struct _WifiAP {
    char             bssid[20];        /**< BSSID */
    char             ssid[144];         /**< SSID */  
    char             flag[128];         /**< Encrypted mode: [] */ 
    int signal_level_dBm;
    int frequency;
} WifiAP;

EXTERNC int wpa_wifi_flag_wep(WifiAP* data);
EXTERNC int wpa_wifi_flag_wpa(WifiAP* data);
EXTERNC int wpa_wifi_flag_wpa2(WifiAP* data);
EXTERNC int wpa_wifi_flag_wpa3(WifiAP* data);
EXTERNC void wpa_wifi_info_dump2stdout(WifiAP* data);
EXTERNC void wpa_wifi_infos_dump2stdout(WifiAP* data, int data_cnt);

EXTERNC int wpa_wifi_scan(void *ctrl);
EXTERNC int wpa_wifi_infos_lock();
EXTERNC int wpa_wifi_infos_get(WifiAP** data, int* data_cnt);
EXTERNC int wpa_wifi_infos_clear();
EXTERNC int wpa_wifi_infos_unlock();

EXTERNC int wpa_wifi_remove(void *ctrl, int network_id);
EXTERNC int wpa_wifi_add(void *ctrl, int *network_id);
// select a network (disable others)
EXTERNC int wpa_wifi_select(void *ctrl, int network_id);
EXTERNC int wpa_wifi_set_ssid(void *ctrl, int network_id, char* ssid);
EXTERNC int wpa_wifi_set_key_mgmt(void *ctrl, int network_id, char* key_mgmt);
EXTERNC int wpa_wifi_set_psk(void *ctrl, int network_id, char* psk);
EXTERNC int wpa_wifi_set_wep_key(void *ctrl, int network_id, char* key);
EXTERNC int wpa_wifi_enable(void *ctrl, int network_id);
EXTERNC int wpa_wifi_disable(void *ctrl, int network_id);
EXTERNC int wpa_wifi_set_scan_ssid(void *ctrl, int network_id, int enable); 
EXTERNC int wpa_wifi_autoreconnect(void *ctrl, int enable);


typedef struct _WiFiStatus {
	WifiAP ap;
	char key_mgmt[16];
	char wpa_state[16];
	int linkspeed;
} WiFiStatus;
EXTERNC void wpa_wifi_status_dump(WiFiStatus* status);
EXTERNC int wpa_wifi_get_status(void *ctrl, WiFiStatus* status);

#endif