/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-08-01 14:09:36
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-08-16 22:23:29
 * @FilePath: /gui/application/setting/System.h
 * @Description: None
 * @other: None
 */
#ifndef __SYSTEM_SETTING_H__
#define __SYSTEM_SETTING_H__

#include <stdint.h>
#include "Base.h"

namespace Setting {

class System : public Base {
public:
    constexpr static uint8_t ItemNum = 3;
    constexpr static uint8_t OnlyTextItemNum = 1;

    enum SystemgSettingItemList {
        Setting_SystemLanguage = 1,
        Setting_SystemOsdTime,
        Setting_SystemRestoreFactory,
        Setting_SystemgDataItemNum
    };

    uint8_t language;
    //constexpr static const char* LanguageName[] = { 
    const char* LanguageName[6] = { 
    "setting_p_system_espaol",
    "setting_p_system_deutsch",
    "setting_p_system_english",
    "setting_p_system_italiano",
    "setting_p_system_france",
    "setting_p_system_chinese"
 };


    uint8_t OsdTime;
    //constexpr static const char* OsdTimeName[] = { 
    const char* OsdTimeName[7] = { 
    "setting_p_system_osd_time_off", 
    "setting_p_system_osd_time_5", 
    "setting_p_system_osd_time_10", 
    "setting_p_system_osd_time_15", 
    "setting_p_system_osd_time_20", 
    "setting_p_system_osd_time_25", 
    "setting_p_system_osd_time_30" };

    uint8_t RestoreFactory;
    //constexpr static const char* RestoreFactoryName[] = { 
    const char* RestoreFactoryName[1] = { 
    "setting_p_system_restore_factory_mode" };

public:
    System(/* args */);
    ~System();

    virtual int GetItemNum(void) { return ItemNum; }
    virtual int GetOnlyTextItemNum(void) { return OnlyTextItemNum; }
    virtual const char** GetStrArray(void);
    virtual int GetUserValue(int index);
    virtual void SetUserValue(int index, int value) { }
    virtual void SelectedValue(int index);
    virtual void IncreaseUserValue(int index);
    virtual void DecreaseUserValue(int index);
    virtual const char* GetStr(int index);
    virtual void* GetDerivedAddress(int index);

};

}
#endif
