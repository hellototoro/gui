/*
network_api.c: use for network, include wifi, etc

 */

#include <math.h>
#include <stdio.h> //printf()
#include <stdlib.h>
#include <string.h> //memcpy()
#include <unistd.h> //usleep()
#include <pthread.h>
#include <arpa/inet.h>

#include <hccast/hccast_dhcpd.h>
#include <hccast/hccast_av.h>
#include <hccast/hccast_wifi_mgr.h>
#include <hccast/hccast_httpd.h>

#include <hccast/hccast_media.h>
#include <hccast/hccast_net.h>

#include <hccast/hccast_net.h>
#include "network_api.h"
#include <hcuapi/dis.h>
#include "cast_api.h"

#define UUID_HEADER "HCcast"

#include "com_api.h"
#include "data_mgr.h"
#include "tv_sys.h"


#define MAX_IP_STR_LEN 32
typedef struct
{
    char local_ip[MAX_IP_STR_LEN];
    char connected_phone_ip[MAX_IP_STR_LEN];
} wifi_config_t;

static wifi_config_t m_wifi_config = {0};

static int m_probed_wifi_module = 0;
static int hostap_connect_count = 0;
static int factary_init = 0;
static int hostap_discover_ok = 0;


static int wifi_mgr_callback_func(hccast_wifi_event_e event, void* in, void* out);
static int httpd_callback_func(hccast_httpd_event_e event, void* in, void* out);
static void media_callback_func(hccast_media_event_e msg_type, void* param);

static void network_probed_wifi(void);

static void hccast_start_services(void)
{
    if (hccast_get_current_scene() != HCCAST_SCENE_NONE)
    {
        hccast_scene_switch(HCCAST_SCENE_NONE);
    }

    printf("[%s]  begin start services.\n", __func__);

    hccast_dlna_service_start();
    hccast_air_service_start();
    hccast_mira_service_start();
}

static void hccast_stop_services(void)
{
    printf("[%s]  begin stop services.\n", __func__);

    hccast_dlna_service_stop();
    hccast_air_service_stop();
    hccast_mira_service_stop();
}


static void hccast_ap_dlna_aircast_start(void)
{
    hccast_dlna_service_start();
    hccast_air_service_start();
}

static void hccast_ap_dlna_aircast_stop(void)
{
    hccast_dlna_service_stop();
    hccast_air_service_stop();
}


static int wifi_mgr_callback_func(hccast_wifi_event_e event, void* in, void* out)
{
    _log(DEMO, INFO, "[%s] event: %d", __func__,event);
    control_msg_t ctl_msg = {0};

    switch (event)
    {
        case HCCAST_WIFI_SCAN:
        {
            //WiFi start scan...
            ctl_msg.msg_type = MSG_TYPE_NETWORK_WIFI_SCANNING;
            api_control_send_msg(&ctl_msg);
            break;
        }
        case HCCAST_WIFI_SCAN_RESULT:
        {
            hccast_wifi_scan_result_t *res = (hccast_wifi_scan_result_t*)out;

            _log(DEMO, INFO, "AP NUM: %d\n***********", res->ap_num);

            for (int i = 0; i < res->ap_num; i++)
            {
                _log(DEMO, INFO, "ssid: %s, quality: %d", res->apinfo[i].ssid, res->apinfo[i].quality);
            }

            _log(DEMO, INFO, "\n***********");
            ctl_msg.msg_type = MSG_TYPE_NETWORK_WIFI_SCAN_DONE;
            api_control_send_msg(&ctl_msg);

            break;
        }
        case HCCAST_WIFI_CONNECT:
        {
            break;
        }
        case HCCAST_WIFI_CONNECT_SSID:
        {
            _log(DEMO, INFO, "SSID: %s", (char*)out);
            break;
        }
        case HCCAST_WIFI_CONNECT_RESULT: //station
        {
            hccast_udhcp_result_t* result = (hccast_udhcp_result_t*) out;
            if (result)
            {
                _log(DEMO, INFO, "state: %d", result->stat);
                if (result->stat)
                {
                    _log(DEMO, INFO, "ifname: %s", result->ifname);
                    _log(DEMO, INFO, "ip addr: %s", result->ip);
                    _log(DEMO, INFO, "mask addr: %s", result->mask);
                    _log(DEMO, INFO, "gw addr: %s", result->gw);
                    _log(DEMO, INFO, "dns: %s", result->dns);

                    hccast_net_ifconfig(result->ifname, result->ip, result->mask, result->gw);
                    hccast_net_set_dns(result->ifname, result->dns);
                    hccast_start_services();

                    strncpy(m_wifi_config.local_ip, result->ip, MAX_IP_STR_LEN);

                    ctl_msg.msg_type = MSG_TYPE_NETWORK_WIFI_CONNECTED;
                    api_control_send_msg(&ctl_msg);
                }
                else
                {
                    hccast_wifi_mgr_disconnect_no_message();
                    usleep(50*1000);
                    ctl_msg.msg_type = MSG_TYPE_NETWORK_WIFI_DISCONNECTED;
                    api_control_send_msg(&ctl_msg);
                    hccast_wifi_mgr_hostap_start();
                    hccast_mira_service_start();
                }
            }

            break;
        }

        case HCCAST_WIFI_DISCONNECT:
        {
            if (hccast_wifi_mgr_p2p_get_connect_stat() == 0)
            {
                printf("%s Wifi has been disconnected, beging change to host ap mode\n",__func__);
                hccast_media_stop();
                hccast_ap_dlna_aircast_stop();
                hccast_wifi_mgr_hostap_start();
            }
            ctl_msg.msg_type = MSG_TYPE_NETWORK_WIFI_DISCONNECTED;
            api_control_send_msg(&ctl_msg);
            break;
        }
        case HCCAST_WIFI_HOSTAP_OFFER:
        {
            if (out)
            {
                struct in_addr tmp_addr =
                {
                    .s_addr = (unsigned int)out
                };

                hostap_discover_ok = 1;

                _log(WIFI, INFO, "addr: %s", inet_ntoa(tmp_addr));

                strncpy(m_wifi_config.connected_phone_ip, inet_ntoa(tmp_addr), MAX_IP_STR_LEN);


            }
            hccast_ap_dlna_aircast_start();
        }


        default:
            break;
    }

    return 0;

}


