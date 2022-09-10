/**
 * @file cast_api.c
 * @author your name (you@domain.com)
 * @brief hichip cast api
 * @version 0.1
 * @date 2022-01-21
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <ffplayer.h>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <time.h>

#include "com_api.h"
#include "osd_com.h"
#include "cast_api.h"
#include "data_mgr.h"

#define CAST_SERVICE_NAME               "HCcast"
#define CAST_AIRCAST_SERVICE_NAME       "HCcast"
#define CAST_DLNA_FIRENDLY_NAME         "HCcast"
#define CAST_MIRACAST_NAME              "HCcast"

#define UUID_HEADER "HCcast"


#ifndef DLNA_SUPPORT
//implement fake functions
int hccast_dlna_service_uninit(void)
{
    return 0;
}

int hccast_dlna_service_start(void)
{
    return 0;
}

int hccast_dlna_service_stop(void)
{
    return 0;
}

#endif


#ifndef AIRCAST_SUPPORT
//implement fake functions
int hccast_air_ap_mirror_stat(void)
{
    return 0;
}
int hccast_air_ap_audio_stat(void)
{
    return 0;
}
int hccast_air_service_start(void)
{
    return 0;
}
int hccast_air_service_stop(void)
{
    return 0;
}
void hccast_air_mdnssd_start(void)
{}
void hccast_air_mdnssd_stop(void)
{}
void hccast_air_mediaplayer_2_aircast_event(int type, void *param)
{}
int hccast_air_ap_get_mirror_frame_num(void)
{
    return 0;
}
int hccast_air_service_is_start(void)
{
    return 0;
}

#endif

#ifndef MIRACAST_SUPPORT
//implement fake functions


int hccast_mira_service_start(void)
{
    return 0;
}

int hccast_mira_service_stop(void)
{
    return 0;
}
int hccast_mira_player_init(void)
{
    return 0;
}

int hccast_mira_get_stat(void)
{
    return 0;
}

int hccast_mira_service_uninit(void)
{
    return 0;
}

#endif


int cast_get_service_name(cast_type_t cast_type, char *service_name, int length)
{
    unsigned char mac_addr[6] = {0};
    char service_prefix[32] = CAST_SERVICE_NAME;

    snprintf(service_prefix, sizeof(service_prefix)-1, "%s", CAST_SERVICE_NAME);
    if (0 != api_get_mac_addr((char*)mac_addr))
        memset(mac_addr, 0xff, sizeof(mac_addr));

    if (CAST_TYPE_AIRCAST == cast_type)
        snprintf(service_prefix, sizeof(service_prefix)-1, "%s", CAST_AIRCAST_SERVICE_NAME);
    else if (CAST_TYPE_DLNA == cast_type)
        snprintf(service_prefix, sizeof(service_prefix)-1, "%s", CAST_DLNA_FIRENDLY_NAME);
    else if (CAST_TYPE_MIRACAST == cast_type)
        snprintf(service_prefix, sizeof(service_prefix)-1, "%s", CAST_MIRACAST_NAME);

    snprintf(service_name, length, "%s-%02x%02x%02x",
             service_prefix, mac_addr[3]&0xFF, mac_addr[4]&0xFF, mac_addr[5]&0xFF);

    return 0;
}

/*
static int cast_get_wifi_mac(unsigned char *mac)
{
    int ret = 0;
    int sock, if_count, i;
    struct ifconf ifc;
    struct ifreq ifr[10];

    if (!mac)
    {
        return 0;
    }

    memset(&ifc, 0, sizeof(struct ifconf));

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    ifc.ifc_len = 10 * sizeof(struct ifreq);
    ifc.ifc_buf = ifr;
    ioctl(sock, SIOCGIFCONF, &ifc);

    if_count = ifc.ifc_len / sizeof(struct ifreq);

    for (i = 0; i < if_count; i ++)
    {
        if (ioctl(sock, SIOCGIFHWADDR, &ifr[i]) == 0)
        {
            memcpy(mac, ifr[i].ifr_hwaddr.sa_data, 6);
            if (!strcmp(ifr[i].ifr_name, "wlan0"))
            {
                return 1;
            }
        }
    }

    return 0;
}
*/


#ifdef DLNA_SUPPORT
int hccast_dlna_callback_func(hccast_dlna_event_e event, void* in, void* out)
{
    _log(DEMO, INFO, "[%s] event: %d", __func__,event);
    char *str_tmp = NULL;

    switch (event)
    {
        case HCCAST_DLNA_GET_DEVICE_NAME:
        {
            printf("[%s]HCCAST_DLNA_GET_DEVICE_NAME\n",__func__);
            if (in)
            {
                str_tmp = data_mgr_get_device_name();
                if (str_tmp)
                {
                    sprintf((char *)in, "%s_dlna", str_tmp);
                    printf("[%s]HCCAST_DLNA_GET_DEVICE_NAME:%s\n",__func__, str_tmp);
                }
            }
            break;
        }

        default:
            break;

    }

    return 0;
}
#endif


