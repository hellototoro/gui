/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-13 13:31:36
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-06-18 17:23:51
 * @FilePath: /gui/application/ui/MediaCom.h
 * @Description: None
 * @other: None
 */
#ifndef __MEDIA_COM_H__
#define __MEDIA_COM_H__

#include "lvgl/lvgl.h"
#include "data_struct.h"
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
    RandPlay
} PlayListMode;

typedef DLNode MediaNode;
typedef LinkDList MediaList;
typedef char* file_name;
#ifdef HOST_GCC
typedef lv_obj_t       MediaHandle;
#elif defined(HCCHIP_GCC)
typedef media_handle_t MediaHandle;
#endif

extern char current_path[];

void MediaComInit(MediaType media_type, MediaHandle* media_hdl);
void MediaComDeinit(void);
MediaList* CreateMediaList(MediaType media_type);
bool MediaListIsEmpty(MediaType media_type);
void AddToMediaList(MediaType media_type, char * media_name);
MediaList* GetMediaList(MediaType media_type);
DLNode * GetNextMediaNode(MediaList* media_list, PlayListMode mode);
DLNode * GetPreMediaNode(MediaList* media_list, PlayListMode mode);
uint16_t GetMediaListSize(MediaType media_type);
void DestroyMediaList(MediaType media_type);
void DestroyAllMediaList(void);
void CreateMediaArray(MediaType media_type);
uint16_t GetMediaArraySize(MediaType media_type);
uint16_t LocateMediaIndex(MediaType media_type, char * file_name);
char* GetCurrentMediaName(void);
char* GetNextMediaName(MediaType media_type, PlayListMode mode);
file_name* GetMediaArray(void);
void DestroyMediaArray(void);
void PlayMedia(MediaHandle* media_hal, char * file_name);

//公共ui部分
lv_obj_t* CreatePlayBar(lv_obj_t* parent);

#ifdef HCCHIP_GCC
int MediaMonitorInit(media_handle_t *media_hld);
int MediaMonitorDeinit(media_handle_t *media_hld);
void MediaMonitorTask(void);
#endif

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
