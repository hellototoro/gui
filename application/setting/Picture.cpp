/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-07-28 20:27:47
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-08-08 18:43:43
 * @FilePath: /gui/application/setting/Picture.cpp
 * @Description: None
 * @other: None
 */
#include <stdio.h>
#include "Picture.h"


namespace Setting {

Picture::Picture(/* args */)
{
}

Picture::~Picture()
{
}

const char** Picture::GetStrArray(void)
{
    static const char* name[11] = {
        _("setting_p_picture_setting"), 
        _("setting_p_picture_mode"), _("setting_p_picture_mode_type_standard"), 
        _("setting_p_picture_scale"), "16:9", 
        _("setting_p_picture_color_temp"), _("setting_p_picture_color_temp_type_standard"), 
        _("setting_p_picture_rate"), _("setting_p_picture_rate_100"), 
        _("setting_p_picture_power_bank_mode"), _("setting_p_picture_power_bank_mode_low")
    };
    return name;
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
    const char* name = nullptr;
    switch (index)
    {
    case static_cast<int>(Setting_PictureMode):
        name = mode.name[mode.type];
        break;

    case static_cast<int>(Setting_PictureScale):
        name = ScaleName[scale];
        break;

    case static_cast<int>(Setting_PictureColorTemperature):
        name = ColorTemperature.name[ColorTemperature.type];
        break;

    case static_cast<int>(Setting_PictureRatio):
        sprintf(RatioName, "%d%%", ratio);
        name = RatioName;
        break;

    case static_cast<int>(Setting_PicturePowerBank):
        name = PowerBankModeName[PowerBankMode];
        break;

    default:
        break;
    }
    return name;
}

void* Picture::GetDerivedAddress(int index)
{
    switch (index)
    {
    case static_cast<int>(Setting_PictureMode):
        return &mode;

    case static_cast<int>(Setting_PictureColorTemperature):
        return &ColorTemperature;

    default:
        return nullptr;
    }
}

const char** Picture::PictureMode::GetStrArray(void)
{
    //const char** name = nullptr;
    static const char* name[5] = { 
    _("setting_p_picture_mode"), 
    _("setting_p_picture_mode_contrast"), 
    _("setting_p_picture_mode_brightness"), 
    _("setting_p_picture_mode_colour"), 
    _("setting_p_picture_mode_sharpness") };
    return name;
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
    const char* name = nullptr;
    switch (index)
    {
    case static_cast<int>(ModeType::Content_Contrast):
        sprintf(user.ContrastName, "%d", user.contrast);
        name = user.ContrastName;
        break;

    case static_cast<int>(ModeType::Content_Brightness):
        sprintf(user.BrightnessName, "%d", user.brightness);
        name = user.BrightnessName;
        break;

    case static_cast<int>(ModeType::Content_Colour):
        sprintf(user.ColourName, "%d", user.colour);
        name = user.ColourName;
        break;

    case static_cast<int>(ModeType::Content_Sharpness):
        sprintf(user.SharpnessName, "%d", user.sharpness);
        name = user.SharpnessName;
        break;
    
    default:
        break;
    }
    return name;
}

const char** Picture::PictureColorTemperature::GetStrArray(void)
{
    static const char* name[4] = { 
    _("setting_p_picture_color_temp"), 
    _("setting_p_picture_color_temp_red"), 
    _("setting_p_picture_color_temp_green"), 
    _("setting_p_picture_color_temp_blue") };
    return name;
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
    const char* name = nullptr;
    switch (index)
    {
    case static_cast<int>(ColorTemperatureType::Content_Red):
        sprintf(user.RedName, "%d", user.red);
        name = user.RedName;
        break;

    case static_cast<int>(ColorTemperatureType::Content_Green):
        sprintf(user.GreenName, "%d", user.green);
        name = user.GreenName;
        break;

    case static_cast<int>(ColorTemperatureType::Content_Blue):
        sprintf(user.BlueName, "%d", user.blue);
        name = user.BlueName;
        break;

    default:
        break;
    }
    return name;
}

}
