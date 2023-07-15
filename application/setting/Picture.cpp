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
#include "application/ConfigParam.h"

namespace Setting {

Picture::Picture(/* args */)
{
    // boost::property_tree::ptree config;// = pt->get_child("picture_setting");
    // ReadConfigFile(config, "picture_setting");
    // mode.type = config.get<int>("mode_type", 0);
    // mode.user.contrast = config.get<int>("mode_user_contrast", 50);
    // mode.user.brightness = config.get<int>("mode_user_brightness", 50);
    // mode.user.colour = config.get<int>("mode_user_colour", 50);
    // mode.user.sharpness = config.get<int>("mode_user_sharpness", 50);
    // scale = config.get<int>("scale", 0);
    // ColorTemperature.type = config.get<int>("ColorTemperature_type", 0);
    // ColorTemperature.user.red = config.get<int>("ColorTemperature_user_red", 50);
    // ColorTemperature.user.green = config.get<int>("ColorTemperature_user_green", 50);
    // ColorTemperature.user.blue = config.get<int>("ColorTemperature_user_blue", 50);
    // ratio = config.get<int>("ratio", 0);
    // PowerBankMode = config.get<int>("PowerBankMode", 0);
}

Picture::~Picture()
{
    // boost::property_tree::ptree pt;
    // boost::property_tree::ini_parser::read_ini(ConfigFileName, pt);
    // boost::property_tree::ptree config;
    // config = pt.get_child("picture_setting");
    // config.put<int>("mode_type", mode.type);
    // config.put<int>("mode_user_contrast", mode.user.contrast);
    // config.put<int>("mode_user_brightness", mode.user.brightness);
    // config.put<int>("mode_user_colour", mode.user.colour);
    // config.put<int>("mode_user_sharpness", mode.user.sharpness);
    // config.put<int>("scale", scale);
    // config.put<int>("ColorTemperature_type", ColorTemperature.type);
    // config.put<int>("ColorTemperature_user_red", ColorTemperature.user.red);
    // config.put<int>("ColorTemperature_user_green", ColorTemperature.user.green);
    // config.put<int>("ColorTemperature_user_blue", ColorTemperature.user.blue);
    // config.put<int>("ratio", ratio);
    // config.put<int>("PowerBankMode", PowerBankMode);
    // pt.put_child("picture_setting",config);
    // boost::property_tree::ini_parser::write_ini(ConfigFileName, pt);
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
