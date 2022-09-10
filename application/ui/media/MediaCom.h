/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-13 13:31:36
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-09-02 21:15:24
 * @FilePath: /gui/application/ui/media/MediaCom.h
 * @Description: None
 * @other: None
 */
#ifndef __MEDIA_COM_H__
#define __MEDIA_COM_H__

#include "application/ui/ui_com.h"
#include "media_list.h"
#include "MediaFile.h"
#ifdef HCCHIP_GCC
#include "hcscreen/media_player.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PlayedTime,
    ProgressSlider,
    TotalTime,
    PlayMode,
    Previous,
    Play,
    Next,
    PlayList,
    PlayBarNumber
} PlayBarList;

typedef enum
{
    CyclePlay,
    OrderPlay,
    OnlyOnePlay,
    RandPlay,
    PlayModeNumber
} PlayListMode;

typedef enum {
    AutoPlay,
    ManualPlay
} GetNextMode;

typedef DLNode MediaNode;
typedef LinkDList MediaList;
typedef char* file_name_t;
#ifdef HOST_GCC
typedef lv_obj_t       MediaHandle;
#elif defined(HCCHIP_GCC)
typedef media_handle_t MediaHandle;
#endif

extern char current_path[];
extern int current_path_size;
extern void MediaComInit(lv_obj_t* MediaScreen, MediaType media_type, MediaHandle* media_hdl);
extern void MediaComDeinit(void);
extern void AddToMediaList(MediaType media_type, char * media_name);
extern uint16_t GetMediaListSize(MediaType media_type);
extern void DestroyAllMediaList(void);
extern void CreateMediaArray(void);
extern uint16_t LocateMediaIndex(char * file_name);
extern char* GetCurrentMediaName(void);
extern void DestroyMediaArray(void);
extern void PlayMedia(char * file_name);

//公共ui部分
extern lv_obj_t* CreatePlayBar(lv_obj_t* parent);

#ifdef HCCHIP_GCC
extern void MediaMsgProc(media_handle_t *media_hld, HCPlayerMsg *msg);
#endif

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
