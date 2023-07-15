/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-08-01 13:38:33
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-09-02 22:44:35
 * @FilePath: /gui/application/setting/Sound.cpp
 * @Description: None
 * @other: None
 */
#include <stdio.h>
#include "Sound.h"
#include "application/ConfigParam.h"

namespace Setting {

Sound::Sound(/* args */)
{
    // boost::property_tree::ptree config;
    // ReadConfigFile(config, "sound_setting");
    // mode.type = config.get<int>("mode_type", 0);
    // mode.user.treble = config.get<int>("mode_user_treble", 50);
    // mode.user.bass = config.get<int>("mode_user_bass", 50);
    // surround = config.get<int>("surround", 0);
    // AutoVolume = config.get<int>("AutoVolume", 0);
}

Sound::~Sound()
{
    // boost::property_tree::ptree pt;
    // boost::property_tree::ini_parser::read_ini(ConfigFileName, pt);
    // boost::property_tree::ptree config;
    // config = pt.get_child("sound_setting");

    // config.put<int>("mode_type", mode.type);
    // config.put<int>("mode_user_treble", mode.user.treble);
    // config.put<int>("mode_user_bass", mode.user.bass);
    // config.put<int>("surround", surround);
    // config.put<int>("AutoVolume", AutoVolume);
    // pt.put_child("sound_setting",config);
    // boost::property_tree::ini_parser::write_ini(ConfigFileName, pt);
}

const char** Sound::GetStrArray(void)
{
    static const char* name[7] = {
    "setting_p_sound_setting",
    "setting_p_sound_mode", "", 
    "setting_p_sound_surround", "", 
    "setting_p_sound_auto_volume", "" };
    name[2] = mode.name[mode.type];
    name[4] = SurroundName[surround];
    name[6] = AutoVolumeName[AutoVolume];
    return name;
}

int Sound::GetUserValue(int index)
{
    return 0;
}

void Sound::IncreaseUserValue(int index)
{
    switch (index)
    {
    case static_cast<int>(Setting_SoundMode):
        IncreaseValueComm(mode.type, 0, static_cast<int>(mode.SoundMode_Num));
        break;

    case static_cast<int>(Setting_SoundSurround):
        IncreaseValueComm(surround, 0, sizeof(SurroundName)/sizeof(SurroundName[0]));
        break;

    case static_cast<int>(Setting_SoundAutoVolume):
        IncreaseValueComm(AutoVolume, 0, sizeof(AutoVolumeName)/sizeof(AutoVolumeName[0]));
        break;
    
    default:
        break;
    }
}

void Sound::DecreaseUserValue(int index)
{
    switch (index)
    {
    case static_cast<int>(Setting_SoundMode):
        DecreaseValueComm(mode.type, 0, static_cast<int>(mode.SoundMode_Num));
        break;

    case static_cast<int>(Setting_SoundSurround):
        DecreaseValueComm(surround, 0, sizeof(SurroundName)/sizeof(SurroundName[0]));
        break;

    case static_cast<int>(Setting_SoundAutoVolume):
        DecreaseValueComm(AutoVolume, 0, sizeof(AutoVolumeName)/sizeof(AutoVolumeName[0]));
        break;
        
    default:
        break;
    }
}

const char* Sound::GetStr(int index)
{
    const char* name = nullptr;
    switch (index)
    {
    case static_cast<int>(Setting_SoundMode):
        name = mode.name[mode.type];
        break;

    case static_cast<int>(Setting_SoundSurround):
        name = SurroundName[surround];
        break;

    case static_cast<int>(Setting_SoundAutoVolume):
        name = AutoVolumeName[AutoVolume];
        break;

    default:
        break;
    }
    return name;
}

void* Sound::GetDerivedAddress(int index)
{
    switch (index)
    {
    case static_cast<int>(Setting_SoundMode):
        if(mode.type == static_cast<int>(SoundMode::SoundMode_User))
            return mode.GetDerivedAddress(0);
        break;

    default:
        break;
    }
    return nullptr;
}

const char** Sound::SoundMode::GetStrArray(void)
{
    //const char** name = nullptr;
    static const char* name_list[3] = { 
    "setting_p_sound_mode", 
    "setting_p_sound_mode_treble", 
    "setting_p_sound_mode_bass" };
    return name_list;
}

int Sound::SoundMode::GetUserValue(int index)
{
    switch (index)
    {
    case static_cast<int>(ModeType::Content_Treble):
        return user.treble;

    case static_cast<int>(ModeType::Content_Bass):
        return user.bass;
    
    default:
        break;
    }
    return 0;
}

void Sound::SoundMode::IncreaseUserValue(int index)
{
    switch (index)
    {
    case static_cast<int>(ModeType::Content_Treble):
        IncreaseValueComm(user.treble, 0, 100 + 1);
        break;

    case static_cast<int>(ModeType::Content_Bass):
        IncreaseValueComm(user.bass, 0, 100 + 1);
        break;
    
    default:
        break;
    }
}

void Sound::SoundMode::DecreaseUserValue(int index)
{
    switch (index)
    {
    case static_cast<int>(ModeType::Content_Treble):
        DecreaseValueComm(user.treble, 0, 100 + 1);
        break;

    case static_cast<int>(ModeType::Content_Bass):
        DecreaseValueComm(user.bass, 0, 100 + 1);
        break;
    
    default:
        break;
    }
}

const char* Sound::SoundMode::GetStr(int index)
{
    const char* name_list = nullptr;
    switch (index)
    {
    case static_cast<int>(ModeType::Content_Treble):
        sprintf(user.TrebleName, "%d", user.treble);
        name_list = user.TrebleName;
        break;

    case static_cast<int>(ModeType::Content_Bass):
        sprintf(user.BassName, "%d", user.bass);
        name_list = user.BassName;
        break;
    
    default:
        break;
    }
    return name_list;
}

}
