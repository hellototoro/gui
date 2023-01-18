/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2023-01-18 15:26:31
 * @FilePath: /gui/application/setting/Position.h
 * @Description: None
 * @other: None
 */
#ifndef __SYSTEM_POSITION_H__
#define __SYSTEM_POSITION_H__

#include <stdint.h>
#include "SettingBase.h"

namespace Setting {

class Position : public Base {
public:
    constexpr static uint8_t ItemNum = 4;
    constexpr static uint8_t OnlyTextItemNum = 4;

    enum PositiongSettingItemList {
        Setting_Position0 = 1, 
        Setting_Position1, 
        Setting_Position2, 
        Setting_Position3, 
        Setting_PositiongItemNum 
    };

    uint8_t position_mode;

public:
    Position(/* args */);
    ~Position();

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
