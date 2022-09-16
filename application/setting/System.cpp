/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-08-01 14:10:02
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-09-11 15:39:46
 * @FilePath: /gui/application/setting/System.cpp
 * @Description: None
 * @other: None
 */
#include <stdio.h>
#include "System.h"
#include "application/ConfigParam.h"
#include "application/ui/LanguageScreen.h"
#include "application/ui/ui_com.h"
#ifdef HCCHIP_GCC
#include "hcapi/com_api.h"
#endif

namespace Setting {

System::System(/* args */)
{
    boost::property_tree::ptree config;
    ReadConfigFile(config, "system_setting");
    language = DefaultLanguageIndex;
    OsdTime = config.get<int>("OsdTime", 0);
}

System::~System()
{
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(ConfigFileName, pt);
    boost::property_tree::ptree config;
    config = pt.get_child("system_setting");
    config.put<int>("OsdTime", OsdTime);
    pt.put_child("system_setting",config);

    DefaultLanguageIndex = language;
    config = pt.get_child("default_language");
    config.put<std::string>("language", Language[language]);
    config.put<int>("index", DefaultLanguageIndex);
    pt.put_child("default_language",config);
    boost::property_tree::ini_parser::write_ini(ConfigFileName, pt);
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
            WriteConfigFile_I("guide_flag.flag", 1);
            WriteConfigFile_I("default_language.index", 2);
            WriteConfigFile_S("default_language.language", "en-GB");
            #ifdef HCCHIP_GCC
            CreateSpinBox(lv_scr_act(), _("setting_p_waiting"), 3, api_system_reboot);
            #else
            CreateSpinBox(lv_scr_act(), _("setting_p_waiting"), 3, nullptr);
            #endif
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
