/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-07-31 17:22:24
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-08-01 12:17:28
 * @FilePath: /gui/application/ui/setting/Base.h
 * @Description: None
 * @other: None
 */
#ifndef __SETTING_BASE_H__
#define __SETTING_BASE_H__

namespace Setting {

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
    virtual int GetValue(int index) = 0;
    virtual void IncreaseValue(int index) = 0;
    virtual void DecreaseValue(int index) = 0;
    virtual const char* GetStr(int index) = 0;
    virtual void* GetDerivedAddress(int index) = 0;
};

}
#endif
