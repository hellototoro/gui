/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-08-01 14:10:02
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-09-18 23:37:50
 * @FilePath: /gui/application/setting/System.cpp
 * @Description: None
 * @other: None
 */
#include <stdio.h>
#include "System.h"
#include "SysParam/SysParam.h"
#include "application/ui/LanguageScreen.h"
#include "application/ui/ui_com.h"
#include "application/ui/SettingScreen.h"

namespace Setting {

System::System(/* args */)
{
    SysParam sys_param;
    language = DefaultLanguageIndex;
    OsdTime = sys_param.read<int>("system_setting", "OsdTime");
}

System::~System()
{
    SysParam sys_param;
    sys_param.write<int>("system_setting", "OsdTime", OsdTime);

    DefaultLanguageIndex = language;
    sys_param.write<std::string>("default_language", "language", Language[language]);
    sys_param.write<int>("default_language", "index", DefaultLanguageIndex);
}

const char** System::GetStrArray(void)
{
    static const char* name[6] = {
    "setting_p_system_setting", 
    "setting_p_system_language", "", 
    "setting_p_system_osd_time", "", 
    "setting_p_system_restore_factory_mode" 
    };
    name[2] = LanguageName[language];
    name[4] = OsdTimeName[OsdTime];
    return name;
}

int System::GetUserValue(int index)
{
    return 0;
}

void System::SelectedValue(int index)
{
    switch (index)
    {
    case static_cast<int>(Setting_SystemRestoreFactory):
    {
        CreateMsgBox(lv_scr_act(), _("setting_p_be_sure"), 2, [] () {
            SysParam sys_param;
            sys_param.write<int>("guide_flag", "flag", 1);
            sys_param.write<int>("default_language", "index", 2);
            sys_param.write<std::string>("default_language", "language", "en-GB");
            CreateSpinBox(lv_scr_act(), _("setting_p_waiting"), 3, nullptr);
        });
        
    }
    break;

    default:
        break;
    }
}

void System::IncreaseUserValue(int index)
{
    switch (index)
    {
    case static_cast<int>(Setting_SystemLanguage):
        IncreaseValueComm(language, 0, sizeof(LanguageName)/sizeof(LanguageName[0]));
        lv_i18n_set_locale(Language[language]);
        refresh_all_lable_text(lv_scr_act());
        break;

    case static_cast<int>(Setting_SystemOsdTime):
        IncreaseValueComm(OsdTime, 0, sizeof(OsdTimeName)/sizeof(OsdTimeName[0]));
        SetOsdTime(OsdTime);
        break;

    default:
        break;
    }
}

void System::DecreaseUserValue(int index)
{
    switch (index)
    {
    case static_cast<int>(Setting_SystemLanguage):
        DecreaseValueComm(language, 0, sizeof(LanguageName)/sizeof(LanguageName[0]));
        lv_i18n_set_locale(Language[language]);
        refresh_all_lable_text(lv_scr_act());
        break;

    case static_cast<int>(Setting_SystemOsdTime):
        DecreaseValueComm(OsdTime, 0, sizeof(OsdTimeName)/sizeof(OsdTimeName[0]));
        SetOsdTime(OsdTime);
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
