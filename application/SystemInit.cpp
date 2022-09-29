/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-08-04 19:12:38
 * @FilePath: /gui/application/SystemInit.cpp
 * @Description: None
 * @other: None
 */
#include "SystemInit.h"
#include "ConfigParam.h"
#include "windows.h"
#include "lv_i18n/src/lv_i18n.h"

/**********外部变量声明***************/
extern ActiveScreen CurrentScreen;
extern ActiveScreen DefaultScreen;
extern int DefaultLanguageIndex;
extern uint8_t Volume;

void SystemInit(void)
{
    //step1 读取开机向导标志
    boost::property_tree::ptree config;
    ReadConfigFile(config, "guide_flag");
    bool guide_flag = static_cast<bool>(config.get<int>("flag", 1));
    DefaultScreen = guide_flag ? LanguageScreen : HomeScreen;
    if(DefaultScreen == HomeScreen) {
        ReadConfigFile(config, "default_language");
        DefaultLanguageIndex = config.get<int>("index", 2);
        lv_i18n_init(lv_i18n_language_pack);
        lv_i18n_set_locale(config.get<std::string>("language", "en-GB").c_str());
    }
    CurrentScreen = DefaultScreen;

    //step2 读取音量
    ReadConfigFile(config, "sound_setting");
    Volume = config.get<int>("volume", 50);
}
