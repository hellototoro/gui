/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-07-31 17:25:49
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-08-02 09:40:07
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

extern void PictureSettingInit(Picture* setting);
extern void SoundSettingInit(Sound* setting);
extern void SystemSettingInit(System* setting);

template <typename T>
T* CreateSettingObj( void (*SettingInit)(T*) )
{
    T* SettingData = new T;
    SettingInit(SettingData);
    return SettingData;
}

template <typename T>
void DeleteSettingObj(T* SettingData)
{
    delete SettingData;
}

}
#endif
