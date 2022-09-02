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
#include "application/ConfigParam.h"
#include "application/windows.h"
#include "application/ui/LanguageScreen.h"

namespace Setting {


void ReadPictureSettingParam(Picture& setting)
{
    boost::property_tree::ptree config;// = pt->get_child("picture_setting");
    ReadConfigFile(config, "picture_setting");
    setting.mode.type = config.get<int>("mode_type", 0);
    setting.mode.user.contrast = config.get<int>("mode_user_contrast", 50);
    setting.mode.user.brightness = config.get<int>("mode_user_brightness", 50);
    setting.mode.user.colour = config.get<int>("mode_user_colour", 50);
    setting.mode.user.sharpness = config.get<int>("mode_user_sharpness", 50);
    setting.scale = config.get<int>("scale", 0);
    setting.ColorTemperature.type = config.get<int>("ColorTemperature_type", 0);
    setting.ColorTemperature.user.red = config.get<int>("ColorTemperature_user_red", 50);
    setting.ColorTemperature.user.green = config.get<int>("ColorTemperature_user_green", 50);
    setting.ColorTemperature.user.blue = config.get<int>("ColorTemperature_user_blue", 50);
    setting.ratio = config.get<int>("ratio", 0);
    setting.PowerBankMode = config.get<int>("PowerBankMode", 0);
    //delete pt;
}

void ReadSoundSettingParam(Sound& setting)
{
    boost::property_tree::ptree config;
    ReadConfigFile(config, "sound_setting");
    setting.mode.type = config.get<int>("mode_type", 0);
    setting.mode.user.treble = config.get<int>("mode_user_treble", 50);
    setting.mode.user.bass = config.get<int>("mode_user_bass", 50);
    setting.surround = config.get<int>("surround", 0);
    setting.AutoVolume = config.get<int>("AutoVolume", 0);
    //delete pt;
}

void ReadSystemSettingParam(System& setting)
{
    boost::property_tree::ptree config;
    ReadConfigFile(config, "system_setting");
    setting.language = DefaultLanguageIndex;
    setting.OsdTime = config.get<int>("OsdTime", 0);
    //delete pt;
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
