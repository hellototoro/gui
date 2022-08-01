/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-28 20:28:05
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-08-01 11:55:11
 * @FilePath: /gui/application/ui/setting/Picture.h
 * @Description: None
 * @other: None
 */
#ifndef __PICTURE_SETTING_H__
#define __PICTURE_SETTING_H__

#include <stdint.h>
#include "Base.h"

namespace Setting {

class Picture : public Base {
public:
    constexpr static uint8_t ItemNum = 5;
    constexpr static uint8_t OnlyTextItemNum = 0;

    enum PicturegSettingItemList {
        Setting_PictureMode = 1,
        Setting_PictureScale,
        Setting_PictureColorTemperature,
        Setting_PictureRatio,
        Setting_PicturePowerBank,
        Setting_PicturegDataItemNum
    };
    
    class PictureMode : public Base {
        public:
        enum PictureModeType {
            PictureMode_Standard,
            PictureMode_Soft,
            PictureMode_User,
            PictureMode_Dynamic,
            PictureMode_Num
        };
        uint8_t type;//标准；柔和；用户；动态
        class ModeType {
            public:
            constexpr static uint8_t ItemNum = 4;
            enum ContentType {
                Content_Contrast = 1,
                Content_Brightness,
                Content_Colour,
                Content_Sharpness,
                Content_Num
            };
            uint8_t contrast;
            char ContrastName[4];
            uint8_t brightness;
            char BrightnessName[4];
            uint8_t colour;
            char ColourName[4];
            uint8_t sharpness;
            char SharpnessName[4];
        } standard, soft, user, dynamic;
        constexpr static const char* name[4] = { 
        "标准", "柔和", "用户", "动态" };

        PictureMode() {}
        ~PictureMode() {}

        virtual int GetItemNum(void) { return user.ItemNum; }
        virtual int GetOnlyTextItemNum(void) { return 0; }
        virtual const char** GetStrArray(void);
        virtual int GetValue(int index);
        virtual void IncreaseValue(int index);
        virtual void DecreaseValue(int index);
        virtual const char* GetStr(int index);
        virtual void* GetDerivedAddress(int index) { return this; }
    } mode;

    class PictureColorTemperature : public Base {
        public:
        enum PictureColorTemperatureType {
            PictureColorTemperature_Standard,
            PictureColorTemperature_Warm,
            PictureColorTemperature_User,
            PictureColorTemperature_Cool,
            PictureColorTemperature_Num
        };
        uint8_t type;//标准；暖色；用户；冷色
        class ColorTemperatureType {
            public:
            constexpr static uint8_t ItemNum = 3;
            enum ContentType {
                Content_Red = 1,
                Content_Green,
                Content_Blue,
                Content_Num
            };
            uint8_t red;
            char RedName[4];
            uint8_t green;
            char GreenName[4];
            uint8_t blue;
            char BlueName[4];
        } standard, warm, user, cool;
        constexpr static const char* name[4] = { 
        "标准", "暖色", "用户", "冷色" };

        PictureColorTemperature() {}
        ~PictureColorTemperature() {}

        virtual int GetItemNum(void) { return user.ItemNum; }
        virtual int GetOnlyTextItemNum(void) { return 0; }
        virtual const char** GetStrArray(void);
        virtual int GetValue(int index);
        virtual void IncreaseValue(int index);
        virtual void DecreaseValue(int index);
        virtual const char* GetStr(int index);
        virtual void* GetDerivedAddress(int index) { return this; }
    } ColorTemperature;

    uint8_t scale;
    constexpr static const char* ScaleName[3] = { 
    "16:9", "4:3", "自动" };

    uint8_t ratio;
    char RatioName[5];

    uint8_t PowerBankMode;
    constexpr static const char* PowerBankModeName[3] = { 
    "低", "中", "高" };

public:
    Picture(/* args */);
    ~Picture();

    virtual int GetItemNum(void) { return ItemNum; }
    virtual int GetOnlyTextItemNum(void) { return OnlyTextItemNum; }
    virtual const char** GetStrArray(void);
    virtual int GetValue(int index);
    virtual void IncreaseValue(int index);
    virtual void DecreaseValue(int index);
    virtual const char* GetStr(int index);
    virtual void* GetDerivedAddress(int index);

};

extern Picture* CreatePictureSettingObj(void);
extern void DeletePictureSettingObj(Picture* PictureData);

}
#endif
