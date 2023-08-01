/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-07-28 20:27:47
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-09-02 22:47:21
 * @FilePath: /gui/application/setting/Picture.cpp
 * @Description: None
 * @other: None
 */
#include <stdio.h>
#include "Picture.h"
#include "SysParam/SysParam.h"

namespace Setting {

Picture::Picture(/* args */)
{
    SysParam sys_param;
    mode.type = sys_param.read<int>("picture_setting", "mode_type");
    mode.user.contrast = sys_param.read<int>("picture_setting", "mode_user_contrast");
    mode.user.brightness = sys_param.read<int>("picture_setting", "mode_user_brightness");
    mode.user.colour = sys_param.read<int>("picture_setting", "mode_user_colour");
    mode.user.sharpness = sys_param.read<int>("picture_setting", "mode_user_sharpness");
    scale = sys_param.read<int>("picture_setting", "scale");
    ColorTemperature.type = sys_param.read<int>("picture_setting", "ColorTemperature_type");
    ColorTemperature.user.red = sys_param.read<int>("picture_setting", "ColorTemperature_user_red");
    ColorTemperature.user.green = sys_param.read<int>("picture_setting", "ColorTemperature_user_green");
    ColorTemperature.user.blue = sys_param.read<int>("picture_setting", "ColorTemperature_user_blue");
    ratio = sys_param.read<int>("picture_setting", "ratio");
    PowerBankMode = sys_param.read<int>("picture_setting", "PowerBankMode");
}

Picture::~Picture()
{
    SysParam sys_param;
    sys_param.write<int>("picture_setting", "mode_type", mode.type);
    sys_param.write<int>("picture_setting", "mode_user_contrast", mode.user.contrast);
    sys_param.write<int>("picture_setting", "mode_user_brightness", mode.user.brightness);
    sys_param.write<int>("picture_setting", "mode_user_colour", mode.user.colour);
    sys_param.write<int>("picture_setting", "mode_user_sharpness", mode.user.sharpness);
    sys_param.write<int>("picture_setting", "scale", scale);
    sys_param.write<int>("picture_setting", "ColorTemperature_type", ColorTemperature.type);
    sys_param.write<int>("picture_setting", "ColorTemperature_user_red", ColorTemperature.user.red);
    sys_param.write<int>("picture_setting", "ColorTemperature_user_green", ColorTemperature.user.green);
    sys_param.write<int>("picture_setting", "ColorTemperature_user_blue", ColorTemperature.user.blue);
    sys_param.write<int>("picture_setting", "ratio", ratio);
    sys_param.write<int>("picture_setting", "PowerBankMode", PowerBankMode);
}

const char** Picture::GetStrArray(void)
{
    static const char* name_list[11] = {
        "setting_p_picture_setting", 
        "setting_p_picture_mode", "", 
        "setting_p_picture_scale", "", 
        "setting_p_picture_color_temp", "", 
        "setting_p_picture_rate", "setting_p_picture_rate_100", 
        "setting_p_picture_power_bank_mode", ""
    };
    name_list[2] = mode.name[mode.type];
    name_list[4] = ScaleName[scale];
    name_list[6] = ColorTemperature.name[ColorTemperature.type];
    //name_list[8] = ScaleName[scale];
    name_list[10] = PowerBankModeName[PowerBankMode];
    return name_list;
}

int Picture::GetUserValue(int index)
{
    return 0;
}

void Picture::IncreaseUserValue(int index)
{
    switch (index)
    {
    case static_cast<int>(Setting_PictureMode):
        IncreaseValueComm(mode.type, 0, static_cast<int>(mode.PictureMode_Num));
        break;

    case static_cast<int>(Setting_PictureScale):
        IncreaseValueComm(scale, 0, sizeof(ScaleName)/sizeof(ScaleName[0]));
        break;

    case static_cast<int>(Setting_PictureColorTemperature):
        IncreaseValueComm(ColorTemperature.type, 0, static_cast<int>(ColorTemperature.PictureColorTemperature_Num));
        break;
    
    case static_cast<int>(Setting_PictureRatio):
        IncreaseValueComm(ratio, 75, 100 + 1);
        break;
    
    case static_cast<int>(Setting_PicturePowerBank):
        IncreaseValueComm(PowerBankMode, 0, sizeof(PowerBankModeName)/sizeof(PowerBankModeName[0]));
        break;
    
    default:
        break;
    }
}

void Picture::DecreaseUserValue(int index)
{
    switch (index)
    {
    case static_cast<int>(Setting_PictureMode):
        DecreaseValueComm(mode.type, 0, static_cast<int>(mode.PictureMode_Num));
        break;

    case static_cast<int>(Setting_PictureScale):
        DecreaseValueComm(scale, 0, sizeof(ScaleName)/sizeof(ScaleName[0]));
        break;

    case static_cast<int>(Setting_PictureColorTemperature):
        DecreaseValueComm(ColorTemperature.type, 0, static_cast<int>(ColorTemperature.PictureColorTemperature_Num));
        break;
        
    case static_cast<int>(Setting_PictureRatio):
        DecreaseValueComm(ratio, 75, 100 + 1);
        break;
        
    case static_cast<int>(Setting_PicturePowerBank):
        DecreaseValueComm(PowerBankMode, 0, sizeof(PowerBankModeName)/sizeof(PowerBankModeName[0]));
        break;
        
    default:
        break;
    }
}

const char* Picture::GetStr(int index)
{
    const char* name_list = nullptr;
    switch (index)
    {
    case static_cast<int>(Setting_PictureMode):
        name_list = mode.name[mode.type];
        break;

    case static_cast<int>(Setting_PictureScale):
        name_list = ScaleName[scale];
        break;

    case static_cast<int>(Setting_PictureColorTemperature):
        name_list = ColorTemperature.name[ColorTemperature.type];
        break;

    case static_cast<int>(Setting_PictureRatio):
        sprintf(RatioName, "%d%%", ratio);
        name_list = RatioName;
        break;

    case static_cast<int>(Setting_PicturePowerBank):
        name_list = PowerBankModeName[PowerBankMode];
        break;

    default:
        break;
    }
    return name_list;
}

void* Picture::GetDerivedAddress(int index)
{
    switch (index)
    {
    case static_cast<int>(Setting_PictureMode):
        if(mode.type == static_cast<int>(PictureMode::PictureMode_User))
            return mode.GetDerivedAddress(0);
        break;

    case static_cast<int>(Setting_PictureColorTemperature):
        if(ColorTemperature.type == static_cast<int>(PictureColorTemperature::PictureColorTemperature_User))
            return ColorTemperature.GetDerivedAddress(0);
        break;

    default:
        break;
    }
    return nullptr;
}

const char** Picture::PictureMode::GetStrArray(void)
{
    //const char** name = nullptr;
    static const char* name_list[5] = { 
    "setting_p_picture_mode", 
    "setting_p_picture_mode_contrast", 
    "setting_p_picture_mode_brightness", 
    "setting_p_picture_mode_colour", 
    "setting_p_picture_mode_sharpness" };
    return name_list;
}

int Picture::PictureMode::GetUserValue(int index)
{
    switch (index)
    {
    case static_cast<int>(ModeType::Content_Contrast):
        return user.contrast;

    case static_cast<int>(ModeType::Content_Brightness):
        return user.brightness;

    case static_cast<int>(ModeType::Content_Colour):
        return user.colour;

    case static_cast<int>(ModeType::Content_Sharpness):
        return user.sharpness;
    
    default:
        break;
    }
    return 0;
}

void Picture::PictureMode::IncreaseUserValue(int index)
{
    switch (index)
    {
    case static_cast<int>(ModeType::Content_Contrast):
        IncreaseValueComm(user.contrast, 0, 100 + 1);
        break;

    case static_cast<int>(ModeType::Content_Brightness):
        IncreaseValueComm(user.brightness, 0, 100 + 1);
        break;

    case static_cast<int>(ModeType::Content_Colour):
        IncreaseValueComm(user.colour, 0, 100 + 1);
        break;

    case static_cast<int>(ModeType::Content_Sharpness):
        IncreaseValueComm(user.sharpness, 0, 100 + 1);
        break;
    
    default:
        break;
    }
}

void Picture::PictureMode::DecreaseUserValue(int index)
{
    switch (index)
    {
    case static_cast<int>(ModeType::Content_Contrast):
        DecreaseValueComm(user.contrast, 0, 100 + 1);
        break;

    case static_cast<int>(ModeType::Content_Brightness):
        DecreaseValueComm(user.brightness, 0, 100 + 1);
        break;

    case static_cast<int>(ModeType::Content_Colour):
        DecreaseValueComm(user.colour, 0, 100 + 1);
        break;

    case static_cast<int>(ModeType::Content_Sharpness):
        DecreaseValueComm(user.sharpness, 0, 100 + 1);
        break;
    
    default:
        break;
    }
}

const char* Picture::PictureMode::GetStr(int index)
{
    const char* name_list = nullptr;
    switch (index)
    {
    case static_cast<int>(ModeType::Content_Contrast):
        sprintf(user.ContrastName, "%d", user.contrast);
        name_list = user.ContrastName;
        break;

    case static_cast<int>(ModeType::Content_Brightness):
        sprintf(user.BrightnessName, "%d", user.brightness);
        name_list = user.BrightnessName;
        break;

    case static_cast<int>(ModeType::Content_Colour):
        sprintf(user.ColourName, "%d", user.colour);
        name_list = user.ColourName;
        break;

    case static_cast<int>(ModeType::Content_Sharpness):
        sprintf(user.SharpnessName, "%d", user.sharpness);
        name_list = user.SharpnessName;
        break;
    
    default:
        break;
    }
    return name_list;
}

const char** Picture::PictureColorTemperature::GetStrArray(void)
{
    static const char* name_list[4] = { 
    "setting_p_picture_color_temp", 
    "setting_p_picture_color_temp_red", 
    "setting_p_picture_color_temp_green", 
    "setting_p_picture_color_temp_blue" };
    return name_list;
}

int Picture::PictureColorTemperature::GetUserValue(int index)
{
    switch (index)
    {
    case static_cast<int>(ColorTemperatureType::Content_Red):
        return user.red;

    case static_cast<int>(ColorTemperatureType::Content_Green):
        return user.green;

    case static_cast<int>(ColorTemperatureType::Content_Blue):
        return user.blue;

    default:
        break;
    }
    return 0;
}

void Picture::PictureColorTemperature::IncreaseUserValue(int index)
{
    switch (index)
    {
    case static_cast<int>(ColorTemperatureType::Content_Red):
        IncreaseValueComm(user.red, 0, 100 + 1);
        break;

    case static_cast<int>(ColorTemperatureType::Content_Green):
        IncreaseValueComm(user.green, 0, 100 + 1);
        break;

    case static_cast<int>(ColorTemperatureType::Content_Blue):
        IncreaseValueComm(user.blue, 0, 100 + 1);
        break;

    default:
        break;
    }
}

void Picture::PictureColorTemperature::DecreaseUserValue(int index)
{
    switch (index)
    {
    case static_cast<int>(ColorTemperatureType::Content_Red):
        DecreaseValueComm(user.red, 0, 100 + 1);
        break;

    case static_cast<int>(ColorTemperatureType::Content_Green):
        DecreaseValueComm(user.green, 0, 100 + 1);
        break;

    case static_cast<int>(ColorTemperatureType::Content_Blue):
        DecreaseValueComm(user.blue, 0, 100 + 1);
        break;

    default:
        break;
    }
}

const char* Picture::PictureColorTemperature::GetStr(int index)
{
    const char* name_list = nullptr;
    switch (index)
    {
    case static_cast<int>(ColorTemperatureType::Content_Red):
        sprintf(user.RedName, "%d", user.red);
        name_list = user.RedName;
        break;

    case static_cast<int>(ColorTemperatureType::Content_Green):
        sprintf(user.GreenName, "%d", user.green);
        name_list = user.GreenName;
        break;

    case static_cast<int>(ColorTemperatureType::Content_Blue):
        sprintf(user.BlueName, "%d", user.blue);
        name_list = user.BlueName;
        break;

    default:
        break;
    }
    return name_list;
}

}
