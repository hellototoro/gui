/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-05 13:39:11
 * @FilePath: /gui/application/ui/media/MediaFile.c
 * @Description: None
 * @other: None
 */
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "MediaFile.h"
#include "application/ui/ui_com.h"

hstack_ptr_t file_list_stack;
FileList *current_list;

static FileType GetFileType(char *file_name);

void MediaFileInit(void)
{
    file_list_stack = hstack_create(sizeof(FileList*));
    current_list = NULL;
}

void MediaFileDeInit(void)
{
    CloseFileList();
}

FileList * GetFileList(char *path)
{
    DIR *dir;
    if (HasUsbDevice() == false) return NULL;
    if ((dir = opendir(path)) == NULL) return NULL;

    FileStr* file_node = NULL;
    hlist_ptr_t dir_list = hlist_create(sizeof(FileStr*));
    hlist_ptr_t other_list = hlist_create(sizeof(FileStr*));

    struct dirent *file;
    while ((file = readdir(dir)) != NULL) {
        if (strncmp(file->d_name, ".", 1) == 0) {//skip the upper dir
            continue;
        }
        file_node = (FileStr*)malloc(sizeof(FileStr));
        strcpy(file_node->name, file->d_name);
        if (file->d_type == DT_DIR) { //dir
            file_node->type = FILE_DIR;
            hlist_push_back(dir_list, &file_node, sizeof(file_node));
        }
        else {
            file_node->type = GetFileType(file->d_name);
            hlist_push_back(other_list, &file_node, sizeof(file_node));
        }
    }
    if (file_node == NULL) {
        hlist_destroy(dir_list);
        hlist_destroy(other_list);
        return NULL;
    }
    FileList *file_list = (FileList*) malloc(sizeof(FileList));
    file_list->DirList = dir_list;
    file_list->NonDirList = other_list;
    hstack_push(file_list_stack, &file_list, sizeof(file_list), NULL);
    current_list = file_list;
    closedir(dir);
    return file_list;
}

/* 获取上一个文件列表，并删除当前文件列表 */
FileList * GetPreviousFileList(void)
{
    if (hstack_empty(file_list_stack)) return NULL;
    hlist_destroy(current_list->DirList);
    hlist_destroy(current_list->NonDirList);
    free(current_list);
    hstack_pop(file_list_stack);
    current_list = DATA_CAST(FileList*)hstack_top(file_list_stack);
    return current_list;
}

uint16_t GetDirNumber(FileList* file_list)
{
    if (file_list)
        return list_size(file_list->DirList);
    else
        return 0;
}

uint16_t GetNonDirNumber(FileList* file_list)
{
    if (file_list)
        return list_size(file_list->NonDirList);
    else
        return 0;
}

uint16_t GetFileNumber(FileList* file_list)
{
    if (file_list)
        return list_size(file_list->DirList) + list_size(file_list->NonDirList);
    else
        return 0;
}

bool IsRootPath(const char * path)
{
    return strcmp(media_dir, path+1) == 0 ? true : false;
}

void CloseFileList(void)
{
    for (int i = 0; i < hstack_size(file_list_stack); ++i) {
        current_list = DATA_CAST(FileList *)hstack_top(file_list_stack);
        hlist_destroy(current_list->DirList);
        hlist_destroy(current_list->NonDirList);
        free(current_list);
        hstack_pop(file_list_stack);
    }
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
