/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-27 21:52:32
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-06-28 15:47:11
 * @FilePath: /gui/application/ui/Volume.h
 * @Description: None
 * @other: None
 */
#ifndef __VOLUME_H__
#define __VOLUME_H__

#ifdef HCCHIP_GCC
#include "hcapi/lvgl/lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif

extern void SetVolume(uint32_t value);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
