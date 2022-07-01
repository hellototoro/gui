/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-05 13:39:11
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-06-30 13:02:18
 * @FilePath: /gui/application/ui/MediaFile.c
 * @Description: None
 * @other: None
 */
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "MediaFile.h"

LinkStack* file_list_stack;
FileList *current_list;

static FileType GetFileType(char *file_name);

void MediaFileInit(void)
{
    file_list_stack = (LinkStack *) malloc(sizeof (LinkStack));
    InitStack(file_list_stack);
    current_list = NULL;
}

void MediaFileDeInit(void)
{
    CloseFileList();
    free(file_list_stack);
}

FileList * GetFileList(char *path)
{
    DIR *dir;
    struct dirent *file;
    FileStr* file_node = NULL;
    FileList *file_list = (FileList*) malloc(sizeof(FileList));
    LinkList *dir_list = (LinkList*) malloc(sizeof(LinkList));
    LinkList *other_list = (LinkList*) malloc(sizeof(LinkList));
    if ((dir = opendir(path)) == NULL) {
        return NULL;
    }

    InitList(dir_list);
    InitList(other_list);
    while ((file = readdir(dir)) != NULL) {
        if (!strncmp(file->d_name, ".", 1)) {
            //skip the upper dir
            continue;
        }
        file_node = (FileStr*)malloc(sizeof(FileStr));
        strcpy(file_node->name, file->d_name);
        if (file->d_type == DT_DIR) { //dir
            file_node->type = FILE_DIR;
            ListAppend(dir_list, file_node);
        }
        else {
            file_node->type = GetFileType(file->d_name);
            ListAppend(other_list, file_node);
        }
    }
    file_list->DirList = dir_list;
    file_list->NonDirList = other_list;
    Push(file_list_stack, current_list);
    current_list = file_list;
    closedir(dir);
    return file_list;
}

/* 获取上一个文件列表，并删除当前文件列表 */
FileList * GetPreviousFileList(void)
{
    DestroyList(current_list->DirList);
    DestroyList(current_list->NonDirList);
    free(current_list);
    if (Pop(file_list_stack, (ElemType *)&current_list) == OK)
        return current_list;
    else 
        return NULL;
}

uint16_t GetDirNumber(FileList* file_list)
{
    if (file_list)
        return file_list->DirList->len;
    else
        return 0;
}

uint16_t GetNonDirNumber(FileList* file_list)
{
    if (file_list)
        return file_list->NonDirList->len;
    else
        return 0;
}

uint16_t GetFileNumber(FileList* file_list)
{
    if (file_list)
        return file_list->DirList->len + file_list->NonDirList->len;
    else
        return 0;
}

FileStr* GetNextFileFromFileList(FileList* file_list)
{
    static LNode *next = NULL;
    static FileList* last_list = NULL;

    if (file_list == NULL) {
        last_list = NULL;
        return NULL;
    }

    if (last_list != file_list) {
        last_list = file_list;
        next = file_list->DirList->head->next;
        if (next == NULL) {
            next = file_list->NonDirList->head->next;
        }
    }
    else {
        next = next->next;
        if (next == NULL)
            next = file_list->NonDirList->head->next;
    }
    if (next)
        return (FileStr*) next->data;
    else
        return NULL;
}

FileStr* GetNextFile(LinkList *list)
{
    static LNode *next = NULL;
    static LinkList* last_list = NULL;

    if (list == NULL) {
        last_list = NULL;
        return NULL;
    }

    if (last_list != list) {
        last_list = list;
        next = list->head->next;
    }
    else {
        if (next)
            next = next->next;
    }
    if (next)
        return (FileStr*) next->data;
    else
        return NULL;
}

bool IsRootPath(const char * path)
{
    return strcmp(media_dir, path) == 0 ? true : false;
}

void CloseFileList(void)
{
    while (GetPreviousFileList() != NULL);//清理栈里面所有内容
    DestroyStack(file_list_stack);
}

static FileType GetFileType(char *file_name)
{
    char *file_extension = NULL;
    FileType file_type = FILE_OTHER;

    file_extension = strrchr(file_name, '.');
    if (file_extension != NULL) {
        file_extension++;
        if ( strcasecmp(file_extension, "mp4")  == 0 || 
             strcasecmp(file_extension, "mpg")  == 0 || 
             strcasecmp(file_extension, "mpeg") == 0 || 
             strcasecmp(file_extension, "dat")  == 0 || 
             strcasecmp(file_extension, "vob")  == 0 || 
             strcasecmp(file_extension, "avi")  == 0 || 
             strcasecmp(file_extension, "ts")   == 0 || 
             strcasecmp(file_extension, "trp")  == 0 || 
             strcasecmp(file_extension, "tp")   == 0 || 
             strcasecmp(file_extension, "m2t")  == 0 || 
             strcasecmp(file_extension, "m2ts") == 0 || 
             strcasecmp(file_extension, "mts")  == 0 || 
             strcasecmp(file_extension, "mkv")  == 0 || 
             strcasecmp(file_extension, "mov")  == 0 || 
             strcasecmp(file_extension, "3gp")  == 0 || 
             strcasecmp(file_extension, "flv")  == 0 || 
             strcasecmp(file_extension, "rmvb") == 0 || 
             strcasecmp(file_extension, "rm")   == 0 || 
             strcasecmp(file_extension, "ram")  == 0 || 
             strcasecmp(file_extension, "webm") == 0 || 
             strcasecmp(file_extension, "h264") == 0 || 
             strcasecmp(file_extension, "es")   == 0 ) {
            file_type = FILE_VIDEO;
        }
        else if ( strcasecmp(file_extension, "mp3") == 0 || 
                  strcasecmp(file_extension, "mp2") == 0 || 
                  strcasecmp(file_extension, "mp1") == 0 || 
                  strcasecmp(file_extension, "mpa") == 0 || 
                  strcasecmp(file_extension, "ogg") == 0 || 
                  strcasecmp(file_extension, "aac") == 0 || 
                  strcasecmp(file_extension, "ac3") == 0 || 
                  strcasecmp(file_extension, "pcm") == 0 || 
                  strcasecmp(file_extension, "wav") == 0 || 
                  strcasecmp(file_extension, "wma") == 0 || 
                  strcasecmp(file_extension, "flac") == 0 ) {
            file_type = FILE_MUSIC;
        }
        else if ( strcasecmp(file_extension, "jpg")  == 0 || 
                  strcasecmp(file_extension, "jpeg") == 0 ||
                  strcasecmp(file_extension, "bmp")  == 0 ||
                  strcasecmp(file_extension, "gif")  == 0 ||
                  strcasecmp(file_extension, "png")  == 0 ) {
            file_type = FILE_PHOTO;
        }
        else if ( strcasecmp(file_extension, "txt")  == 0 || 
                  strcasecmp(file_extension, "log")  == 0 ) {
            file_type = FILE_TEXT;
        }
    }
    return file_type;
}
