/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-28 20:28:05
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-07-31 20:11:43
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
    class PictureMode {
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
            uint8_t contrast;
            uint8_t brightness;
            uint8_t colour;
            uint8_t sharpness;
        } standard, soft, user, dynamic;
        constexpr static const char* name[4] = { 
        "标准", "柔和", "用户", "动态" };
    } mode;

    class PictureColorTemperature {
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
            uint8_t red;
            uint8_t green;
            uint8_t blue;
        } standard, warm, user, cool;
        constexpr static const char* name[4] = { 
        "标准", "暖色", "用户", "冷色" };
    } ColorTemperature;

    uint8_t scale;
    constexpr static const char* ScaleName[3] = { 
    "16:9", "4:3", "自动" };

    uint8_t ratio;
    //const char* RatioName;
    constexpr static const char* RatioName[] = { 
    "75%", "76%", "77%", "78%", "79%", 
    "80%", "81%", "82%", "83%", "84%", "85%", "86%", "87%", "88%", "89%", 
    "90%", "91%", "92%", "93%", "94%", "95%", "96%", "97%", "98%", "99%", "100%" };

    uint8_t PowerBankMode;
    constexpr static const char* PowerBankModeName[3] = { 
    "低", "中", "高" };

public:
    Picture(/* args */);
    ~Picture();

    void IncreaseDataComm(uint8_t& data, int MinRange, int MaxRange);
    void DecreaseDataComm(uint8_t& data, int MinRange, int MaxRange);

    virtual int GetItemNum(void) { return ItemNum; }
    virtual int GetOnlyTextItemNum(void) { return OnlyTextItemNum; }
    virtual const char** GetStrArray(void);
    virtual void SelectData(int index);
    virtual void IncreaseData(int index);
    virtual void DecreaseData(int index);
    virtual const char* GetStr(int index);

};

extern Picture* CreatePictureSettingObj(void);
extern void DeletePictureSettingObj(Picture* PictureData);

}
#endif
