/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-05 13:39:26
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-06-10 15:34:32
 * @FilePath: /gui/application/ui/MediaFile.h
 * @Description: None
 * @other: None
 */
#ifndef __UI_MEDIA_FILE_H__
#define __UI_MEDIA_FILE_H__

#include "FileType.h"
#include "data_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HOST_GCC
#define media_dir "/mnt"
#else
#define media_dir "/media"
#endif

typedef enum
{ 
    FILE_DIR,
    FILE_VIDEO,
    FILE_MUSIC,
    FILE_IMAGE,
    FILE_TEXT,
    FILE_OTHER,
    FILE_ALL,
} FileType;

typedef struct FileStr
{
    FileType type;
    uint8_t len;
    char name[100];
} FileStr;

typedef struct FileList 
{
    LinkList *DirList;
    LinkList *OtherList;
} FileList;


extern FileList *current_list;

extern void MediaFileInit(void);
extern FileList * GetFileList(char *path);
extern int GetDirNumber(FileList* file_list);
extern int GetFileNumber(FileList* file_list);
extern FileStr* GetNextFileFromFileList(FileList* file_list);
extern FileStr* GetNextFile(LinkList *list);
extern FileList * GetPreviousFileList(void);
extern bool IsRootPath(const char * path);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
