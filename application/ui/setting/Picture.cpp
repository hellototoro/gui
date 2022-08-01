/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-07-28 20:27:47
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-08-01 12:18:16
 * @FilePath: /gui/application/ui/setting/Picture.cpp
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
    static const char* name[] = {
        "图像设置",
        "图像模式", "标准", 
        "缩放设置", "16:9", 
        "色温", "标准", 
        "投影缩放", "100%", 
        "充电宝亮度模式", "低"
    };
    return name;
}

int Picture::GetValue(int index)
{
    return 0;
}

void Picture::IncreaseValue(int index)
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

void Picture::DecreaseValue(int index)
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

void PictureSettingInit(Picture* setting)
{
    setting->mode.type = static_cast<uint8_t>(setting->mode.PictureMode_User);
    setting->mode.user.contrast = 50;
    setting->mode.user.brightness = 60;
    setting->mode.user.colour = 70;
    setting->mode.user.sharpness = 80;

    setting->scale = 0;
    
    setting->ColorTemperature.type = static_cast<uint8_t>(setting->ColorTemperature.PictureColorTemperature_User);
    setting->ColorTemperature.user.red = 50;
    setting->ColorTemperature.user.green = 60;
    setting->ColorTemperature.user.blue = 70;

    
    setting->ratio = 100;

    setting->PowerBankMode = 0;
}

Picture* CreatePictureSettingObj(void)
{
    Picture* PictureData = new Picture;
    PictureSettingInit(PictureData);
    return PictureData;
}

void DeletePictureSettingObj(Picture* PictureData)
{
    delete PictureData;
}

const char** Picture::PictureMode::GetStrArray(void)
{
    //const char** name = nullptr;
    static const char* name[] = { "图像设置", "对比度", "亮度", "色彩",  "锐度" };
    return name;
}

int Picture::PictureMode::GetValue(int index)
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

void Picture::PictureMode::IncreaseValue(int index)
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

void Picture::PictureMode::DecreaseValue(int index)
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
    static const char* name[] = { "色温", "红", "绿", "蓝", };
    return name;
}

int Picture::PictureColorTemperature::GetValue(int index)
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

void Picture::PictureColorTemperature::IncreaseValue(int index)
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

void Picture::PictureColorTemperature::DecreaseValue(int index)
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
