/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-28 20:28:05
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-08-15 01:08:41
 * @FilePath: /gui/application/setting/Picture.h
 * @Description: None
 * @other: None
 */
#ifndef __PICTURE_SETTING_H__
#define __PICTURE_SETTING_H__

#include <stdint.h>
#include "SettingBase.h"
#include "lv_i18n/src/lv_i18n.h"

namespace Setting {

class Picture : public Base {
private:
    constexpr static uint8_t ItemNum = 5;
    constexpr static uint8_t OnlyTextItemNum = 0;

public:
    enum PicturegSettingItemList {
        Setting_PictureMode = 1,
        Setting_PictureScale,
        Setting_PictureColorTemperature,
        Setting_PictureRatio,
        Setting_PicturePowerBank,
        Setting_PicturegDataItemNum
    };
    
    class PictureMode : public Base {
        private:
        
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
            ModeType() = default;
            ModeType(uint8_t _contrast, uint8_t _brightness, uint8_t _colour, uint8_t _sharpness) : 
                contrast { _contrast }, brightness { _brightness }, colour { _colour }, sharpness { _sharpness } { }

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
        };
        ModeType standard{50, 50, 50, 50};
        ModeType soft{50, 50, 50, 50};
        ModeType user{50, 50, 50, 50};
        ModeType dynamic{50, 50, 50, 50};
        //constexpr static const char* name[4] = { setting_p_picture_mode_type_standard_
        const char* name[4] = { 
        "setting_p_picture_mode_type_standard", 
        "setting_p_picture_mode_type_standard_soft", 
        "setting_p_picture_mode_type_standard_user", 
        "setting_p_picture_mode_type_standard_dynamic" };

        PictureMode() {}
        ~PictureMode() {}

        virtual int GetItemNum(void) { return user.ItemNum; }
        virtual int GetOnlyTextItemNum(void) { return 0; }
        virtual const char** GetStrArray(void);
        virtual int GetUserValue(int index);
        virtual void SetUserValue(int index, int value) { }
        virtual void SelectedValue(int index) { }
        virtual void IncreaseUserValue(int index);
        virtual void DecreaseUserValue(int index);
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
            ColorTemperatureType() = default;
            ColorTemperatureType(uint8_t _red, uint8_t _green, uint8_t _blue) : 
                red { _red }, green { _green }, blue { _blue } { }

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
        };
        ColorTemperatureType standard{50, 50, 50};
        ColorTemperatureType warm{50, 50, 50};
        ColorTemperatureType user{50, 50, 50};
        ColorTemperatureType cool{50, 50, 50};
        //constexpr static const char* name[4] = { setting_p_picture_color_temp_type_
        const char* name[4] = { 
        "setting_p_picture_color_temp_type_standard", 
        "setting_p_picture_color_temp_type_warm", 
        "setting_p_picture_color_temp_type_user", 
        "setting_p_picture_color_temp_type_cool" };

        PictureColorTemperature() { }
        ~PictureColorTemperature() { }

        virtual int GetItemNum(void) { return user.ItemNum; }
        virtual int GetOnlyTextItemNum(void) { return 0; }
        virtual const char** GetStrArray(void);
        virtual int GetUserValue(int index);
        virtual void SetUserValue(int index, int value) { }
        virtual void SelectedValue(int index) { }
        virtual void IncreaseUserValue(int index);
        virtual void DecreaseUserValue(int index);
        virtual const char* GetStr(int index);
        virtual void* GetDerivedAddress(int index) { return this; }
    } ColorTemperature;

    uint8_t scale;
    //constexpr static const char* ScaleName[3] = { 
    const char* ScaleName[3] = { 
    "16:9", 
    "4:3", 
    "setting_p_picture_scale_auto" };

    uint8_t ratio;
    char RatioName[5];

    uint8_t PowerBankMode;
    //constexpr static const char* PowerBankModeName[3] = { 
    const char* PowerBankModeName[3] = { 
    "setting_p_picture_power_bank_mode_low", 
    "setting_p_picture_power_bank_mode_middle", 
    "setting_p_picture_power_bank_mode_high" };

public:
    Picture(/* args */);
    ~Picture();

    virtual int GetItemNum(void) { return ItemNum; }
    virtual int GetOnlyTextItemNum(void) { return OnlyTextItemNum; }
    virtual const char** GetStrArray(void);
    virtual int GetUserValue(int index);
    virtual void SetUserValue(int index, int value) { }
    virtual void SelectedValue(int index) { }
    virtual void IncreaseUserValue(int index);
    virtual void DecreaseUserValue(int index);
    virtual const char* GetStr(int index);
    virtual void* GetDerivedAddress(int index);

};

}
#endif
