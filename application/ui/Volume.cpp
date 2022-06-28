/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-27 21:51:44
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-06-28 15:47:27
 * @FilePath: /gui/application/ui/Volume.cpp
 * @Description: None
 * @other: None
 */
#include "Volume.h"
#include "application/key_map.h"

lv_obj_t* VolumePanel;
lv_obj_t* VolumeSlider;
lv_obj_t* VolumeImage;
lv_timer_t* VolumeTimer;
const lv_img_dsc_t* VolumeImg;

uint8_t Volume;

LV_IMG_DECLARE(ui_img_audio_volume_png);    // assets\dlna_cast_f.png
LV_IMG_DECLARE(ui_img_audio_bluetooth_connect_png);    // assets\dlna_cast_f.png
LV_IMG_DECLARE(ui_img_audio_decrease_volume_png);    // assets\dlna_cast_f.png
LV_IMG_DECLARE(ui_img_audio_increase_volume_png);    // assets\dlna_cast_f.png
LV_IMG_DECLARE(ui_img_audio_mute_volume_png);    // assets\dlna_cast_f.png

static void CreateVolumePanel(lv_obj_t* parent);
static void VolumeTimer_cb(lv_timer_t * timer);

void SetVolume(uint32_t value)
{
    static const lv_img_dsc_t* LastVolumeImg = NULL;
    switch (value)
    {
        case LV_KEY_VOLUME_UP:
            if (Volume < 100) Volume += 5;
            break;

        case LV_KEY_VOLUME_DOWN:
            if (Volume > 0) Volume -= 5;
            break;

        default :
        break;
    }
    if (!lv_obj_is_valid(VolumePanel)) {
        CreateVolumePanel(lv_scr_act());
        VolumeTimer = lv_timer_create(VolumeTimer_cb, 3*1000, NULL);
        LastVolumeImg = NULL;
    }
    else {
        lv_timer_reset(VolumeTimer);
    }
    if (Volume == 0)
        VolumeImg = &ui_img_audio_mute_volume_png;
    else
        VolumeImg = &ui_img_audio_volume_png;
    if (LastVolumeImg != VolumeImg) {
        LastVolumeImg = VolumeImg;
        lv_img_set_src(VolumeImage, VolumeImg);
    }
    lv_slider_set_value(VolumeSlider, Volume, LV_ANIM_ON);
}

static void CreateVolumePanel(lv_obj_t* parent)
{
    // VolumePanel
    VolumePanel = lv_obj_create(parent);
    lv_obj_set_width(VolumePanel, 60);
    lv_obj_set_height(VolumePanel, 380);
    lv_obj_set_x(VolumePanel, 580);
    lv_obj_set_y(VolumePanel, 0);
    lv_obj_set_align(VolumePanel, LV_ALIGN_CENTER);
    lv_obj_clear_flag(VolumePanel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(VolumePanel, 50, LV_PART_MAIN | LV_STATE_DEFAULT);

    // VolumeSlider
    VolumeSlider = lv_slider_create(VolumePanel);
    lv_slider_set_range(VolumeSlider, 0, 100);
    if(lv_slider_get_mode(VolumeSlider) == LV_SLIDER_MODE_RANGE) lv_slider_set_left_value(VolumeSlider, 50, LV_ANIM_OFF);
    lv_obj_set_width(VolumeSlider, 14);
    lv_obj_set_height(VolumeSlider, 264);
    lv_obj_set_x(VolumeSlider, 0);
    lv_obj_set_y(VolumeSlider, -30);
    lv_obj_set_align(VolumeSlider, LV_ALIGN_CENTER);

    // VolumeImage
    VolumeImage = lv_img_create(VolumePanel);
    //lv_img_set_src(VolumeImage, &ui_img_audio_volume_png);
    lv_obj_set_width(VolumeImage, LV_SIZE_CONTENT);
    lv_obj_set_height(VolumeImage, LV_SIZE_CONTENT);
    lv_obj_set_x(VolumeImage, 0);
    lv_obj_set_y(VolumeImage, 145);
    lv_obj_set_align(VolumeImage, LV_ALIGN_CENTER);
    lv_obj_add_flag(VolumeImage, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(VolumeImage, LV_OBJ_FLAG_SCROLLABLE);
}

static void VolumeTimer_cb(lv_timer_t * timer)
{
    lv_obj_del_async(VolumePanel);
    lv_timer_del(VolumeTimer);
}
