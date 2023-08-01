/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2023-01-18 15:26:31
 * @FilePath: /gui/application/setting/Position.cpp
 * @Description: None
 * @other: None
 */
#include <stdio.h>
#include "Position.h"
#include "SysParam/SysParam.h"
#include "application/ui/ui_com.h"
#include "application/ui/SettingScreen.h"

namespace Setting {

Position::Position(/* args */)
{
    SysParam sys_param;
    position_mode = sys_param.read<int>("position", "position_mode");
}

Position::~Position()
{
    SysParam sys_param;
    sys_param.write<int>("position", "position_mode", position_mode);
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