#ifdef MIRACAST_SUPPORT
int hccast_mira_callback_func(hccast_mira_event_e event, void* in, void* out)
{
    _log(DEMO, INFO, "[%s] event: %d", __func__, event);
    control_msg_t ctl_msg = {0};
    char *str_tmp = NULL;

    switch (event)
    {
        case HCCAST_MIRA_GET_DEVICE_NAME:
        {
            if (in)
            {
                str_tmp = data_mgr_get_device_name();
                if (str_tmp)
                {
                    sprintf((char *)in, "%s_mira", str_tmp);
                    _log(DEMO, INFO, "HCCAST_MIRA_GET_DEVICE_NAME:%s\n", str_tmp);
                }
            }
            break;
        }
        case HCCAST_MIRA_SSID_DONE:
        {
            _log(DEMO, DEBUG, "[%s]HCCAST_MIRA_SSID_DONE\n",__func__);
            ctl_msg.msg_type = MSG_TYPE_CAST_MIRACAST_SSID_DONE;
            break;
        }
        case HCCAST_MIRA_GET_CUR_WIFI_INFO:
        {
            _log(DEMO, DEBUG, "[%s]HCCAST_MIRA_GET_CUR_WIFI_INFO\n",__func__);
            char cur_ssid[64] = {0};
            hccast_wifi_ap_info_t *cur_ap;
            hccast_wifi_mgr_get_connect_ssid(cur_ssid, sizeof(cur_ssid));
            cur_ap = data_mgr_get_wifi_info(cur_ssid);
            if(cur_ap)
                memcpy(in,cur_ap,sizeof(hccast_wifi_ap_info_t));
            break;
        }
        case HCCAST_MIRA_CONNECT:
        {
            //miracast connect start
            _log(DEMO, DEBUG, "[%s]HCCAST_MIRA_CONNECT\n",__func__);
			ctl_msg.msg_type = MSG_TYPE_CAST_MIRACAST_CONNECTING;
            break;
        }
        case HCCAST_MIRA_CONNECTED:
        {
            //miracast connect success
            _log(DEMO, DEBUG, "[%s]HCCAST_MIRA_CONNECTED\n",__func__);
			ctl_msg.msg_type = MSG_TYPE_CAST_MIRACAST_CONNECTED;
            break;
        }
        case HCCAST_MIRA_DISCONNECT:
        {
            //miracast disconnect
            _log(DEMO, DEBUG, "[%s]HCCAST_MIRA_DISCONNECT\n",__func__);
            break;
        }

        case HCCAST_MIRA_START_DISP:
        {
            //miracast start
            _log(DEMO, DEBUG, "[%s]HCCAST_MIRA_START_DISP\n",__func__);
            ctl_msg.msg_type = MSG_TYPE_CAST_MIRACAST_START;
            break;
        }
        case HCCAST_MIRA_STOP_DISP:
        {
            //miracast stop
            _log(DEMO, DEBUG, "[%s]HCCAST_MIRA_STOP_DISP\n",__func__);
            ctl_msg.msg_type = MSG_TYPE_CAST_MIRACAST_STOP;
            break;
        }

        default:
            break;

    }

    if (0 != ctl_msg.msg_type)
    {
        api_control_send_msg(&ctl_msg);
    }

    if (MSG_TYPE_CAST_MIRACAST_STOP == ctl_msg.msg_type)
    {
        //printf("[%s] wait cast root start tick: %d\n",__func__,(int)time(NULL));
        bool win_cast_root_wait_open(uint32_t timeout);
        win_cast_root_wait_open(20000);
        //printf("[%s] wait cast root end tick: %d\n",__func__,(int)time(NULL));
    }

    if (MSG_TYPE_CAST_MIRACAST_START == ctl_msg.msg_type)
    {
        //printf("[%s] wait cast play start tick: %d\n",__func__,(int)time(NULL));
        bool win_cast_play_wait_open(uint32_t timeout);
        win_cast_play_wait_open(20000);
        //printf("[%s] wait cast play end tick: %d\n",__func__,(int)time(NULL));
    }

    return 0;
}

#endif


