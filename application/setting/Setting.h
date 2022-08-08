/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-07-31 17:25:49
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-08-04 19:17:00
 * @FilePath: /gui/application/setting/Setting.h
 * @Description: None
 * @other: None
 */
#ifndef __SETTING_H__
#define __SETTING_H__

#include "Picture.h"
#include "Sound.h"
#include "System.h"

namespace Setting {

extern void ReadPictureSettingParam(Picture& setting);
extern void ReadSoundSettingParam(Sound& setting);
extern void ReadSystemSettingParam(System& setting);
template <typename T> extern T* CreateSettingObj( void (*SettingInit)(T&) );
template <typename T> extern void DeleteSettingObj(T& SettingData);

}

#endif
