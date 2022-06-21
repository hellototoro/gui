/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-13 20:20:58
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-06-18 21:53:35
 * @FilePath: /gui/application/ui/Music.h
 * @Description: None
 * @other: None
 */
#ifndef __MUSIC_H__
#define __MUSIC_H__

#include "lvgl/lvgl.h"
#ifdef HCCHIP_GCC
#include "hcapi/media_player.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HCCHIP_GCC
media_handle_t *MusicHandler;
#endif

lv_obj_t* creat_music_window(lv_obj_t* foucsed_obj);
void close_music_window(lv_obj_t* music_window);
void PlayMusic(char * file_name);
void SetCurrentMusicInfo(char* file_name, const lv_img_dsc_t* cover);
void ShowPlayList(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif


#endif