#ifdef AIRCAST_SUPPORT
int hccast_air_callback_event(hccast_air_event_e event, void* in, void* out)
{
    control_msg_t ctl_msg = {0};
    app_data_t * app_data = data_mgr_app_get();

    switch (event)
    {
        case HCCAST_AIR_GET_SERVICE_NAME:
            printf("[%s]HCCAST_AIR_GET_SERVICE_NAME\n",__func__);
            sprintf((char *)in, "%s_itv", data_mgr_get_device_name());

            break;

        case HCCAST_AIR_GET_NETWORK_DEVICE:
            printf("[%s]HCCAST_AIR_GET_NETWORK_DEVICE\n",__func__);
            sprintf((char *)in, "%s", "wlan0");
            break;
        case HCCAST_AIR_GET_MIRROR_RESOLUTION:
            printf("[%s]HCCAST_AIR_GET_MIRROR_RESOLUTION\n",__func__);
            //*(int*)in = app_data->res;
            break;
        case HCCAST_AIR_GET_MIRROR_FRAME:
            printf("[%s]HCCAST_AIR_GET_MIRROR_FRAME\n",__func__);
            //*(int*)in = app_data->res;
            break;
        case HCCAST_AIR_GET_MIRROR_MODE:
            printf("[%s]HCCAST_AIR_GET_MIRROR_MODE\n",__func__);
            *(int*)in = app_data->aircast_mode;
            break;
        case HCCAST_AIR_MIRROR_START:
            printf("[%s]HCCAST_AIR_MIRROR_START\n",__func__);
            ctl_msg.msg_type = MSG_TYPE_CAST_AIRMIRROR_START;
            break;
        case HCCAST_AIR_MIRROR_STOP:
            printf("[%s]HCCAST_AIR_MIRROR_STOP\n",__func__);
            ctl_msg.msg_type = MSG_TYPE_CAST_AIRMIRROR_STOP;
            break;
        case HCCAST_AIR_AUDIO_START:
            printf("[%s]HCCAST_AIR_AUDIO_START\n",__func__);
            ctl_msg.msg_type = MSG_TYPE_CAST_AIRCAST_AUDIO_START;
            break;
        case HCCAST_AIR_AUDIO_STOP:
            ctl_msg.msg_type = MSG_TYPE_CAST_AIRCAST_AUDIO_STOP;
            printf("[%s]HCCAST_AIR_AUDIO_STOP\n",__func__);
            break;
        case HCCAST_AIR_INVALID_CERT:
            ctl_msg.msg_type = MSG_TYPE_AIR_INVALID_CERT;
            printf("[%s]HCCAST_AIR_INVALID_CERT\n",__func__);
            break;
        case HCCAST_AIR_CHECK_4K_MODE:
            if(data_mgr_de_tv_sys_get() < TV_LINE_4096X2160_30)
            {
                *(int*)in = 0;
                printf("[%s] NOT 4K MODE, tv_sys:%d\n",__func__,data_mgr_de_tv_sys_get());

            }
            else
            {
                *(int*)in = 1;
                printf("[%s] NOW IS 4K MODE, tv_sys:%d\n",__func__,data_mgr_de_tv_sys_get());
            }

            break;

        default:
            break;
    }

    if (0 != ctl_msg.msg_type)
        api_control_send_msg(&ctl_msg);

    if (MSG_TYPE_CAST_AIRMIRROR_STOP == ctl_msg.msg_type)
    {
        //While dlna preempt air mirror(air mirror->air play), air mirror stop and dlna start,
        //sometime it is still in cast play UI(not exit to win root UI),
        //the next dlna url play is starting, then the UI/logo may block the dlna playing.
        //So here exit callback function wait for win cast root UI opening
        printf("[%s] wait cast root start tick: %d\n",__func__,(int)time(NULL));
        bool win_cast_root_wait_open(uint32_t timeout);
        win_cast_root_wait_open(20000);
        printf("[%s] wait cast root end tick: %d\n",__func__,(int)time(NULL));
    }

    if (MSG_TYPE_CAST_AIRMIRROR_START == ctl_msg.msg_type)
    {
        printf("[%s] wait cast play start tick: %d\n",__func__,(int)time(NULL));
        bool win_cast_play_wait_open(uint32_t timeout);
        win_cast_play_wait_open(20000);
        printf("[%s] wait cast play end tick: %d\n",__func__,(int)time(NULL));
    }

    return 0;
}
#endif

void cast_restart_services()
{
    if(hccast_get_current_scene() != HCCAST_SCENE_NONE)
    {
        hccast_scene_switch(HCCAST_SCENE_NONE);
    }

    printf("[%s]  begin restart services.\n",__func__);

    hccast_dlna_service_stop();
    hccast_dlna_service_start();

    hccast_air_service_stop();
    hccast_air_service_start();

}