char *wifi_local_ip_get(void)
{
    return m_wifi_config.local_ip;
}

char *wifi_connected_phone_ip_get(void)
{
    return m_wifi_config.connected_phone_ip;
}

static void *wifi_disconnect_thread(void *args)
{
    printf("----------------------------wifi_disconnect_thread is running.-----------------------------\n");
    hccast_wifi_mgr_disconnect();
    return NULL;
}

static void *wifi_connect_thread(void *args)
{
    hccast_wifi_ap_info_t *ap_wifi = (hccast_wifi_ap_info_t*)args;
    int index;
    printf("----------------------------wifi_connect_thread is running.-----------------------------\n");

    hccast_stop_services();
    hccast_wifi_mgr_hostap_stop();

    hccast_wifi_mgr_connect(ap_wifi);
    if (hccast_wifi_mgr_get_connect_status())
    {
        hccast_wifi_mgr_udhcpc_start();

        index = data_mgr_check_ap_saved(ap_wifi);
        printf("ssid index: %d\n",index);
        if(index >= 0)//set the index ap to first.
        {
            data_mgr_wifi_ap_delete(index);
            data_mgr_wifi_ap_save(ap_wifi);
        }
        else
        {
            data_mgr_wifi_ap_save(ap_wifi);
        }

        data_mgr_save();
        sleep(1);
    }
    else
    {
        hccast_wifi_mgr_disconnect_no_message();
        usleep(50*1000);
        control_msg_t ctl_msg = {0};
        ctl_msg.msg_type = MSG_TYPE_NETWORK_WIFI_DISCONNECTED;
        api_control_send_msg(&ctl_msg);
        hccast_wifi_mgr_hostap_start();
        hccast_mira_service_start();
    }

    free(ap_wifi);
    return NULL;
}

static void *wifi_switch_mode_thread(void* arg)
{
    hccast_wifi_freq_mode_e mode = (hccast_wifi_freq_mode_e)arg;
    hccast_wifi_mgr_hostap_switch_mode(mode);

    pthread_detach(pthread_self());

    return NULL;
}

static int hccast_itoa(char * str, unsigned int val)
{
    char *p;
    char *first_dig;
    char temp;
    unsigned t_val;
    int len = 0;
    p = str;
    first_dig = p;

    do
    {
        t_val = (unsigned)(val % 0x0a);
        val   /= 0x0a;
        *p++ = (char)(t_val + '0');
        len++;
    }
    while (val > 0);
    *p-- = '\0';

    do
    {
        temp = *p;
        *p   = *first_dig;
        *first_dig = temp;
        --p;
        ++first_dig;
    }
    while (first_dig < p);
    return len;
}


