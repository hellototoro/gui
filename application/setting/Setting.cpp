/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-07-31 17:25:46
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-08-02 08:12:54
 * @FilePath: /gui/application/ui/setting/Setting.cpp
 * @Description: None
 * @other: None
 */
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include "Setting.h"

namespace Setting {

void PictureSettingInit(Picture* setting)
{
    boost::property_tree::ptree pt, commonCfg;
    #ifdef HCCHIP_GCC
    boost::property_tree::ini_parser::read_ini("/tmp/data/gui_setting.ini", pt);
    #else
    boost::property_tree::ini_parser::read_ini("/home/totoro/workspace/HiChip/hclinux/SOURCE/gui/application/ui/setting/gui_setting.ini", pt);
    #endif

    commonCfg = pt.get_child("picture_setting");
    setting->mode.type = commonCfg.get<int>("mode_type", 0);
    setting->mode.user.contrast = commonCfg.get<int>("mode_user_contrast", 50);
    setting->mode.user.brightness = commonCfg.get<int>("mode_user_brightness", 50);
    setting->mode.user.colour = commonCfg.get<int>("mode_user_colour", 50);
    setting->mode.user.sharpness = commonCfg.get<int>("mode_user_sharpness", 50);
    setting->scale = commonCfg.get<int>("scale", 0);
    setting->ColorTemperature.type = commonCfg.get<int>("ColorTemperature_type", 0);
    setting->ColorTemperature.user.red = commonCfg.get<int>("ColorTemperature_user_red", 50);
    setting->ColorTemperature.user.green = commonCfg.get<int>("ColorTemperature_user_green", 50);
    setting->ColorTemperature.user.blue = commonCfg.get<int>("ColorTemperature_user_blue", 50);
    setting->ratio = commonCfg.get<int>("ratio", 0);
    setting->PowerBankMode = commonCfg.get<int>("PowerBankMode", 0);
}

Picture* CreatePictureSettingObj(void)
{
    Picture* PictureData = new Picture;
    PictureSettingInit(PictureData);
    return PictureData;
}

void SoundSettingInit(Sound* setting)
{
    boost::property_tree::ptree pt, commonCfg;
    #ifdef HCCHIP_GCC
    boost::property_tree::ini_parser::read_ini("/tmp/data/gui_setting.ini", pt);
    #else
    boost::property_tree::ini_parser::read_ini("/home/totoro/workspace/HiChip/hclinux/SOURCE/gui/application/ui/setting/gui_setting.ini", pt);
    #endif

    commonCfg = pt.get_child("sound_setting");
    setting->mode.type = commonCfg.get<int>("mode_type", 0);
    setting->mode.user.treble = commonCfg.get<int>("mode_user_treble", 50);
    setting->mode.user.bass = commonCfg.get<int>("mode_user_bass", 50);
    setting->surround = commonCfg.get<int>("surround", 0);
    setting->AutoVolume = commonCfg.get<int>("AutoVolume", 0);
}

Sound* CreateSoundSettingObj(void)
{
    Sound* SoundData = new Sound;
    SoundSettingInit(SoundData);
    return SoundData;
}

void SystemSettingInit(System* setting)
{
    boost::property_tree::ptree pt, commonCfg;
    #ifdef HCCHIP_GCC
    boost::property_tree::ini_parser::read_ini("/tmp/data/gui_setting.ini", pt);
    #else
    boost::property_tree::ini_parser::read_ini("/home/totoro/workspace/HiChip/hclinux/SOURCE/gui/application/ui/setting/gui_setting.ini", pt);
    #endif

    commonCfg = pt.get_child("system_setting");
    setting->language = commonCfg.get<int>("language", 0);
    setting->OsdTime = commonCfg.get<int>("OsdTime", 0);
}

System* CreateSystemSettingObj(void)
{
    System* SystemData = new System;
    SystemSettingInit(SystemData);
    return SystemData;
}

template <typename T>
void DeleteSettingObj(T* SettingData)
{
    delete SettingData;
}

}
