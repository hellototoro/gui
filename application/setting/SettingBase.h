/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-07-31 17:22:24
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-09-18 13:26:27
 * @FilePath: /gui/application/setting/SettingBase.h
 * @Description: None
 * @other: None
 */
#ifndef __SETTING_BASE_H__
#define __SETTING_BASE_H__

namespace Setting {
    
#define _2INT(x)  static_cast<int>(x)

class Base {
private:
    /* data */
public:
    Base(/* args */) {}
    virtual ~Base() {}

    void IncreaseValueComm(uint8_t& data, int MinRange, int MaxRange) {
        if (data < MaxRange - 1) ++data;
        else                     data = MinRange;
    }
    void DecreaseValueComm(uint8_t& data, int MinRange, int MaxRange) {
        if (data == MinRange) data = MaxRange - 1;
        else                  --data;
    }

    virtual int GetItemNum(void) = 0;
    virtual int GetOnlyTextItemNum(void) = 0;
    virtual const char** GetStrArray(void) = 0;
    virtual int GetUserValue(int index) = 0;
    virtual void SetUserValue(int index, int value) = 0;
    virtual void SelectedValue(int index) = 0;
    virtual void IncreaseUserValue(int index) = 0;
    virtual void DecreaseUserValue(int index) = 0;
    virtual const char* GetStr(int index) = 0;
    virtual void* GetDerivedAddress(int index) = 0;
};

}
#endif
