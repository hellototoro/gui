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
#include "com_api.h"
#include "com_api.h"

#include "cast_api.h"


#ifdef CAST_AIRCAST_ENABLE
#include <libaircast/aircast_service.h>
static AIRCAST_SERVICE_CALLBACK_t m_aircast_cls;
#endif

#if CAST_DLNA_ENABLE
#include <Platinum/PltUPnP.h>
#include <Platinum/PltMediaRenderer.h>
#include <libdlna-porting/MediaRenderer.h>
#endif

#define CAST_SERVICE_NAME               "HCcast"
#define CAST_AIRCAST_SERVICE_NAME       "HCcast"
#define CAST_DLNA_FIRENDLY_NAME         "HCcast"
#define CAST_MIRACAST_NAME              "HCcast"


int cast_get_service_name(cast_type_t cast_type, char *service_name, int length)
{
    char mac_addr[6] = {0};
    char service_prefix[32] = CAST_SERVICE_NAME;

    snprintf(service_prefix, sizeof(service_prefix)-1, "%s", CAST_SERVICE_NAME);
    if (0 != api_get_mac_addr(mac_addr))
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

#ifdef CAST_AIRCAST_ENABLE
static void aircast_start_callback(AIRCAST_SERVICE_CALLBACK_t *cls, AIRCAST_SERVICE_STATUS_t status)
{
    control_msg_t msg;

    if (CAST_STATE_DLNA_PLAY == api_cast_play_state_get() ||
        CAST_STATE_DLNA_PAUSE == api_cast_play_state_get()){
        //stop DLNA play
    }
    else if (CAST_STATE_MIRACAST_PLAY == api_cast_play_state_get() ||
        CAST_STATE_MIRACAST_PAUSE == api_cast_play_state_get()){
        //stop miracast play
    }

    api_cast_play_state_set(CAST_STATE_AIRCAST_PLAY);

    //
    if (AIRCAST_SERVICE_AUDIO == status) 
        msg.msg_type = MSG_TYPE_CAST_AIRCAST_AUDIO_START;
    else
        msg.msg_type = MSG_TYPE_CAST_AIRCAST_START;

    api_control_send_msg(&msg);
	printf("aircast start\n");
}

static void aircast_stop_callback(AIRCAST_SERVICE_CALLBACK_t *cls)
{
    control_msg_t msg;
    msg.msg_type = MSG_TYPE_CAST_AIRCAST_STOP;

    api_cast_play_state_set(CAST_STATE_IDLE);
    api_control_send_msg(&msg);
	printf("aircast stop\n");
}

static void aircast_update_status_cb(AIRCAST_SERVICE_CALLBACK_t *cls, AIRCAST_SERVICE_STATUS_t status)
{
	printf("%s:%d, status: %d\n",__func__, __LINE__, status);

    switch (status)
    {
    case AIRCAST_SERVICE_STOP:
        aircast_stop_callback(cls);
        break;
    case AIRCAST_SERVICE_PLAYING:
    case AIRCAST_SERVICE_MIRRORING:
    case AIRCAST_SERVICE_AUDIO:
        aircast_start_callback(cls, status);
        break;
    default:
        break;
    }
}


void cast_airpaly_open()
{
    char service_name[64] = {0};
    cast_get_service_name(CAST_TYPE_NONE, service_name, sizeof(service_name));

    memset(&m_aircast_cls, 0, sizeof(AIRCAST_SERVICE_CALLBACK_t));
    m_aircast_cls.aircast_update_status_cb = aircast_update_status_cb;
    strcpy(m_aircast_cls.aircast_device_name, service_name);
    
	aircast_service_start(&m_aircast_cls, false, false);
}

/**
 * @brief stop the service of aircast
 * 
 */
void cast_airpaly_close()
{
	aircast_service_stop();
}
#endif    


#if CAST_DLNA_ENABLE
static void dlna_play_callback()
{
    control_msg_t msg;
    memset(&msg, 0, sizeof(control_msg_t));

    if (CAST_STATE_AIRCAST_PLAY == api_cast_play_state_get() ||
        CAST_STATE_AIRCAST_PLAY == api_cast_play_state_get()){
        //stop aircast
    }
    else if (CAST_STATE_MIRACAST_PLAY == api_cast_play_state_get() ||
        CAST_STATE_MIRACAST_PAUSE == api_cast_play_state_get()){
        //stop miracast
    }

    api_cast_play_state_set(CAST_STATE_DLNA_PLAY);

    msg.msg_type = MSG_TYPE_CAST_DLNA_START;
    api_control_send_msg(&msg);
    printf("dlna start\n");    
}

static void dlna_stop_callback()
{
    control_msg_t msg;
    memset(&msg, 0, sizeof(control_msg_t));

    msg.msg_type = MSG_TYPE_CAST_DLNA_STOP;
    api_cast_play_state_set(CAST_STATE_IDLE);
    api_control_send_msg(&msg);
    printf("dlna stop\n");
}

class MediaRendererObserverTest :public MediaRendererObserver
{
    void Play(void)
    {
        printf("%s()\n", __FUNCTION__);
        dlna_play_callback();
    };
    void Pause(void)
    {
        printf("%s()\n", __FUNCTION__);
    };
    void Stop(void)
    {
        printf("%s()\n", __FUNCTION__);
        dlna_stop_callback();
    };
    void BufferProgress(int val)
    {
        printf("%s()\n", __FUNCTION__);
    };
    void VolumeChange(int val)
    {
        printf("%s(), val:%d\n", __FUNCTION__, val);
    };
    void PlayerEvent(HCPlayerMsgType event, HCPlayerMsg)
    {
        printf(" ");
    };
    void *userdata = NULL;
};

static const char *default_uuid = "43f4f224-4848-11ec-afcd-1f4dfee3a432";
static PLT_UPnP m_upnp_render;
static MediaRenderer *m_media_render = NULL;
static class MediaRendererObserverTest m_dlna_observer;


void cast_dlna_open()
{
    char service_name[64] = {0};
    cast_get_service_name(CAST_TYPE_NONE, service_name, sizeof(service_name));

    // api_logo_off();
    // usleep(10000);

    printf("%s, line: %d, service_name: %s\n", __FUNCTION__, __LINE__, service_name);
    NPT_LogManager::GetDefault().Configure("plist:.level=INFO;.handlers=ConsoleHandler;.ConsoleHandler.colors=off;.ConsoleHandler.filter=24");

//    hcplayer_init((HCPlayerLogLevel)1);

    m_media_render = new MediaRenderer(service_name, false,
                                       default_uuid, 0, &m_dlna_observer); 
    PLT_DeviceHostReference device(m_media_render);

    device->m_ModelDescription = "Platinum File Media Render";
    device->m_ModelURL = "http://www.plutinosoft.com/";
    device->m_ModelNumber = "1.0";
    device->m_ModelName = "Platinum File Media Server";
    device->m_Manufacturer = "Plutinosoft";
    device->m_ManufacturerURL = "http://www.plutinosoft.com/";
    device->m_PresentationURL = "http://10.0.2.15:8080/";

    m_upnp_render.AddDevice(device);

    m_upnp_render.Start();
    m_upnp_render.Stop();
    m_upnp_render.Start();

}

void cast_dlna_close()
{
    m_upnp_render.Stop();
    PLT_DeviceHostReference device(m_media_render);
    m_upnp_render.RemoveDevice(device);
    
}
#endif


#ifdef CAST_MIRACAST_ENABLE    
void cast_miracast_open()
{
    char service_name[64] = {0};
    cast_get_service_name(CAST_TYPE_NONE, service_name, sizeof(service_name));

}

void cast_miracast_close()
{

}

#endif
