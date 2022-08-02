/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-08-01 14:09:36
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-08-01 21:42:43
 * @FilePath: /gui/application/ui/setting/System.h
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
    "西班牙语", "德语", "英语", "意大利语", "法语", "简体中文" };


    uint8_t OsdTime;
    //constexpr static const char* OsdTimeName[] = { 
    const char* OsdTimeName[7] = { 
    "关", "5秒", "10秒", "15秒", "20秒", "25秒", "30秒" };

    uint8_t RestoreFactory;
    //constexpr static const char* RestoreFactoryName[] = { 
    const char* RestoreFactoryName[1] = { 
    "恢复出厂设置" };

public:
    System(/* args */);
    ~System();

    virtual int GetItemNum(void) { return ItemNum; }
    virtual int GetOnlyTextItemNum(void) { return OnlyTextItemNum; }
    virtual const char** GetStrArray(void);
    virtual int GetValue(int index);
    virtual void IncreaseValue(int index);
    virtual void DecreaseValue(int index);
    virtual const char* GetStr(int index);
    virtual void* GetDerivedAddress(int index);

};

}
#endif
