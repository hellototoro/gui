/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-13 13:31:36
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-08-09 08:06:26
 * @FilePath: /gui/application/ui/media/MediaCom.h
 * @Description: None
 * @other: None
 */
#ifndef __MEDIA_COM_H__
#define __MEDIA_COM_H__

#include "application/ui/resource/ui.h"
#include "media_list.h"
#include "MediaFile.h"
#ifdef HCCHIP_GCC
#include "hcapi/media_player.h"
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
extern void MediaComInit(MediaType media_type, MediaHandle* media_hdl, lv_group_t* old_group);
extern void MediaComDeinit(void);
extern MediaList* CreateMediaList(MediaType media_type);
extern bool MediaListIsEmpty(MediaType media_type);
extern void AddToMediaList(MediaType media_type, char * media_name);
extern uint16_t GetMediaListSize(MediaType media_type);
extern void DestroyMediaList(MediaType media_type);
extern void DestroyAllMediaList(void);
extern void CreateMediaArray(MediaType media_type);
extern uint16_t GetMediaArraySize(MediaType media_type);
extern uint16_t LocateMediaIndex(MediaType media_type, char * file_name);
extern void SetMediaIndex(int index);
extern int GetMediaIndex(void);
extern char* GetCurrentMediaName(void);
extern char* GetPreMediaName(MediaType media_type, PlayListMode mode);
extern char* GetNextMediaName(MediaType media_type, PlayListMode mode, GetNextMode next_mode);
extern file_name_t* GetMediaArray(void);
extern void DestroyMediaArray(void);
extern void PlayMedia(MediaHandle* media_hal, char * file_name);
extern void LoadingMediaFileScreen(lv_obj_t* parent);
extern void CloseLoadingMediaFileScreen(void);

//公共ui部分
extern lv_obj_t* CreatePlayBar(lv_obj_t* parent);

#ifdef HCCHIP_GCC
extern void MediaMsgProc(media_handle_t *media_hld, HCPlayerMsg *msg);
#endif

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
