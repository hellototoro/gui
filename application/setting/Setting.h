/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-07-31 17:25:49
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-09-01 23:33:40
 * @FilePath: /gui/application/setting/Setting.h
 * @Description: None
 * @other: None
 */
#ifndef __SETTING_H__
#define __SETTING_H__

#include "Picture.h"
#include "Sound.h"
#include "System.h"
#include "NetWork.h"

namespace Setting {

extern void ReadPictureSettingParam(Picture& setting);
extern void ReadSoundSettingParam(Sound& setting);
extern void ReadSystemSettingParam(System& setting);
extern void WritePictureSettingParam(Picture& setting);
extern void WriteSoundSettingParam(Sound& setting);
extern void WriteSystemSettingParam(System& setting);
template <typename T> extern T* CreateSettingObj( void (*ReadSettingParam)(T&) );
template <typename T> extern void DeleteSettingObj(T* SettingData, void (*WriteSettingParam)(T&) );

}

#endif
