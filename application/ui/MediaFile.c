/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-05 13:39:11
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-06-08 15:39:00
 * @FilePath: /SOURCE/gui/application/ui/MediaFile.c
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
    file_list->OtherList = other_list;
    Push(file_list_stack, file_list);
    return file_list;
}

int GetDirNumber(FileList* file_list)
{
    return file_list->DirList->len;
}

int GetFileNumber(FileList* file_list)
{
    //return file_list->len;
    return file_list->DirList->len + file_list->OtherList->len;
}

FileStr* GetNextFileFromFileList(FileList* file_list)
{
    static LNode *next = NULL;
    static FileList* last_list = NULL;

    if (last_list != file_list) {
        last_list = file_list;
        next = file_list->DirList->head->next;
        if (next == NULL) {
            next = file_list->OtherList->head->next;
        }
    }
    else {
        next = next->next;
        if (next == NULL)
            next = file_list->OtherList->head->next;
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

static FileType GetFileType(char *file_name)
{
    char *file_extension = NULL;
    FileType file_type = FILE_OTHER;

    file_extension = strrchr(file_name, '.');
    file_extension++;
    if (file_extension != NULL) {
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
            file_type = FILE_IMAGE;
        }
        else if ( strcasecmp(file_extension, "txt")  == 0 || 
                  strcasecmp(file_extension, "log")  == 0 ) {
            file_type = FILE_TEXT;
        }
    }
    return file_type;
}
