/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-05-23 13:51:24
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-07-01 16:49:04
 * @FilePath: /gui/application/ui/HomeScreen.c
 * @Description: None
 * @other: None
 */
#include <unistd.h>
#include <stdio.h>
#include "HomeScreen.h"
#include "application/windows.h"
#include "ui_com.h"
#include "Volume.h"
#include "application/key_map.h"
#include "SettingScreen.h"
//#include "lv_i18n/src/lv_i18n.h"

lv_obj_t* ui_HomeScreen;
lv_obj_t* ui_Main_Panel;
lv_obj_t* ui_Source_Panel;
lv_obj_t* ui_IMG_SourceType;
lv_group_t* HomeScreenGroup;
int LastFocusedObjIndex;

LV_IMG_DECLARE(ui_img_udisk_n_png);    // assets\udisk_n.png
LV_IMG_DECLARE(ui_img_setting_n_png);    // assets\setting_n.png
LV_IMG_DECLARE(ui_img_source_n_png);    // assets\source_n.png
LV_IMG_DECLARE(ui_img_ios_cast_n_png);    // assets\ios_cast_n.png
LV_IMG_DECLARE(ui_img_android_cast_n_png);    // assets\android_cast_n.png
LV_IMG_DECLARE(ui_img_dlna_cast_n_png);    // assets\dlna_cast_n.png
LV_IMG_DECLARE(ui_img_usb2_big_png);    // assets\dlna_cast_f.png
LV_IMG_DECLARE(ui_img_hdmi_big_png);    // assets\dlna_cast_f.png
LV_IMG_DECLARE(ui_img_usb2_n_png);    // assets\dlna_cast_f.png
LV_IMG_DECLARE(ui_img_hdmi_n_png);    // assets\dlna_cast_f.png

LV_FONT_DECLARE(ui_font_MyFont30);
LV_FONT_DECLARE(ui_font_MyFont34);
LV_FONT_DECLARE(ui_font_MyFont38);

enum CategoryList {
    UDisk,
    Setting,
    Source,
    IOS,
    Android,
    DLNA,
    CategoryNumber
};

enum SourceList {
    SourceUDisk,
    HDMI,
    SourceListNumber
};

static const lv_img_dsc_t* source_img_src[] = {
    &ui_img_usb2_big_png,
    &ui_img_hdmi_big_png
};

static void event_handler(lv_event_t* event);
static void ExitHome(ActiveScreen screen);
static void source_event_handler(lv_event_t* event);

