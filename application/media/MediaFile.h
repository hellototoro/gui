/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-05 13:39:26
 * @FilePath: /gui/application/ui/media/MediaFile.h
 * @Description: None
 * @other: None
 */
#ifndef __UI_MEDIA_FILE_H__
#define __UI_MEDIA_FILE_H__

#include <stdint.h>
#include "media_list.h"
#include "list/hlist.h"

#ifdef __cplusplus
extern "C" {
#endif

#define media_dir "~/tmp/lvgl_test_dir"

typedef enum
{ 
    FILE_DIR,
    FILE_VIDEO,
    FILE_MUSIC,
    FILE_PHOTO,
    FILE_TEXT,
    FILE_OTHER,
    FILE_ALL,
} FileType;

typedef enum{
    MEDIA_VIDEO = FILE_VIDEO,
    MEDIA_MUSIC,
    MEDIA_PHOTO,
    MEDIA_TEXT,
    MEDIA_MAX
} MediaType;

typedef struct FileStr
{
    uint8_t len;
    FileType type;
    char name[100];
} FileStr;

typedef struct FileList
{
    hlist_ptr_t DirList;
    hlist_ptr_t NonDirList;
} FileList;


extern FileList *current_list;

extern void MediaFileInit(void);
extern FileList* GetFileList(char *path);
extern uint16_t GetDirNumber(FileList* file_list);
extern uint16_t GetNonDirNumber(FileList* file_list);
extern uint16_t GetFileNumber(FileList* file_list);
extern FileList* GetPreviousFileList(void);
extern bool IsRootPath(const char * path);
extern void CloseFileList(void);
extern void MediaFileDeInit(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
