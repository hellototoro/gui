/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-27 21:51:44
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-06-29 14:12:59
 * @FilePath: /gui/application/ui/Volume.cpp
 * @Description: None
 * @other: None
 */
#include "Volume.h"
#include "application/key_map.h"
#ifdef HCCHIP_GCC
#include "hcapi/media_player.h"
#endif

lv_obj_t* VolumePanel;
lv_obj_t* VolumeSlider;
lv_obj_t* VolumeImage;
lv_timer_t* VolumeTimer;
const lv_img_dsc_t* VolumeImg;
bool PlayingAnimation_Flag;
uint8_t Volume;

LV_IMG_DECLARE(ui_img_audio_volume_png);    // assets\dlna_cast_f.png
LV_IMG_DECLARE(ui_img_audio_bluetooth_connect_png);    // assets\dlna_cast_f.png
LV_IMG_DECLARE(ui_img_audio_decrease_volume_png);    // assets\dlna_cast_f.png
LV_IMG_DECLARE(ui_img_audio_increase_volume_png);    // assets\dlna_cast_f.png
LV_IMG_DECLARE(ui_img_audio_mute_volume_png);    // assets\dlna_cast_f.png

static void CreateVolumePanel(lv_obj_t* parent);
static void VolumeTimer_cb(lv_timer_t * timer);
static int32_t anim_callback_get_opacity(lv_anim_t * a);
static void anim_callback_set_opacity(lv_anim_t * a, int32_t v);
static void fade_up_Animation(lv_obj_t * TargetObject, int delay);
static void fade_down_Animation(lv_obj_t * TargetObject, int delay);
static void anim_callback_delete_obj(struct _lv_anim_t *a);

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
        fade_up_Animation(VolumePanel, 300);
        VolumeTimer = lv_timer_create(VolumeTimer_cb, 2*1000, NULL);
        LastVolumeImg = NULL;
        PlayingAnimation_Flag = false;
    }
    else {
        if (PlayingAnimation_Flag)
            return;
        else 
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
    #ifdef HCCHIP_GCC
    media_set_vol(Volume);
    #endif
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
    lv_obj_set_style_opa(VolumePanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
}

static void VolumeTimer_cb(lv_timer_t * timer)
{
    fade_down_Animation(VolumePanel, 300);
    PlayingAnimation_Flag = true;
    lv_timer_del(VolumeTimer);
}

static int32_t anim_callback_get_opacity(lv_anim_t * a)
{
    return lv_obj_get_style_opa((lv_obj_t *)a->user_data, 0);
}

static void anim_callback_set_opacity(lv_anim_t * a, int32_t v)
{
    lv_obj_set_style_opa((lv_obj_t *)a->user_data, v, 0);
}

static void fade_up_Animation(lv_obj_t * TargetObject, int delay)
{
    lv_anim_t PropertyAnimation_0;
    lv_anim_init(&PropertyAnimation_0);
    lv_anim_set_time(&PropertyAnimation_0, 300);
    lv_anim_set_user_data(&PropertyAnimation_0, TargetObject);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_0, anim_callback_set_opacity);
    lv_anim_set_values(&PropertyAnimation_0, 0, 255);
    lv_anim_set_path_cb(&PropertyAnimation_0, lv_anim_path_ease_in);
    lv_anim_set_delay(&PropertyAnimation_0, delay + 0);
    lv_anim_set_early_apply(&PropertyAnimation_0, false);
    lv_anim_set_get_value_cb(&PropertyAnimation_0, &anim_callback_get_opacity);
    lv_anim_start(&PropertyAnimation_0);
}

static void anim_callback_delete_obj(struct _lv_anim_t *a)
{
    lv_obj_del_async((lv_obj_t *)a->user_data);
    PlayingAnimation_Flag = false;
}

static void fade_down_Animation(lv_obj_t * TargetObject, int delay)
{
    lv_anim_t PropertyAnimation_0;
    lv_anim_init(&PropertyAnimation_0);
    lv_anim_set_time(&PropertyAnimation_0, 300);
    lv_anim_set_user_data(&PropertyAnimation_0, TargetObject);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_0, anim_callback_set_opacity);
    lv_anim_set_values(&PropertyAnimation_0, 255, 0);
    lv_anim_set_path_cb(&PropertyAnimation_0, lv_anim_path_ease_out);
    lv_anim_set_delay(&PropertyAnimation_0, delay + 0);
    lv_anim_set_early_apply(&PropertyAnimation_0, false);
    lv_anim_set_get_value_cb(&PropertyAnimation_0, &anim_callback_get_opacity);
    lv_anim_set_deleted_cb(&PropertyAnimation_0, anim_callback_delete_obj);
    lv_anim_start(&PropertyAnimation_0);
}
