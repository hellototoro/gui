/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-13 13:31:36
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-06-13 15:14:18
 * @FilePath: /gui/application/ui/MediaCom.h
 * @Description: None
 * @other: None
 */
#ifndef __MEDIA_COM_H__
#define __MEDIA_COM_H__

#include "data_struct.h"
#ifdef HCCHIP_GCC
#include "hcapi/media_player.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef LinkDList VideoList;

extern char current_path[];

VideoList* CreatVideoList(void);
bool VideoListIsEmpty(VideoList* video_list);
void AddToVideoList(VideoList* video_list, char * video_name);
char * GetNextVideoName(VideoList* video_list);
char * GetPreVideoName(VideoList* video_list);

#ifdef HCCHIP_GCC
int MediaMonitorInit(media_handle_t *media_hld);
int MediaMonitorDeinit(media_handle_t *media_hld);
void MediaMonitorTask(media_handle_t *media_hld);
#endif

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
