/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-12 18:50:08
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-06-12 19:59:52
 * @FilePath: /gui/application/ui/Video.h
 * @Description: None
 * @other: None
 */
#ifndef __VIDEO_H__
#define __VIDEO_H__

#include "data_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef LinkDList VideoList;

VideoList* CreatVideoList(void);
bool VideoListIsEmpty(VideoList* video_list);
void AddToVideoList(VideoList* video_list, char * video_name);
char * GetNextVideoName(VideoList* video_list);
char * GetPreVideoName(VideoList* video_list);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