static int httpd_callback_func(hccast_httpd_event_e event, void* in, void* out)
{
    int ret = 0;
    char mac[6];
    int cur_scene = 0;
    int last_resolution = 0;
    hccast_wifi_ap_info_t *save_ap = NULL;
    hccast_wifi_ap_info_t *con_ap = NULL;
    hccast_wifi_ap_info_t *check_ap = NULL;
    hccast_wifi_ap_info_t *del_ap = NULL;
    int index;
    app_data_t *app_data = data_mgr_app_get();
    sys_data_t* sys_data = data_mgr_sys_get();
    int ap_tv_sys;
    hccast_wifi_ap_info_t *ap_wifi = NULL;
    unsigned int version;
    control_msg_t msg = {0};

    switch (event)
    {
        case HCCAST_HTTPD_GET_DEV_PRODUCT_ID:
            printf("product_id: %s\n",sys_data->product_id);
            strcpy((char*)in, sys_data->product_id);
            break;
        case HCCAST_HTTPD_GET_DEV_VERSION:
            version = sys_data->firmware_version;
            hccast_itoa((char*)in,version);
            printf("version: %s\n",(char*)in);
            break;
        case HCCAST_HTTPD_GET_MIRROR_MODE:
            ret = app_data->mirror_mode;
            break;
        case HCCAST_HTTPD_SET_MIRROR_MODE:
            app_data->mirror_mode = (int)in;
            data_mgr_save();
            break;
        case HCCAST_HTTPD_GET_AIRCAST_MODE:
            ret = app_data->aircast_mode;
            break;
        case HCCAST_HTTPD_SET_AIRCAST_MODE:
            app_data->aircast_mode = (int)in;
            data_mgr_save();
            if(hccast_get_current_scene() == HCCAST_SCENE_NONE)
            {
                hccast_air_service_stop();
                hccast_air_service_start();
            }
            break;
        case HCCAST_HTTPD_GET_MIRROR_FRAME:
            ret = app_data->mirror_frame;
            break;
        case HCCAST_HTTPD_SET_MIRROR_FRAME:
            app_data->mirror_frame = (int)in;
            data_mgr_save();
            break;
        case HCCAST_HTTPD_GET_BROWSER_LANGUAGE:
            ret = app_data->browserlang;
            break;
        case HCCAST_HTTPD_SET_BROWSER_LANGUAGE:
            app_data->browserlang = (int)in;
            data_mgr_save();
            break;
        case HCCAST_HTTPD_GET_SYS_RESOLUTION:
            ret = data_mgr_app_tv_sys_get();
            break;
        case HCCAST_HTTPD_SET_SYS_RESOLUTION:
            ap_tv_sys = (int)in;
            if (ap_tv_sys != APP_TV_SYS_AUTO &&
                data_mgr_app_tv_sys_get() == ap_tv_sys)
            {
                printf("%s(), same tvsys:%d, not change TV sys\n",
                       __func__, ap_tv_sys);
                break;
            }

            last_resolution = data_mgr_app_tv_sys_get();

            ret = tv_sys_app_set(ap_tv_sys);

            if (API_SUCCESS == ret)
            {
                printf("%s(), line:%d. save app tv sys: %d!\n",
                       __func__, __LINE__, ap_tv_sys);
                data_mgr_app_tv_sys_set(ap_tv_sys);

                if(((last_resolution == APP_TV_SYS_4K)&&(ap_tv_sys != APP_TV_SYS_4K)) \
                   || ((last_resolution != APP_TV_SYS_4K)&&(ap_tv_sys == APP_TV_SYS_4K)) \
                   || ((last_resolution != APP_TV_SYS_AUTO)&&(ap_tv_sys == APP_TV_SYS_AUTO)) \
                   || ((last_resolution == APP_TV_SYS_AUTO)&&(ap_tv_sys != APP_TV_SYS_AUTO)) \
                  )
                {
                    cur_scene = hccast_get_current_scene();
                    if((cur_scene != HCCAST_SCENE_AIRCAST_PLAY) && (cur_scene != HCCAST_SCENE_AIRCAST_MIRROR))
                    {
                        if(hccast_air_service_is_start())
                        {
                            hccast_air_service_stop();
                            hccast_air_service_start();
                        }
                    }
                }

                data_mgr_save();
            }
            break;
        case HCCAST_HTTPD_GET_DEVICE_MAC:
            api_get_mac_addr(mac);
            memcpy(in, mac,sizeof(mac));
            break;
        case HCCAST_HTTPD_GET_DEVICE_NAME:
            strcpy((char*)in, app_data->cast_dev_name);
            break;
        case HCCAST_HTTPD_SET_DEVICE_NAME:
            strncpy(app_data->cast_dev_name, (char*)in,SERVICE_NAME_MAX_LEN);
            data_mgr_cast_dev_name_changed_set(1);
            data_mgr_save();
            msg.msg_type = MSG_TYPE_NETWORK_DEV_NAME_SET;
            api_control_send_msg(&msg);
            break;
        case HCCAST_HTTPD_GET_DEVICE_PSK:
            strcpy((char*)in, app_data->cast_dev_psk);
            break;
        case HCCAST_HTTPD_SET_DEVICE_PSK:
            strncpy(app_data->cast_dev_psk, (char*)in,DEVICE_PSK_MAX_LEN);
            data_mgr_save();
            break;
        case HCCAST_HTTPD_SET_SYS_RESTART:
            printf("HCCAST_HTTPD_SET_SYS_RESTART\n");
            api_system_reboot();
            break;
        case HCCAST_HTTPD_SET_SYS_RESET:
            printf("HCCAST_HTTPD_SET_SYS_RESET\n");
            data_mgr_factory_reset();
            factary_init = 1;
            api_system_reboot();
            break;
        case HCCAST_HTTPD_WIFI_AP_DISCONNECT:
        {
            pthread_t tid;
            pthread_create(&tid, NULL,wifi_disconnect_thread, NULL);
            break;
        }
        case HCCAST_HTTPD_CHECK_AP_SAVE:
        {

            check_ap = (hccast_wifi_ap_info_t*)in;
            save_ap = (hccast_wifi_ap_info_t *)out;
            index =data_mgr_check_ap_saved(check_ap);

            if(index >= 0)
            {
                if(save_ap)
                {
                    strcpy(save_ap->ssid, app_data->wifi_ap[index].ssid);
                    strcpy(save_ap->pwd, app_data->wifi_ap[index].pwd);
                }
                ret = 0;
            }
            else
            {
                ret = -1;
            }

            break;
        }
        case HCCAST_HTTPD_WIFI_AP_CONNECT:
        {
            con_ap = (hccast_wifi_ap_info_t *)in;
            if(con_ap)
            {
                ap_wifi = (hccast_wifi_ap_info_t *)malloc(sizeof(hccast_wifi_ap_info_t));
                if(ap_wifi)
                {
                    pthread_t tid;
                    memcpy(ap_wifi,con_ap,sizeof(hccast_wifi_ap_info_t));
                    pthread_create(&tid, NULL,wifi_connect_thread,(void*)ap_wifi);
                }
                else
                {
                    return -1;
                }
            }
            break;
        }
        case HCCAST_HTTPD_DELETE_WIFI_INFO:
            del_ap = (hccast_wifi_ap_info_t *)in;
            if(del_ap)
            {
                index =data_mgr_check_ap_saved(del_ap);
                printf("del_ap->ssid:%s, index: %d\n",del_ap->ssid,index);
                if(index >= 0)
                {
                    data_mgr_wifi_ap_delete(index);
                    data_mgr_save();
                }
            }
            break;
        case HCCAST_HTTPD_GET_CUR_WIFI_INFO:
        {
            char cur_ssid[64] = {0};
            hccast_wifi_ap_info_t *cur_ap;
            hccast_wifi_mgr_get_connect_ssid(cur_ssid, sizeof(cur_ssid));
            cur_ap = data_mgr_get_wifi_info(cur_ssid);
            if(cur_ap)
                memcpy(in,cur_ap,sizeof(hccast_wifi_ap_info_t));
            break;
        }
        case HCCAST_HTTPD_SHOW_PROGRESS:
        {
            printf("progress: %d\n",(int)in);
            msg.msg_type = MSG_TYPE_UPG_DOWNLOAD_PROGRESS;
            msg.msg_code = (uint32_t)in;
            api_control_send_msg(&msg);
            break;
        }
        case HCCAST_HTTPD_GET_UPLOAD_DATA_START:
        {
            msg.msg_type = MSG_TYPE_NET_UPGRADE;
            api_control_send_msg(&msg);
            api_sleep_ms(500);

            break;
        }
        case HCCAST_HTTPD_GET_UPGRADE_FILE_BEGING:
        {
            msg.msg_type = MSG_TYPE_NET_UPGRADE;
            api_control_send_msg(&msg);
            api_sleep_ms(500);
            break;
        }
        case HCCAST_HTTPD_GET_UPLOAD_DATA_FAILED:
        {
            break;
        }
        case HCCAST_HTTPD_MSG_UPGRADE_BAD_RES:
        {
            //http server return 4xx.
            break;
        }
        case HCCAST_HTTPD_MSG_USER_UPGRADE_ABORT:
        {
            break;
        }
        case HCCAST_HTTPD_MSG_UPGRADE_FILE_SUC:
        {
            //can block to upgrade.
            hccast_web_upgrade_info_st* info = NULL;
            info = (hccast_web_upgrade_info_st*)in;
            if(info)
            {
                printf("upgrade len: %d\n",info->length);
                printf("upgrade buf: %p\n",info->buf);
                //free(info->buf);
            }
            sys_upg_flash_burn(info->buf, info->length);

            break;
        }
        case HCCAST_HTTPD_MSG_UPLOAD_DATA_SUC:
        {
            hccast_web_upload_info_st* info = NULL;
            info = (hccast_web_upload_info_st*)in;
            if(info)
            {
                printf("upload len: %d\n",info->length);
                printf("upload buf: %p\n",info->buf);
                //free(info->buf);
            }
            sys_upg_flash_burn(info->buf, info->length);
            break;
        }
        case HCCAST_HTTPD_GET_WIFI_FREQ_MODE_EN:
            if(HCCAST_WIFI_FREQ_MODE_5G == hccast_wifi_mgr_freq_support_mode() \
               /*HCCAST_WIFI_FREQ_MODE_60G == hccast_wifi_mgr_freq_support_mode()*/)
            {
                ret = 1;
            }
            else
            {
                ret = 0;
            }
            break;
        case HCCAST_HTTPD_GET_WIFI_FREQ_MODE:
            ret = app_data->wifi_mode;
            break;
        case HCCAST_HTTPD_SET_WIFI_FREQ_MODE:
        {
            app_data->wifi_mode = (int)in;
            if (1 == app_data->wifi_mode)
            {
                app_data->wifi_ch = 6;
            }
            else if (2 == app_data->wifi_mode)
            {
                app_data->wifi_ch = 36;
            }
            else
            {
                app_data->wifi_ch = 6;
            }

            data_mgr_save();

            pthread_t tid;
            pthread_create(&tid, NULL,wifi_switch_mode_thread, (void*)in);
            break;
        }
        case HCCAST_HTTPD_GET_CUR_SCENE_PLAY:
            if((hccast_get_current_scene() != HCCAST_SCENE_NONE) || (hccast_air_ap_audio_stat()))
            {
                ret = 1;
            }
            else
            {
                ret = 0;
            }
            break;
        case HCCAST_HTTPD_STOP_MIRA_SERVICE:
            hccast_mira_service_stop();
            sleep(1);
            break;
        case HCCAST_HTTPD_START_MIRA_SERVICE:
            hccast_mira_service_start();
            break;
        default :
            break;
    }

    return ret;
}



