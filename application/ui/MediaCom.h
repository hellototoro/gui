/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-13 13:31:36
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-06-15 13:13:19
 * @FilePath: /gui/application/ui/MediaCom.h
 * @Description: None
 * @other: None
 */
#ifndef __MEDIA_COM_H__
#define __MEDIA_COM_H__

#include "lvgl/lvgl.h"
#include "data_struct.h"
#ifdef HCCHIP_GCC
#include "hcapi/media_player.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HOST_GCC
typedef enum{
    MEDIA_TYPE_VIDEO,
    MEDIA_TYPE_MUSIC,
    MEDIA_TYPE_PHOTO,
    MEDIA_TYPE_COUNT,
} media_type_t;
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

MediaList* CreatMediaList(media_type_t media_type);
bool MediaListIsEmpty(media_type_t media_type);
void AddToMediaList(media_type_t media_type, char * media_name);
MediaList* GetMediaList(media_type_t media_type);
DLNode * GetNextMediaNode(MediaList* media_list, PlayListMode mode);
DLNode * GetPreMediaNode(MediaList* media_list, PlayListMode mode);
uint16_t GetMediaListSize(media_type_t media_type);
void DestroyMediaList(media_type_t media_type);
void DestroyAllMediaList(void);
void CreatMediaArray(media_type_t media_type);
uint16_t GetMediaArraySize(void);
uint16_t LocateMediaIndex(char * file_name);
char* GetCurrentMediaName(void);
char* GetNextMediaName(PlayListMode mode);
void DestroyMediaArray(void);

void SetMediaTotalTime(lv_obj_t * total_time_obj_);
void SetMediaProgress(lv_obj_t * progress_obj_);

#ifdef HCCHIP_GCC
int MediaMonitorInit(media_handle_t *media_hld);
int MediaMonitorDeinit(media_handle_t *media_hld);
void MediaMonitorTask(media_handle_t *media_hld);
#endif

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
