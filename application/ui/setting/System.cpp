/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-08-01 14:10:02
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-08-01 21:37:02
 * @FilePath: /gui/application/ui/setting/System.cpp
 * @Description: None
 * @other: None
 */
#include <stdio.h>
#include "System.h"

namespace Setting {

System::System(/* args */)
{
}

System::~System()
{
}

const char** System::GetStrArray(void)
{
    static const char* name[6] = {
    "系统设置", 
    "语言", "简体中文", 
    "菜单显示时间", "关", 
    "恢复出厂设置" 
    };
    return name;
}

int System::GetValue(int index)
{
    return 0;
}

void System::IncreaseValue(int index)
{
    switch (index)
    {
    case static_cast<int>(Setting_SystemLanguage):
        IncreaseValueComm(language, 0, sizeof(LanguageName)/sizeof(LanguageName[0]));
        break;

    case static_cast<int>(Setting_SystemOsdTime):
        IncreaseValueComm(OsdTime, 0, sizeof(OsdTimeName)/sizeof(OsdTimeName[0]));
        break;

    default:
        break;
    }
}

void System::DecreaseValue(int index)
{
    switch (index)
    {
    case static_cast<int>(Setting_SystemLanguage):
        DecreaseValueComm(language, 0, sizeof(LanguageName)/sizeof(LanguageName[0]));
        break;

    case static_cast<int>(Setting_SystemOsdTime):
        DecreaseValueComm(OsdTime, 0, sizeof(OsdTimeName)/sizeof(OsdTimeName[0]));
        break;

    default:
        break;
    }
}

const char* System::GetStr(int index)
{
    const char* name = nullptr;
    switch (index)
    {
    case static_cast<int>(Setting_SystemLanguage):
        name = LanguageName[language];
        break;

    case static_cast<int>(Setting_SystemOsdTime):
        name = OsdTimeName[OsdTime];
        break;

    default:
        break;
    }
    return name;
}

void* System::GetDerivedAddress(int index)
{
    return nullptr;
}

}