static void media_callback_func(hccast_media_event_e msg_type, void* param)
{
    control_msg_t ctl_msg = {0};
    ctl_msg.msg_code = (uint32_t)param;

    switch (msg_type)
    {
        case HCCAST_MEDIA_EVENT_PARSE_END:
            printf("[%s] %d   HCCAST_MEDIA_EVENT_PARSE_END\n", __func__, __LINE__);
            break;
        case HCCAST_MEDIA_EVENT_PLAYING:
            ctl_msg.msg_type = MSG_TYPE_CAST_DLNA_PLAY;
            printf("[%s] %d   HCCAST_MEDIA_EVENT_PLAYING\n", __func__, __LINE__);
            break;
        case HCCAST_MEDIA_EVENT_PAUSE:
            ctl_msg.msg_type = MSG_TYPE_CAST_DLNA_PAUSE;
            printf("[%s] %d   HCCAST_MEDIA_EVENT_PAUSE\n", __func__, __LINE__);
            break;
        case HCCAST_MEDIA_EVENT_BUFFERING:
            ctl_msg.msg_type = MSG_TYPE_MEDIA_BUFFERING;
            ctl_msg.msg_code = (uint32_t)param;
            //printf("[%s] %d   HCCAST_MEDIA_EVENT_BUFFERING, %d%%\n", __func__, __LINE__, (int)param);
            break;
        case HCCAST_MEDIA_EVENT_PLAYBACK_END:
            printf("[%s] %d   HCCAST_MEDIA_EVENT_PLAYBACK_END\n", __func__, __LINE__);
            ctl_msg.msg_type = MSG_TYPE_CAST_DLNA_STOP;
            break;
        case HCCAST_MEDIA_EVENT_VIDEO_DECODER_ERROR:
            printf("[%s] %d   HCCAST_MEDIA_EVENT_VIDEO_DECODER_ERROR\n", __func__, __LINE__);
            ctl_msg.msg_type = MSG_TYPE_MEDIA_VIDEO_DECODER_ERROR;
            break;
        case HCCAST_MEDIA_EVENT_AUDIO_DECODER_ERROR:
            printf("[%s] %d   HCCAST_MEDIA_EVENT_AUDIO_DECODER_ERROR\n", __func__, __LINE__);
            ctl_msg.msg_type = MSG_TYPE_MEDIA_AUDIO_DECODER_ERROR;
            break;
        case HCCAST_MEDIA_EVENT_VIDEO_NOT_SUPPORT:
            printf("[%s] %d   HCCAST_MEDIA_EVENT_VIDEO_NOT_SUPPORT\n", __func__, __LINE__);
            ctl_msg.msg_type = MSG_TYPE_MEDIA_VIDEO_NOT_SUPPORT;
            ctl_msg.msg_code = (uint32_t)param;
            break;
        case HCCAST_MEDIA_EVENT_AUDIO_NOT_SUPPORT:
            printf("[%s] %d   HCCAST_MEDIA_EVENT_AUDIO_NOT_SUPPORT\n", __func__, __LINE__);
            ctl_msg.msg_type = MSG_TYPE_MEDIA_AUDIO_NOT_SUPPORT;
            ctl_msg.msg_code = (uint32_t)param;
            break;
        case HCCAST_MEDIA_EVENT_NOT_SUPPORT:
            printf("[%s] %d   HCCAST_MEDIA_EVENT_NOT_SUPPORT\n", __func__, __LINE__);
            ctl_msg.msg_type = MSG_TYPE_MEDIA_NOT_SUPPORT;
            break;
        case HCCAST_MEDIA_EVENT_URL_FROM_DLNA:
            printf("[%s] %d   HCCAST_MEDIA_EVENT_URL_FROM_DLNA, media_type: %d\n", __func__, __LINE__,(hccast_media_type_e)param);

            ctl_msg.msg_type = MSG_TYPE_CAST_DLNA_START;
            ctl_msg.msg_code = (uint32_t)param;
            break;
        case HCCAST_MEDIA_EVENT_URL_FROM_AIRCAST:
            printf("[%s] %d   HCCAST_MEDIA_EVENT_URL_FROM_AIRCAST, media_type: %d\n", __func__, __LINE__,(hccast_media_type_e)param);
            ctl_msg.msg_type = MSG_TYPE_CAST_AIRCAST_START;
            ctl_msg.msg_code = (uint32_t)param;
            break;
        case HCCAST_MEDIA_EVENT_URL_SEEK:
            printf("[%s] %d   HCCAST_MEDIA_EVENT_URL_SEEK  position: %ds\n", __func__, __LINE__, (int)param / 1000);
            ctl_msg.msg_type = MSG_TYPE_CAST_DLNA_SEEK;
            break;
        case HCCAST_MEDIA_EVENT_SET_VOLUME:
            ctl_msg.msg_type = MSG_TYPE_CAST_DLNA_VOL_SET;
            ctl_msg.msg_code = (uint32_t)param;
            printf("[%s] %d   HCCAST_MEDIA_EVENT_SET_VOLUME  volume: %d\n", __func__, __LINE__, (int)param);
            break;
        default:
            break;
    }

    if (0 != ctl_msg.msg_type)
        api_control_send_msg(&ctl_msg);

    if (MSG_TYPE_CAST_DLNA_STOP == ctl_msg.msg_type)
    {
        //While air mirror preempt dlna(air play->air mirror), dlna stop and air mirror start,
        //sometime it is still in dlna play UI(not exit to win root UI),
        //the next air mirror play is starting, then the UI/logo may block the air mirror playing.
        //So here exit callback function wait for win cast root UI opening
        // printf("[%s] wait cast root start tick: %d\n",__func__,(int)time(NULL));
        //bool win_cast_root_wait_open(uint32_t timeout);
        //win_cast_root_wait_open(20000);
        // printf("[%s] wait cast root end tick: %d\n",__func__,(int)time(NULL));
    }

    if ((MSG_TYPE_CAST_DLNA_START == ctl_msg.msg_type) || (MSG_TYPE_CAST_AIRCAST_START == ctl_msg.msg_type))
    {
        //Wait the win dlna is open, then start play to avoid the OSD
        // close slowly.
        // printf("[%s] wait dlna menu open start tick: %d\n",__func__,(int)time(NULL));
        //bool win_dlna_wait_open(uint32_t timeout);
        //win_dlna_wait_open(20000);
        // printf("[%s] wait dlna menu open end tick: %d\n",__func__,(int)time(NULL));
    }
}

