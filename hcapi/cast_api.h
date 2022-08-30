/**
 * @file cast_api.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-01-20
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef __CAST_API_H__
#define __CAST_API_H__

#ifdef DLNA_SUPPORT
#include <hccast/hccast_dlna.h>
#endif

#ifdef AIRCAST_SUPPORT
#include <hccast/hccast_air.h>
#endif

#ifdef MIRACAST_SUPPORT
#include <hccast/hccast_mira.h>
#endif

#include <hccast/hccast_scene.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef enum{
    CAST_TYPE_AIRCAST = 0,
    CAST_TYPE_DLNA,
    CAST_TYPE_MIRACAST,

    CAST_TYPE_NONE,
}cast_type_t;


int cast_get_service_name(cast_type_t cast_type, char *service_name, int length);
void cast_restart_services(void);

#ifdef DLNA_SUPPORT
int hccast_dlna_callback_func(hccast_dlna_event_e event, void* in, void* out);
#else
int hccast_dlna_service_uninit(void);
int hccast_dlna_service_start(void);
int hccast_dlna_service_stop(void);
#endif

#ifdef MIRACAST_SUPPORT
int hccast_mira_callback_func(hccast_mira_event_e event, void* in, void* out);
#else
int hccast_mira_service_start(void);
int hccast_mira_service_stop(void);
int hccast_mira_player_init(void);
int hccast_mira_get_stat(void);
int hccast_mira_service_uninit(void);
#endif

#ifdef AIRCAST_SUPPORT
int hccast_air_callback_event(hccast_air_event_e event, void* in, void* out);
#else
int hccast_air_ap_mirror_stat(void);
int hccast_air_ap_audio_stat(void);
int hccast_air_service_start(void);
int hccast_air_service_stop(void);
void hccast_air_mdnssd_start(void);
void hccast_air_mdnssd_stop(void);
void hccast_air_mediaplayer_2_aircast_event(int type, void *param);
int hccast_air_ap_get_mirror_frame_num(void);
int hccast_air_service_is_start(void);
#endif

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif



