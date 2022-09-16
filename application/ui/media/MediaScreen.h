/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-05-23 13:51:29
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-08-31 12:34:23
 * @FilePath: /gui/application/ui/media/MediaScreen.h
 * @Description: None
 * @other: None
 */
#ifndef __UI_MEDIA_SCREEN_H__
#define __UI_MEDIA_SCREEN_H__

#include "application/ui/ui_com.h"
#include "MediaFile.h"
#ifdef HCCHIP_GCC
#include "hcapi/com_api.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MediaFile_All,
    MediaFile_Vedio = FILE_VIDEO,
    MediaFile_Music,
    MediaFile_Photo,
    MediaFile_Text,
    MediaFile_CategoryNumber
} MediaFileCategoryList;

extern MediaFileCategoryList FileFilter;

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