static void network_probed_wifi(void)
{
    if (0 == access("/var/lib/misc/RTL8188FTV.probe", F_OK))
    {
        printf("Wi-Fi: RTL8188FTV\n");
        m_probed_wifi_module = HCCAST_NET_WIFI_8188FTV;
    }
    else if (0 == access("/var/lib/misc/RTL8811FTV.probe", F_OK))
    {
        printf("Wi-Fi: RTL8811FTV\n");
        m_probed_wifi_module = HCCAST_NET_WIFI_8811FTV;
    }

    hccast_wifi_mgr_set_wifi_model(m_probed_wifi_module);
}

void network_wifi_module_set(int wifi_module)
{
    m_probed_wifi_module = wifi_module;
    hccast_wifi_mgr_set_wifi_model(wifi_module);
}

int network_wifi_module_get(void)
{
    return m_probed_wifi_module;
}

int hostap_get_connect_count(void)
{
    return hostap_connect_count;
}

static void hostap_set_connect_count(int count)
{
    hostap_connect_count = count;
}


static int hostap_connect_update(void)
{
    int cur_connect = 0;
    int pre_connect = 0;
    static int needfresh = 0;
    control_msg_t ctl_msg = {0};

    //hostap not ready.
    if((hccast_wifi_mgr_get_hostap_status() == 0) || hccast_wifi_mgr_p2p_get_connect_stat() || factary_init)
    {
        needfresh = 0;
        hostap_set_connect_count(0);
        return 0;
    }


    if(hccast_wifi_mgr_p2p_get_connect_stat())
    {
        //printf("########%s mira connect do not anything.#########\n", __FUNCTION__);
        return 0;
    }

    cur_connect = hccast_wifi_mgr_hostap_get_sta_num(NULL);
    pre_connect = hostap_get_connect_count();

    if(hostap_discover_ok == 0)
    {
        //wait real connect success at first time.
        cur_connect = 0;
        pre_connect = 0;
    }


    if (cur_connect != pre_connect)
    {
        printf("########%s cur_connect=%d pre_connect=%d#########\n", __FUNCTION__, cur_connect, pre_connect);
        hostap_set_connect_count(cur_connect);
        if (cur_connect == 0)
        {
            //send msg notify ui no phone had connect to dongle.
            //if (needfresh == 1)
            {
                printf("====================== no phone connect===============\n");
                ctl_msg.msg_type = MSG_TYPE_NETWORK_DEVICE_BE_DISCONNECTED;
                api_control_send_msg(&ctl_msg);

                needfresh = 0;
                hostap_discover_ok = 0;
                hccast_ap_dlna_aircast_stop();
            }
        }
        else
        {
            //send msg notify ui have phone connect to dongle.
            //if (needfresh == 0)
            {
                printf("====================== new phone connect===============\n");

                ctl_msg.msg_type = MSG_TYPE_NETWORK_DEVICE_BE_CONNECTED;
                api_control_send_msg(&ctl_msg);

                needfresh = 1;
            }
        }
    }

    return 0;
}

