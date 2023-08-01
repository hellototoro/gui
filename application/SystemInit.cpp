/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-08-04 19:12:38
 * @FilePath: /gui/application/SystemInit.cpp
 * @Description: None
 * @other: None
 */
#include "SystemInit.h"
#include "SysParam/SysParam.h"
#include "windows.h"
#include "lv_i18n/src/lv_i18n.h"

/**********外部变量声明***************/
extern ActiveScreen CurrentScreen;
extern ActiveScreen DefaultScreen;
extern int DefaultLanguageIndex;
extern uint8_t Volume;

void SystemInit(void)
{
    SysParam sys_param;
    //step1 读取开机向导标志
    bool guide_flag = static_cast<bool>(sys_param.read<int>("guide_flag","flag"));
    DefaultScreen = guide_flag ? LanguageScreen : HomeScreen;
    if(DefaultScreen == HomeScreen) {
        DefaultLanguageIndex = sys_param.read<int>("default_language", "index");
        lv_i18n_init(lv_i18n_language_pack);
        lv_i18n_set_locale(sys_param.read<std::string>("language", "en-GB").c_str());
    }
    CurrentScreen = DefaultScreen;

    //step2 读取音量
    Volume = sys_param.read<int>("sound_setting", "volume");
}
