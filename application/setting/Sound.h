/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-08-01 13:36:19
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-08-01 21:42:57
 * @FilePath: /gui/application/ui/setting/Sound.h
 * @Description: None
 * @other: None
 */
#ifndef __SOUND_SETTING_H__
#define __SOUND_SETTING_H__

#include <stdint.h>
#include "Base.h"

namespace Setting {

class Sound : public Base {
public:
    constexpr static uint8_t ItemNum = 3;
    constexpr static uint8_t OnlyTextItemNum = 0;

    enum SoundgSettingItemList {
        Setting_SoundMode = 1,
        Setting_SoundSurround,
        Setting_SoundAutoVolume,
        Setting_SoundgDataItemNum
    };
    
    class SoundMode : public Base {
        public:
        enum SoundModeType {
            SoundMode_Standard,
            SoundMode_Music,
            SoundMode_Movie,
            SoundMode_Sport,
            SoundMode_User,
            SoundMode_Num
        };
        uint8_t type;//标准；音乐；电影；运动；用户
        class ModeType {
            public:
            ModeType() = default;
            ModeType(uint8_t _treble, uint8_t _bass) : 
                treble { _treble }, bass { _bass } { }

            constexpr static uint8_t ItemNum = 2;
            enum ContentType {
                Content_Treble = 1,
                Content_Bass,
                Content_Num
            };
            uint8_t treble;
            char TrebleName[4];
            uint8_t bass;
            char BassName[4];
        };
        ModeType standard{50, 50};
        ModeType music{50, 50};
        ModeType movie{50, 50};
        ModeType sport{50, 50};
        ModeType user{50, 50};
        //constexpr static const char* name[] = { 
        const char* name[5] = { 
        "标准", "音乐", "电影", "运动", "用户" };

        SoundMode() {}
        ~SoundMode() {}

        virtual int GetItemNum(void) { return user.ItemNum; }
        virtual int GetOnlyTextItemNum(void) { return 0; }
        virtual const char** GetStrArray(void);
        virtual int GetValue(int index);
        virtual void IncreaseValue(int index);
        virtual void DecreaseValue(int index);
        virtual const char* GetStr(int index);
        virtual void* GetDerivedAddress(int index) { return this; }
    } mode;

    uint8_t surround;
    //constexpr static const char* SurroundName[] = { 
    const char* SurroundName[2] = { 
    "关", "SRS 环绕" };

    uint8_t AutoVolume;
    //constexpr static const char* AutoVolumeName[] = { 
    const char* AutoVolumeName[2] = { 
    "关", "开" };

public:
    Sound(/* args */);
    ~Sound();

    virtual int GetItemNum(void) { return ItemNum; }
    virtual int GetOnlyTextItemNum(void) { return OnlyTextItemNum; }
    virtual const char** GetStrArray(void);
    virtual int GetValue(int index);
    virtual void IncreaseValue(int index);
    virtual void DecreaseValue(int index);
    virtual const char* GetStr(int index);
    virtual void* GetDerivedAddress(int index);

};

}
#endif