static void *hostap_connect_thread(void *args)
{
    printf("----------------------------hccast_hostap_connect_thread is running.-----------------------------\n");
    while (1)
    {
        hostap_connect_update();
        api_sleep_ms(200);
    }
    return NULL;
}


static void hostap_connect_detect_init()
{
    pthread_t tid;

    if (pthread_create(&tid, NULL,hostap_connect_thread, NULL) < 0)
    {
        printf("Create hccast_hostap_connect_thread error.\n");
    }
}


int network_init(void)
{
    hccast_wifi_mgr_init(wifi_mgr_callback_func);
    hccast_httpd_service_init(httpd_callback_func);
    hccast_media_init(media_callback_func);

#ifdef DLNA_SUPPORT
    hccast_dlna_service_init(hccast_dlna_callback_func);
#endif

#ifdef MIRACAST_SUPPORT    
    hccast_mira_service_init(hccast_mira_callback_func);
#endif    

#ifdef AIRCAST_SUPPORT    
    hccast_air_service_init(hccast_air_callback_event);
#endif    

    hccast_scene_init();
    hccast_set_aspect_mode(1, 3);//16:9 as default.

    network_probed_wifi();
    hostap_connect_detect_init();

    hccast_httpd_service_start();

#if 0
    hccast_wifi_hostap_conf_t conf = {0};
    conf.channel = 1;
    strncpy(conf.pwd, "12345678", sizeof(conf.pwd));
    strncpy(conf.ssid, "ZHC", sizeof(conf.ssid));
    hccast_wifi_mgr_hostap_set_conf(&conf);
    hccast_wifi_mgr_hostap_start();

    hccast_httpd_service_start();
#endif
    return API_SUCCESS;
}

