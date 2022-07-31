/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-07-28 20:27:47
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-07-31 20:11:53
 * @FilePath: /gui/application/ui/setting/Picture.cpp
 * @Description: None
 * @other: None
 */
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

void Picture::IncreaseDataComm(uint8_t& data, int MinRange, int MaxRange)
{
    if (data < MaxRange - 1) ++data;
    else                     data = MinRange;
}

void Picture::DecreaseDataComm(uint8_t& data, int MinRange, int MaxRange)
{
    if (data == MinRange) data = MaxRange - 1;
    else                  --data;
}

void Picture::SelectData(int index)
{

}

void Picture::IncreaseData(int index)
{
    switch (index)
    {
    case static_cast<int>(Setting_PictureMode):
        IncreaseDataComm(mode.type, 0, static_cast<int>(mode.PictureMode_Num));
        break;

    case static_cast<int>(Setting_PictureScale):
        IncreaseDataComm(scale, 0, sizeof(ScaleName)/sizeof(ScaleName[0]));
        break;

    case static_cast<int>(Setting_PictureColorTemperature):
        IncreaseDataComm(ColorTemperature.type, 0, static_cast<int>(ColorTemperature.PictureColorTemperature_Num));
        break;
    
    case static_cast<int>(Setting_PictureRatio):
        IncreaseDataComm(ratio, 75, 100 + 1);
        break;
    
    case static_cast<int>(Setting_PicturePowerBank):
        IncreaseDataComm(PowerBankMode, 0, sizeof(PowerBankModeName)/sizeof(PowerBankModeName[0]));
        break;
    
    default:
        break;
    }
}

void Picture::DecreaseData(int index)
{
    switch (index)
    {
    case static_cast<int>(Setting_PictureMode):
        DecreaseDataComm(mode.type, 0, static_cast<int>(mode.PictureMode_Num));
        break;

    case static_cast<int>(Setting_PictureScale):
        DecreaseDataComm(scale, 0, sizeof(ScaleName)/sizeof(ScaleName[0]));
        break;

    case static_cast<int>(Setting_PictureColorTemperature):
        DecreaseDataComm(ColorTemperature.type, 0, static_cast<int>(ColorTemperature.PictureColorTemperature_Num));
        break;
        
    case static_cast<int>(Setting_PictureRatio):
        DecreaseDataComm(ratio, 75, 100 + 1);
        break;
        
    case static_cast<int>(Setting_PicturePowerBank):
        DecreaseDataComm(PowerBankMode, 0, sizeof(PowerBankModeName)/sizeof(PowerBankModeName[0]));
        break;
        
    default:
        break;
    }
}

const char* Picture::GetStr(int index)
{
    const char* name = "";
    switch (index)
    {
    case static_cast<int>(Setting_PictureMode):
        //name = mode.name;
        name = mode.name[mode.type];
        break;

    case static_cast<int>(Setting_PictureScale):
        name = ScaleName[scale];
        break;

    case static_cast<int>(Setting_PictureColorTemperature):
        name = ColorTemperature.name[ColorTemperature.type];
        break;

    case static_cast<int>(Setting_PictureRatio):
        name = RatioName[ratio - 75];
        break;

    case static_cast<int>(Setting_PicturePowerBank):
        name = PowerBankModeName[PowerBankMode];
        break;

    default:
        break;
    }
    return name;
}

void PictureSettingInit(Picture* setting)
{
    setting->mode.type = static_cast<uint8_t>(setting->mode.PictureMode_Standard);

    setting->scale = 0;
    
    setting->ColorTemperature.type = static_cast<uint8_t>(setting->ColorTemperature.PictureColorTemperature_Standard);
    
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

}
