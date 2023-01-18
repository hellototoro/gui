/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2023-01-18 15:26:31
 * @FilePath: /gui/application/setting/Position.cpp
 * @Description: None
 * @other: None
 */
#include <stdio.h>
#include "Position.h"
#include "application/ConfigParam.h"
#include "application/ui/ui_com.h"
#ifdef HCCHIP_GCC
#include "hcapi/com_api.h"
#endif
#include "application/ui/SettingScreen.h"

namespace Setting {

Position::Position(/* args */)
{
    boost::property_tree::ptree config;
    ReadConfigFile(config, "position");
    position_mode = config.get<int>("position_mode", 0);
}

Position::~Position()
{
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(ConfigFileName, pt);
    boost::property_tree::ptree config;
    config = pt.get_child("position");
    config.put<int>("position_mode", position_mode);
    pt.put_child("position",config);
    boost::property_tree::ini_parser::write_ini(ConfigFileName, pt);
}

const char** Position::GetStrArray(void)
{
    static const char* name[6] = {
    "setting_p_position_setting", 
    "setting_p_position_0", 
    "setting_p_position_1", 
    "setting_p_position_2", 
    "setting_p_position_3" 
    };
    return name;
}

int Position::GetUserValue(int index)
{
    return 0;
}

void Position::SelectedValue(int index)
{
    switch (index)
    {
    case static_cast<int>(Setting_Position0):
    {
        position_mode = 0;
        /* 调用翻转函数 */
    }
    break;

    case static_cast<int>(Setting_Position1):
    {
        position_mode = 1;
        /* 调用翻转函数 */
    }
    break;

    case static_cast<int>(Setting_Position2):
    {
        position_mode = 2;
        /* 调用翻转函数 */
    }
    break;

    case static_cast<int>(Setting_Position3):
    {
        position_mode = 3;
        /* 调用翻转函数 */
    }
    break;

    default:
        break;
    }
}

void Position::IncreaseUserValue(int index)
{
}

void Position::DecreaseUserValue(int index)
{
}

const char* Position::GetStr(int index)
{
    const char* name = nullptr;
    return name;
}

void* Position::GetDerivedAddress(int index)
{
    return nullptr;
}

}