int network_deinit(void)
{
    hccast_wifi_mgr_uninit();
    hccast_httpd_service_uninit();
    hccast_mira_service_uninit();
    hccast_dlna_service_uninit();
    return API_SUCCESS;
}

static void hostap_config_init(void)
{
    hccast_wifi_hostap_conf_t conf = {0};
    int wifi_mode = hccast_wifi_mgr_freq_support_mode();


    //! The wpas conf will be overwritten here
    conf.mode    = date_mgr_get_device_wifi_mode();
    //printf("#### wifi_mode:%d, mode:%d\n", wifi_mode, conf.mode);
    if (conf.mode <= wifi_mode)
    {
        conf.mode = date_mgr_get_device_wifi_mode();
        conf.channel = date_mgr_get_device_wifi_channel();
    }
    else
    {
        app_data_t *app_data = data_mgr_app_get();
        conf.mode = app_data->wifi_mode = wifi_mode;
        if (HCCAST_WIFI_FREQ_MODE_24G == wifi_mode)
        {
            conf.channel =  app_data->wifi_ch = HOATAP_CHANNEL_24G;
        }
        else if (HCCAST_WIFI_FREQ_MODE_5G == wifi_mode)
        {
            conf.channel =  app_data->wifi_ch = HOATAP_CHANNEL_5G;
        }
        else
        {
            conf.channel =  app_data->wifi_ch = HOATAP_CHANNEL_24G;
        }
    }

    //strcpy(conf.country_code, "CN");
    strncpy(conf.pwd, data_mgr_get_device_psk(), sizeof(conf.pwd));
    strncpy(conf.ssid, data_mgr_get_device_name(), sizeof(conf.ssid));

    _log(DEMO, INFO, "wifi mode: %d, wifi ch: %d", conf.mode, conf.channel);

    hccast_wifi_mgr_hostap_set_conf(&conf);
}

