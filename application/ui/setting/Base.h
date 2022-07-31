/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-07-31 17:22:24
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-07-31 18:06:23
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

    virtual int GetItemNum(void) = 0;
    virtual int GetOnlyTextItemNum(void) = 0;
    virtual const char** GetStrArray(void) = 0;
    virtual void SelectData(int index) = 0;
    virtual void IncreaseData(int index) = 0;
    virtual void DecreaseData(int index) = 0;
    virtual const char* GetStr(int index) = 0;
};

}
#endif
