/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-13 20:20:58
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-06-24 16:14:43
 * @FilePath: /gui/application/ui/Music.h
 * @Description: None
 * @other: None
 */
#ifndef __MUSIC_H__
#define __MUSIC_H__

#ifdef HCCHIP_GCC
#include "hcapi/lvgl/lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif
#ifdef HCCHIP_GCC
#include "hcapi/media_player.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HCCHIP_GCC
extern media_handle_t *MusicHandler;
#endif

extern lv_obj_t* creat_music_window(lv_obj_t* foucsed_obj);
extern void close_music_window(lv_obj_t* music_window);
extern void SetCurrentMusicTitle(char* file_name);
extern void SetCurrentMusicCover(const lv_img_dsc_t* cover);
extern void LoadLyric(char* music_file_name);
extern void RefreshLyric(uint32_t played_time);
extern void LyricTask(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif


#endif