static volatile bool m_wifi_connecting = false;
#define WIFI_CHECK_TIME 50000
static void *network_connect_task(void *arg)
{
    hccast_wifi_ap_info_t wifi_ap;
    int loop_cnt = WIFI_CHECK_TIME/100;
    m_wifi_connecting = true;
    control_msg_t ctl_msg = {0};

    while(!network_wifi_module_get())
    {
        api_sleep_ms(100);
        if (0 == loop_cnt--)
            goto connect_exit;
    }

    //step 1: http server startup
    //hccast_wifi_mgr_connect("TP-LINK_A422", "hichip1234");

    data_mgr_init_device_name();
    hostap_config_init();

#if 1
    //step 2:
    if (data_mgr_wifi_ap_get(&wifi_ap))
    {
        ctl_msg.msg_type = MSG_TYPE_NETWORK_WIFI_CONNECTING;
        api_control_send_msg(&ctl_msg);

        //Get wifi AP from flash, connect wifi
        printf("%s(), line:%d, connect to %s, pwd:%s\n", __FUNCTION__, __LINE__, \
               wifi_ap.ssid, wifi_ap.pwd);
        hccast_wifi_mgr_connect(&wifi_ap);
        if (hccast_wifi_mgr_get_connect_status())
        {
            hccast_wifi_mgr_udhcpc_start();
            api_sleep_ms(100);
        }
        else
        {
            hccast_wifi_mgr_disconnect_no_message();
            api_sleep_ms(50);
            hccast_wifi_mgr_hostap_start();
            hccast_mira_service_start();
        }
    }
    else
    {
        //No wif AP in flash, entering AP mode
        hccast_wifi_mgr_hostap_start();
        hccast_mira_service_start();

        printf("%s(), line:%d. AP mode start\n", __FUNCTION__, __LINE__);
    }
#endif
connect_exit:
    m_wifi_connecting = false;

    return NULL;
}

//Connet wifi ap if there is valid wifi AP information in flash. Otherwise
//set device to AP mdoe
int network_connect(void)
{

    if (m_wifi_connecting)
        return API_SUCCESS;

    pthread_t thread_id = 0;
    pthread_attr_t attr;
    //create the message task
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 0x2000);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED); //release task resource itself
    if(pthread_create(&thread_id, &attr, network_connect_task, NULL))
    {
        return API_FAILURE;
    }
    return API_SUCCESS;
}

