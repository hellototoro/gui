/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-13 13:31:36
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-06-17 13:59:56
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

typedef enum
{
    CyclePlay,
    OrderPlay,
    OnlyOnePlay,
    RandPlay
} PlayListMode;

typedef DLNode MediaNode;
typedef LinkDList MediaList;

extern char current_path[];

MediaList* CreatMediaList(MediaType media_type);
bool MediaListIsEmpty(MediaType media_type);
void AddToMediaList(MediaType media_type, char * media_name);
MediaList* GetMediaList(MediaType media_type);
DLNode * GetNextMediaNode(MediaList* media_list, PlayListMode mode);
DLNode * GetPreMediaNode(MediaList* media_list, PlayListMode mode);
uint16_t GetMediaListSize(MediaType media_type);
void DestroyMediaList(MediaType media_type);
void DestroyAllMediaList(void);
void CreatMediaArray(MediaType media_type);
uint16_t GetMediaArraySize(MediaType media_type);
uint16_t LocateMediaIndex(MediaType media_type, char * file_name);
char* GetCurrentMediaName(void);
char* GetNextMediaName(MediaType media_type, PlayListMode mode);
void DestroyMediaArray(void);

void SetMediaTotalTime(lv_obj_t * total_time_obj_);
void SetMediaProgress(lv_obj_t * progress_obj_);

#ifdef HCCHIP_GCC
int MediaMonitorInit(media_handle_t *media_hld);
int MediaMonitorDeinit(media_handle_t *media_hld);
void MediaMonitorTask(void);
#endif

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
