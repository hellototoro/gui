/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-07-31 17:25:46
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-08-02 09:40:41
 * @FilePath: /gui/application/setting/Setting.cpp
 * @Description: None
 * @other: None
 */
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include "Setting.h"

namespace Setting {

boost::property_tree::ptree* ReadConfigFile(void)
{
    boost::property_tree::ptree* pt = new boost::property_tree::ptree;
    #ifdef HCCHIP_GCC
    boost::property_tree::ini_parser::read_ini("/tmp/data/gui_setting.ini", *pt);
    #else
    boost::property_tree::ini_parser::read_ini("/home/totoro/workspace/HiChip/hclinux/SOURCE/gui/application/setting/gui_setting.ini", *pt);
    #endif
    return pt;
}

void PictureSettingInit(Picture* setting)
{
    boost::property_tree::ptree* pt = ReadConfigFile();
    boost::property_tree::ptree config = pt->get_child("picture_setting");
    setting->mode.type = config.get<int>("mode_type", 0);
    setting->mode.user.contrast = config.get<int>("mode_user_contrast", 50);
    setting->mode.user.brightness = config.get<int>("mode_user_brightness", 50);
    setting->mode.user.colour = config.get<int>("mode_user_colour", 50);
    setting->mode.user.sharpness = config.get<int>("mode_user_sharpness", 50);
    setting->scale = config.get<int>("scale", 0);
    setting->ColorTemperature.type = config.get<int>("ColorTemperature_type", 0);
    setting->ColorTemperature.user.red = config.get<int>("ColorTemperature_user_red", 50);
    setting->ColorTemperature.user.green = config.get<int>("ColorTemperature_user_green", 50);
    setting->ColorTemperature.user.blue = config.get<int>("ColorTemperature_user_blue", 50);
    setting->ratio = config.get<int>("ratio", 0);
    setting->PowerBankMode = config.get<int>("PowerBankMode", 0);
    delete pt;
}

void SoundSettingInit(Sound* setting)
{
    boost::property_tree::ptree* pt = ReadConfigFile();
    boost::property_tree::ptree config = pt->get_child("sound_setting");
    setting->mode.type = config.get<int>("mode_type", 0);
    setting->mode.user.treble = config.get<int>("mode_user_treble", 50);
    setting->mode.user.bass = config.get<int>("mode_user_bass", 50);
    setting->surround = config.get<int>("surround", 0);
    setting->AutoVolume = config.get<int>("AutoVolume", 0);
    delete pt;
}

void SystemSettingInit(System* setting)
{
    boost::property_tree::ptree* pt = ReadConfigFile();
    boost::property_tree::ptree config = pt->get_child("system_setting");
    setting->language = config.get<int>("language", 0);
    setting->OsdTime = config.get<int>("OsdTime", 0);
    delete pt;
}

}
