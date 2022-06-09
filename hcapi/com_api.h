/**
 * @file com_api.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-01-20
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef __COM_API_H__
#define __COM_API_H__

#include <stdio.h> //printf()
#include <stdlib.h>
#include <string.h> //memcpy()
#include <unistd.h> //usleep()
#include <stdbool.h> //bool
#include <stdint.h> //uint32_t

#include "cast_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CTL_MSG_COUNT   100
    
#define API_SUCCESS     (0)
#define API_FAILURE     (-1)

#define INVALID_VALUE_8     (0xFF)
#define INVALID_VALUE_16    (0xFFFF)
#define INVALID_VALUE_32    (0xFFFFFFFF)

#define OSD_MAX_WIDTH   1280
#define OSD_MAX_HEIGHT  720

#define SUB_NAME_X  140
#define SUB_NAME_Y  80
#define SUB_NAME_W  200

#define SUB_MENU_IMG_W  40
#define SUB_MENU_ITEM_W  400
#define SUB_MENU_ITEM_H  60

#define SUB_MENU_MAX    10

#define MAX_FILE_NAME 1024    


typedef enum TV_SYS_TYPE
{
    TV_MODE_AUTO = 0,
    TV_MODE_PAL,
    TV_MODE_PAL_M,//  PAL3.58
    TV_MODE_PAL_N,
    TV_MODE_NTSC358,//NTSC3.58
    TV_MODE_NTSC443,
    TV_MODE_SECAM,
#if 1
    TV_MODE_576P,
    TV_MODE_480P,
    TV_MODE_720P_50,
    TV_MODE_720P_60,
    TV_MODE_1080I_25,
    TV_MODE_1080I_30,   
    TV_MODE_BY_EDID,
    TV_MODE_1080P_50,
    TV_MODE_1080P_60,   
    TV_MODE_1080P_25,
    TV_MODE_1080P_30,   
    TV_MODE_1080P_24,
#endif
    TV_MODE_800_480,
    TV_MODE_1024X768_60,
    TV_MODE_1360X768_60,
    TV_MODE_1280X960_60,
    TV_MODE_1280X1024_60,
    TV_MODE_1024X768_50,
    TV_MODE_1080P_55,   
    TV_MODE_768X1024_60,
    TV_MODE_COUNT,
}TV_SYS_TYPE;


typedef enum{
    //key
    MSG_TYPE_KEY = 0,

    //message
    MSG_TYPE_MSG = 10,
    MSG_TYPE_OPEN_WIN,
    MSG_TYPE_NETWORK_CONNECTING,

    MSG_TYPE_MOUNT,
    MSG_TYPE_USB_UNMOUNT,
    MSG_TYPE_NETWORK_INIT_OK,
    MSG_TYPE_NETWORK_MAC_OK,
    MSG_TYPE_NETWORK_WIFI_CONNECTED,
    MSG_TYPE_NETWORK_WIFI_DISCONNECTED,
    MSG_TYPE_NETWORK_WIFI_PWD_WRONG,
    MSG_TYPE_NETWORK_DHCP_ON,
    MSG_TYPE_NETWORK_DHCP_OFF,
    MSG_TYPE_USB_PLUGIN,

    MSG_TYPE_CAST_DLNA_START,
    MSG_TYPE_CAST_DLNA_STOP,
    MSG_TYPE_CAST_AIRPLAY_START,
    MSG_TYPE_CAST_AIRPLAY_AUDIO_START,
    MSG_TYPE_CAST_AIRPLAY_STOP,
    MSG_TYPE_CAST_MIRACAST_START,
    MSG_TYPE_CAST_MIRACAST_STOP,

    //command
    MSG_TYPE_CMD = 1000
}msg_type_t;

typedef enum{
    CAST_STATE_IDLE = 0,
    CAST_STATE_AIRPLAY_PLAY,
    CAST_STATE_ARIPLAY_PAUSE,
    CAST_STATE_DLNA_PLAY,
    CAST_STATE_DLNA_PAUSE,
    CAST_STATE_MIRACAST_PLAY,
    CAST_STATE_MIRACAST_PAUSE,
}cast_play_state_t;

typedef enum{
    NETWORK_STATE_DISCONNECT = 0,
    NETWORK_STATE_DHCP_ON,
    NETWORK_STATE_DHCP_OFF,
    NETWORK_STATE_CONNECT,
}cast_network_state_t;

#define NETWORK_STATE_DISCONNECT         (0)
#define NETWORK_STATE_DHCP_ON            (1 << 0)
#define NETWORK_STATE_PHONE_CONNECT      (1 << 1) //the phone connnet to device OK
#define NETWORK_STATE_WIFI_AP_CONNECT    (1 << 2) //the devoce connect to wifi AP OK

typedef struct{
    msg_type_t	msg_type;
    uint32_t	msg_code;
}control_msg_t;

#define BACK_LOGO "/gui_files/HICHP_DLNA.m2v"

#define WLAN0_NAME      "wlan0"
#define WLAN1_NAME      "wlan1"

void hc_control_init();

extern int api_system_init(void);
extern int api_video_init(void);
extern int api_audio_init(void);

cast_play_state_t api_cast_play_state_get(void);
void api_cast_play_state_set(cast_play_state_t state);

int api_control_send_msg(control_msg_t *control_msg);
int api_control_receive_msg(control_msg_t *control_msg);
int api_control_send_key(uint32_t key);

int api_dis_show_onoff(bool on_off);
int api_logo_show(const char *file);
extern int api_logo_off(void);

bool api_is_ip_addr(char *ip_buff);
int api_get_mac_addr(char *mac);

extern void app_ffplay_init(void);
extern void app_ffplay_deinit(void);
extern void app_exit(void);


int webs_start();
int webs_stop();

void api_sleep_ms(uint32_t ms);
int api_shell_exe_result(char *cmd);

int api_lvgl_init(int width, int height);

#define SYS_HALT()      \
{                       \
    while(1);           \
}

#define ASSERT_API(expression)              \
    {                                   \
        if (!(expression))              \
        {                               \
            printf("assertion(%s) failed: file \"%s\", line %d\n",   \
                #expression, __FILE__, __LINE__);   \
            SYS_HALT();                    \
        }                               \
    }

#ifndef MKTAG
  #define MKTAG(a,b,c,d) ((a) | ((b) << 8) | ((c) << 16) | ((unsigned)(d) << 24))
#endif

#ifdef __cplusplus
} /*extern "C"*/
#endif
#endif
