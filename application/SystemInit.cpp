/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-08-04 19:12:38
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-09-02 01:40:56
 * @FilePath: /gui/application/SystemInit.cpp
 * @Description: None
 * @other: None
 */
#include "SystemInit.h"
#include "ConfigParam.h"
#include "windows.h"
#include "ui/LanguageScreen.h"
#include "lv_i18n/src/lv_i18n.h"

void SystemInit(void)
{
    //step1 读取开机向导标志
    boost::property_tree::ptree config;
    ReadConfigFile(config, "guide_flag");
    bool guide_flag = static_cast<bool>(config.get<int>("flag", 1));
    DefaultScreen = guide_flag ? LanguageScreen : HomeScreen;
    if(DefaultScreen == HomeScreen) {
        ReadConfigFile(config, "default_language");
        DefaultLanguage = config.get<std::string>("language", DefaultLanguage).c_str();
        DefaultLanguageIndex = config.get<int>("index", DefaultLanguageIndex);
        lv_i18n_init(lv_i18n_language_pack);
        lv_i18n_set_locale(DefaultLanguage);
    }
    CurrentScreen = DefaultScreen;
}