static void CreateMainPanel(lv_obj_t* parent)
{
    static const lv_coord_t img_area[CategoryNumber][4] = {//{ x, y, w, h}
        { -226, -82, 430, 231},
        {  118, -82, 200, 231},
        {  345,   0, 200, 395},
        { -342, 130, 200, 135},
        { -110, 130, 200, 135},
        {  118, 130, 200, 135}
    };
    static const lv_coord_t lab_area[CategoryNumber][2] = {//{ x, y}
        { -124,  -60},
        {    0,  -60},
        {    0, -142},
        {    0,  -38},
        {    0,  -38},
        {   -8,  -38}
    };
    static const char* str[] =  { "U Disk", "Setting", "Source", "IOS", "Android", "DLNA"};
    static const lv_img_dsc_t* image_src[] = {
        & ui_img_udisk_n_png,
        & ui_img_setting_n_png,
        & ui_img_source_n_png,
        & ui_img_ios_cast_n_png,
        & ui_img_android_cast_n_png,
        & ui_img_dlna_cast_n_png
    };

    // ui_Main_Panel
    ui_Main_Panel = lv_obj_create(parent);
    lv_obj_set_size(ui_Main_Panel, 1085, 650);
    lv_obj_set_pos(ui_Main_Panel, 0, 0);
    lv_obj_set_align(ui_Main_Panel, LV_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(ui_Main_Panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_Main_Panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    HomeScreenGroup = lv_group_create();
    set_group_activity(HomeScreenGroup);
    for (int i = 0; i < CategoryNumber; i++) {
        lv_obj_t* ui_btn = lv_btn_create(ui_Main_Panel);
        lv_obj_set_size(ui_btn, img_area[i][2], img_area[i][3]);
        lv_obj_set_pos(ui_btn, img_area[i][0], img_area[i][1]);
        lv_obj_set_align(ui_btn, LV_ALIGN_CENTER);
        lv_obj_set_style_radius(ui_btn, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(ui_btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_opa(ui_btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_opa(ui_btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_img_src(ui_btn, image_src[i], LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(ui_btn, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_border_opa(ui_btn, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_border_width(ui_btn, 5, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_group_add_obj(HomeScreenGroup, ui_btn);
        lv_obj_add_event_cb(ui_btn, event_handler, LV_EVENT_KEY, NULL);

        lv_obj_t* ui_lab = lv_label_create(ui_btn);
        lv_obj_set_size(ui_lab, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_pos(ui_lab, lab_area[i][0], lab_area[i][1]);
        lv_obj_set_align(ui_lab, LV_ALIGN_CENTER);
        lv_label_set_text(ui_lab, str[i]);
        lv_obj_set_style_text_font(ui_lab, &ui_font_MyFont38, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    lv_group_focus_obj(lv_obj_get_child(parent, LastFocusedObjIndex));
}

static void CreateSourcePanel(lv_obj_t* parent)
{
    ui_Source_Panel = lv_obj_create(parent);
    lv_obj_set_size(ui_Source_Panel, 1280, 720);
    lv_obj_clear_flag(ui_Source_Panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_Source_Panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Source_Panel, lv_color_hex(0x3500FE), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Source_Panel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_IMG_SourceType
    ui_IMG_SourceType = lv_img_create(ui_Source_Panel);
    lv_img_set_src(ui_IMG_SourceType, &ui_img_usb2_big_png);
    lv_obj_set_size(ui_IMG_SourceType, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_pos(ui_IMG_SourceType, 0, -80);
    lv_obj_set_align(ui_IMG_SourceType, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_IMG_SourceType, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_IMG_SourceType, LV_OBJ_FLAG_SCROLLABLE);
    // ui_LAB_SourceType
    lv_obj_t* ui_LAB_SourceType = lv_label_create(ui_IMG_SourceType);
    lv_obj_set_size(ui_LAB_SourceType, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_pos(ui_LAB_SourceType, 0, 84);
    lv_obj_set_align(ui_LAB_SourceType, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LAB_SourceType, "U Disk");
    lv_obj_set_style_text_color(ui_LAB_SourceType, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LAB_SourceType, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LAB_SourceType, &ui_font_MyFont38, LV_PART_MAIN | LV_STATE_DEFAULT);

    HomeScreenGroup = create_new_group(HomeScreenGroup);
    set_group_activity(HomeScreenGroup);
    // ui_BTN_Source_USB
    lv_obj_t* ui_BTN_Source_USB = lv_btn_create(ui_Source_Panel);
    lv_obj_set_size(ui_BTN_Source_USB, 113, 107);
    lv_obj_set_pos(ui_BTN_Source_USB, -87, 160);
    lv_obj_set_align(ui_BTN_Source_USB, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_BTN_Source_USB, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_BTN_Source_USB, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_BTN_Source_USB, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_BTN_Source_USB, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui_BTN_Source_USB, &ui_img_usb2_n_png, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_BTN_Source_USB, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(ui_BTN_Source_USB, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_BTN_Source_USB, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_border_opa(ui_BTN_Source_USB, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui_BTN_Source_USB, 5, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_group_add_obj(HomeScreenGroup, ui_BTN_Source_USB);
    lv_obj_add_event_cb(ui_BTN_Source_USB, source_event_handler, LV_EVENT_KEY, NULL);

    // ui_BTN_Source_HDMI
    lv_obj_t* ui_BTN_Source_HDMI = lv_btn_create(ui_Source_Panel);
    lv_obj_set_size(ui_BTN_Source_HDMI, 113, 107);
    lv_obj_set_pos(ui_BTN_Source_HDMI, 87, 160);
    lv_obj_set_align(ui_BTN_Source_HDMI, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_BTN_Source_HDMI, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_BTN_Source_HDMI, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_BTN_Source_HDMI, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_BTN_Source_HDMI, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui_BTN_Source_HDMI, &ui_img_hdmi_n_png, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_BTN_Source_HDMI, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(ui_BTN_Source_HDMI, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_BTN_Source_HDMI, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_border_opa(ui_BTN_Source_HDMI, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui_BTN_Source_HDMI, 5, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_group_add_obj(HomeScreenGroup, ui_BTN_Source_HDMI);
    lv_obj_add_event_cb(ui_BTN_Source_HDMI, source_event_handler, LV_EVENT_KEY, NULL);
}

static void event_handler(lv_event_t* event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t* target = lv_event_get_target(event);
    lv_obj_t* parent = lv_obj_get_parent(target);
    if (LV_EVENT_KEY == code) {
        uint32_t value = lv_indev_get_key(lv_indev_get_act());
        lv_group_t* group = (lv_group_t*)lv_obj_get_group(target);
        switch (value)
        {
        case LV_KEY_LEFT:
            lv_group_focus_prev(group);
            break;
        case LV_KEY_RIGHT:
            lv_group_focus_next(group);
            break;
        case LV_KEY_UP:
            switch (lv_obj_get_index(target))
            {
            case IOS:
            case Android:
                lv_group_focus_obj(lv_obj_get_child(parent, UDisk));
                break;
            case DLNA:
                lv_group_focus_obj(lv_obj_get_child(parent, Setting));
                break;
            default:
                break;
            }
            break;
        case LV_KEY_DOWN:
            switch (lv_obj_get_index(target))
            {
            case UDisk:
                lv_group_focus_obj(lv_obj_get_child(parent, IOS));
                break;
            case Setting:
                lv_group_focus_obj(lv_obj_get_child(parent, DLNA));
                break;
            default:
                break;
            }
            break;
        case LV_KEY_ENTER:
            switch (lv_obj_get_index(target))
            {
            case UDisk:
                ExitHome(MediaScreen);
                break;
            case Setting:
                CreateSettingScreen(ui_HomeScreen, HomeScreenGroup);
                //ExitHome(SettingScreen);
                break;
            case Source:
                CreateSourcePanel(ui_HomeScreen);
                break;
            default:
                break;
            }
            break;
        case LV_KEY_VOLUME_UP:
        case LV_KEY_VOLUME_DOWN:
            SetVolume(value);
            break;

        default:
            break;
        }
    }
    else if (LV_EVENT_FOCUSED == code) {
        lv_img_set_src(ui_IMG_SourceType, source_img_src[lv_obj_get_index(target)]);
    }
}

static void source_event_handler(lv_event_t* event)
{
    lv_obj_t* target = lv_event_get_target(event);
    uint32_t value = lv_indev_get_key(lv_indev_get_act());
    lv_group_t* group = (lv_group_t*)lv_obj_get_group(target);
    switch (value)
    {
    case LV_KEY_LEFT:
        lv_group_focus_prev(group);
        break;
    case LV_KEY_RIGHT:
        lv_group_focus_next(group);
        break;
    case LV_KEY_ESC:
        HomeScreenGroup = delete_group(HomeScreenGroup);
        lv_obj_del_async(ui_Source_Panel);
        break;
    default:
        break;
    }
}

static void ExitHome(ActiveScreen screen)
{
    LastFocusedObjIndex = lv_obj_get_index(lv_group_get_focused(HomeScreenGroup));
    CurrentScreen = screen;
}

static void HomeInit(lv_obj_t* parent, void *param)
{
    (void)param;

    ui_HomeScreen = lv_obj_create(parent);
    lv_obj_set_size(ui_HomeScreen, 1280, 720);
    lv_obj_set_style_bg_color(ui_HomeScreen, lv_color_hex(0x3200FE), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_HomeScreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    CreateMainPanel(ui_HomeScreen);
}

static void LoadHome(void)
{
    //lv_disp_load_scr(ui_HomeScreen);
    lv_scr_load_anim(ui_HomeScreen, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0, true);
}

static void HomeClose(void)
{
    lv_group_del(HomeScreenGroup);
    //lv_obj_del(ui_HomeScreen);
}

window HomeWindow = {
    .ScreenInit = HomeInit,
    .ScreenLoad = LoadHome,
    .ScreenClose = HomeClose
};
