/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-07-31 17:25:46
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-09-02 01:57:59
 * @FilePath: /gui/application/setting/Setting.cpp
 * @Description: None
 * @other: None
 */
#include <string>
#include "Setting.h"
#include "SysParam/SysParam.h"
#include "application/windows.h"
#include "application/ui/LanguageScreen.h"

namespace Setting {


void ReadPictureSettingParam(Picture& setting)
{
    SysParam sys_param;
    setting.mode.type = sys_param.read<int>("picture_setting", "mode_type");
    setting.mode.user.contrast = sys_param.read<int>("picture_setting", "mode_user_contrast");
    setting.mode.user.brightness = sys_param.read<int>("picture_setting", "mode_user_brightness");
    setting.mode.user.colour = sys_param.read<int>("picture_setting", "mode_user_colour");
    setting.mode.user.sharpness = sys_param.read<int>("picture_setting", "mode_user_sharpness");
    setting.scale = sys_param.read<int>("picture_setting", "scale");
    setting.ColorTemperature.type = sys_param.read<int>("picture_setting", "ColorTemperature_type");
    setting.ColorTemperature.user.red = sys_param.read<int>("picture_setting", "ColorTemperature_user_red");
    setting.ColorTemperature.user.green = sys_param.read<int>("picture_setting", "ColorTemperature_user_green");
    setting.ColorTemperature.user.blue = sys_param.read<int>("picture_setting", "ColorTemperature_user_blue");
    setting.ratio = sys_param.read<int>("picture_setting", "ratio");
    setting.PowerBankMode = sys_param.read<int>("picture_setting", "PowerBankMode");
}

void ReadSoundSettingParam(Sound& setting)
{
    SysParam sys_param;
    setting.mode.type = sys_param.read<int>("sound_setting", "mode_type");
    setting.mode.user.treble = sys_param.read<int>("sound_setting", "mode_user_treble");
    setting.mode.user.bass = sys_param.read<int>("sound_setting", "mode_user_bass");
    setting.surround = sys_param.read<int>("sound_setting", "surround");
    setting.AutoVolume = sys_param.read<int>("sound_setting", "AutoVolume");
}

void ReadSystemSettingParam(System& setting)
{
    SysParam sys_param;
    setting.language = DefaultLanguageIndex;
    setting.OsdTime = sys_param.read<int>("system_setting", "OsdTime");
}

template <typename T>
T* CreateSettingObj( void (*ReadSettingParam)(T&) )
{
    T* SettingData = new T;
    ReadSettingParam(*SettingData);
    return SettingData;
}
//显式声明模板函数
template Picture* CreateSettingObj<Picture>(void (*ReadSettingParam)(Picture&));
template Sound* CreateSettingObj<Sound>(void (*ReadSettingParam)(Sound&));
template System* CreateSettingObj<System>(void (*ReadSettingParam)(System&));

template <typename T>
void DeleteSettingObj(T* SettingData, void (*WriteSettingParam)(T&) )
{
    WriteSettingParam(*SettingData);
    delete SettingData;
}
//显式声明模板函数
template void DeleteSettingObj<Picture>(Picture*, void (*WriteSettingParam)(Picture&) );
template void DeleteSettingObj<Sound>(Sound*, void (*WriteSettingParam)(Sound&) );
template void DeleteSettingObj<System>(System*, void (*WriteSettingParam)(System&) );

void LoadSettingParameter(void)
{

    /*************************picture*****************************/
    Picture picture;
    ReadPictureSettingParam(picture);
    switch (picture.mode.type)
    {
    case static_cast<int>(Picture::PictureMode::PictureMode_Standard):
        //SetContrast(picture.mode.standard.contrast);
        //SetBrightness(picture.mode.standard.brightness);
        //SetColour(picture.mode.standard.colour);
        //SetSharpness(picture.mode.standard.sharpness);
        break;
    case static_cast<int>(Picture::PictureMode::PictureMode_Soft):
        //SetContrast(picture.mode.soft.contrast);
        //SetBrightness(picture.mode.soft.brightness);
        //SetColour(picture.mode.soft.colour);
        //SetSharpness(picture.mode.soft.sharpness);
        break;
    case static_cast<int>(Picture::PictureMode::PictureMode_User):
        //SetContrast(picture.mode.user.contrast);
        //SetBrightness(picture.mode.user.brightness);
        //SetColour(picture.mode.user.colour);
        //SetSharpness(picture.mode.user.sharpness);
        break;
    case static_cast<int>(Picture::PictureMode::PictureMode_Dynamic):
        //SetContrast(picture.mode.dynamic.contrast);
        //SetBrightness(picture.mode.dynamic.brightness);
        //SetColour(picture.mode.dynamic.colour);
        //SetSharpness(picture.mode.dynamic.sharpness);
        break;
    default:
        break;
    }

    switch (picture.ColorTemperature.type)
    {
    case static_cast<int>(Picture::PictureColorTemperature::PictureColorTemperature_Standard):
        //SetColorTemperatureRed(picture.ColorTemperature.standard.red);
        //SetColorTemperatureGreen(picture.ColorTemperature.standard.green);
        //SetColorTemperatureBlue(picture.ColorTemperature.standard.blue);
        break;
    case static_cast<int>(Picture::PictureColorTemperature::PictureColorTemperature_Warm):
        //SetColorTemperatureRed(picture.ColorTemperature.warm.red);
        //SetColorTemperatureGreen(picture.ColorTemperature.warm.green);
        //SetColorTemperatureBlue(picture.ColorTemperature.warm.blue);
        break;
    case static_cast<int>(Picture::PictureColorTemperature::PictureColorTemperature_User):
        //SetColorTemperatureRed(picture.ColorTemperature.standard.red);
        //SetColorTemperatureGreen(picture.ColorTemperature.standard.green);
        //SetColorTemperatureBlue(picture.ColorTemperature.standard.blue);
        break;
    case static_cast<int>(Picture::PictureColorTemperature::PictureColorTemperature_Cool):
        //SetColorTemperatureRed(picture.ColorTemperature.cool.red);
        //SetColorTemperatureGreen(picture.ColorTemperature.cool.green);
        //SetColorTemperatureBlue(picture.ColorTemperature.cool.blue);
        break;
    default:
        break;
    }
    //SetScale(picture.scale);
    //SetRatio(picture.ratio);
    //SetPowerBankMode(picture.PowerBankMode);

    /*************************sound*****************************/
    Sound sound;
    ReadSoundSettingParam(sound);
    
    /*************************system*****************************/
    System system;
    ReadSystemSettingParam(system);
    
}

}
