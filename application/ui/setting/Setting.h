/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-07-31 17:25:49
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-08-01 21:43:08
 * @FilePath: /gui/application/ui/setting/Setting.h
 * @Description: None
 * @other: None
 */
#ifndef __SETTING_H__
#define __SETTING_H__

#include "Picture.h"
#include "Sound.h"
#include "System.h"

namespace Setting {

extern Picture* CreatePictureSettingObj(void);
extern void DeletePictureSettingObj(Picture* PictureData);
extern Sound* CreateSoundSettingObj(void);
extern void DeleteSoundSettingObj(Sound* sound);
extern System* CreateSystemSettingObj(void);
extern void DeleteSystemSettingObj(System* sound);

}
#endif
