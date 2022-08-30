/**
 * network_api.h
 */

#ifndef __NETWORK_API_H__
#define __NETWORK_API_H__

#include <hccast/hccast_wifi_mgr.h>

#ifdef __cplusplus
extern "C" {
#endif

int network_init(void);
int network_deinit(void);
int network_connect(void);
void network_wifi_module_set(int wifi_module);
int network_wifi_module_get(void);
int hostap_get_connect_count(void);
char *wifi_local_ip_get(void);
char *wifi_connected_phone_ip_get(void);

#define HOATAP_CHANNEL_24G  6
#define HOATAP_CHANNEL_5G   36

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif    

