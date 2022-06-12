/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-12 18:49:59
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-06-12 19:59:48
 * @FilePath: /gui/application/ui/Video.c
 * @Description: None
 * @other: None
 */
#include <stdlib.h>
#include "Video.h"


VideoList* CreatVideoList(void)
{
    VideoList* video_list = (VideoList*) malloc(sizeof(VideoList));
    InitDList(video_list);
    return video_list;
}

bool VideoListIsEmpty(VideoList* video_list)
{
    return (video_list->len ==0 ) ? true : false;
}

void AddToVideoList(VideoList* video_list, char * video_name)
{
    DListAppend(video_list, video_name);
}

char * GetNextVideoName(VideoList* video_list)
{
    static DLNode *next = NULL;
    static DLNode *head = NULL;
    static VideoList* last_list = NULL;

    if (video_list == NULL) {//清除获取记录
        last_list = NULL;
        next = NULL;
        head = NULL;
        return NULL;
    }

    if ((next == head) && (head != NULL)) { //一个循环结束
        next = head->next;
    }

    if (last_list != video_list) {
        last_list = video_list;
        head = video_list->head;
        next = head->next;
    }
    else {
        next = next->next;
    }
    return (char*) next->data;
}

char * GetPreVideoName(VideoList* video_list)
{
    static DLNode *pre = NULL;
    static DLNode *head = NULL;
    static VideoList* last_list = NULL;

    if (video_list == NULL) {//清除获取记录
        last_list = NULL;
        pre = NULL;
        head = NULL;
        return NULL;
    }

    if ((pre == head) && (head != NULL)) { //一个循环结束
        pre = head->pre;
    }

    if (last_list != video_list) {
        last_list = video_list;
        head = video_list->head;
        pre = head->pre;
    }
    else {
        pre = pre->pre;
    }
    return (char*) pre->data;
}