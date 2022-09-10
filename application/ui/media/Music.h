/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-13 20:20:58
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-08-22 14:52:20
 * @FilePath: /gui/application/ui/media/Music.h
 * @Description: None
 * @other: None
 */
#ifndef __MUSIC_H__
#define __MUSIC_H__

#include "application/ui/ui_com.h"
#ifdef HCCHIP_GCC
#include "hcscreen/media_player.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HCCHIP_GCC
extern media_handle_t *MusicHandler;
#endif

extern lv_obj_t* creat_music_window(char* file_name);
extern void close_music_window(lv_obj_t* music_window);
extern void SetCurrentMusicTitle(char* file_name);
extern void SetCurrentMusicCover(const lv_img_dsc_t* cover);
extern void LoadLyric(char* music_file_name);
extern void RefreshLyric(uint32_t played_time);
extern void LyricTask(void);
extern void MusicCoverSpinRun(void);
extern void MusicCoverSpinStop(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif


#endif
