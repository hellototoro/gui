/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-26 11:13:24
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-06-29 13:41:11
 * @FilePath: /gui/application/ui/SettingScreen.cpp
 * @Description: None
 * @other: None
 */
#include "SettingScreen.h"
#include "ui_com.h"

lv_obj_t* ui_SettingScreen;
lv_group_t* SettingGroup;

enum CategoryList {
    NetWork,
    Projector,
    Picture,
    Sound,
    Setting,
    Update,
    ProjectorInfo,
    CategoryNumber
};

LV_IMG_DECLARE(ui_img_network_setting_png);    // assets\dlna_cast_f.png
LV_IMG_DECLARE(ui_img_projector_setting_png);    // assets\dlna_cast_f.png
LV_IMG_DECLARE(ui_img_pic_setting_png);    // assets\dlna_cast_f.png
LV_IMG_DECLARE(ui_img_sound_setting_png);    // assets\dlna_cast_f.png
LV_IMG_DECLARE(ui_img_system_setting_png);    // assets\dlna_cast_f.png
LV_IMG_DECLARE(ui_img_update_setting_png);    // assets\dlna_cast_f.png
LV_IMG_DECLARE(ui_img_projector_info_png);    // assets\dlna_cast_f.png

LV_FONT_DECLARE(ui_font_MyFont34);

static void event_handler(lv_event_t* event);

void CreateSettingScreen(lv_obj_t* parent, lv_group_t* group)
{
    static const lv_coord_t img_area[CategoryNumber][2] = {//{ x, y}
        {-330,    0},
        {-110, -102},
        { 110, -102},
        { 330, -102},
        {-110,  101},
        { 110,  101},
        { 330,  101}
    };
    static const lv_coord_t lab_area[CategoryNumber][2] = {//{ x, y}
        { 0, 75},
        { 0, 45},
        { 0, 45},
        { 0, 45},
        { 0, 45},
        { 0, 45},
        { 0, 45}
    };
    static const char* str[CategoryNumber] =  { "网络设置", "投影设置", "图像设置", "声音设置", "系统设置", "系统升级", "关于投影"};
    static const lv_img_dsc_t* image_src[CategoryNumber] = {
        & ui_img_network_setting_png,
        & ui_img_projector_setting_png,
        & ui_img_pic_setting_png,
        & ui_img_sound_setting_png,
        & ui_img_system_setting_png,
        & ui_img_update_setting_png,
        & ui_img_projector_info_png,
    };
    // ui_SettingScreen
    ui_SettingScreen = lv_obj_create(parent);
    lv_obj_set_size(ui_SettingScreen, 1280, 720);
    lv_obj_clear_flag(ui_SettingScreen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_SettingScreen, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_SettingScreen, lv_color_hex(0x3500FE), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_SettingScreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    //HomeScreenGroup = lv_group_create();
    //set_group_activity(HomeScreenGroup);
    SettingGroup = create_new_group(group);
    set_group_activity(SettingGroup);
    for (int i = 0; i < CategoryNumber; i++) {
        lv_obj_t* ui_Image = lv_img_create(ui_SettingScreen);
        lv_img_set_src(ui_Image, image_src[i]);
        lv_obj_set_width(ui_Image, LV_SIZE_CONTENT);
        lv_obj_set_height(ui_Image, LV_SIZE_CONTENT);
        lv_obj_set_x(ui_Image, img_area[i][0]);
        lv_obj_set_y(ui_Image, img_area[i][1]);
        lv_obj_set_align(ui_Image, LV_ALIGN_CENTER);
        lv_obj_add_flag(ui_Image, LV_OBJ_FLAG_ADV_HITTEST);
        lv_obj_clear_flag(ui_Image, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_radius(ui_Image, 20, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_border_color(ui_Image, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_border_opa(ui_Image, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_border_width(ui_Image, 5, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_group_add_obj(SettingGroup, ui_Image);
        lv_obj_add_event_cb(ui_Image, event_handler, LV_EVENT_ALL, NULL);

        lv_obj_t* ui_Label = lv_label_create(ui_Image);
        lv_obj_set_width(ui_Label, LV_SIZE_CONTENT);
        lv_obj_set_height(ui_Label, LV_SIZE_CONTENT);
        lv_obj_set_x(ui_Label, lab_area[i][0]);
        lv_obj_set_y(ui_Label, lab_area[i][1]);
        lv_obj_set_align(ui_Label, LV_ALIGN_CENTER);
        lv_label_set_text(ui_Label, str[i]);
        lv_obj_set_style_text_color(ui_Label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(ui_Label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(ui_Label, &ui_font_MyFont34, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    lv_group_focus_obj(lv_obj_get_child(ui_SettingScreen, 0));
}

static void event_handler(lv_event_t* event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t* target = lv_event_get_target(event);
    lv_obj_t* parent = lv_obj_get_parent(target);
    if (LV_EVENT_KEY == code) {
        uint32_t value = lv_indev_get_key(lv_indev_get_act());
        lv_group_t* group = (lv_group_t*)lv_obj_get_group(target);
        uint32_t index = lv_obj_get_index(target);
        switch (value)
        {
        case LV_KEY_LEFT:
            lv_group_focus_prev(group);
            break;
        case LV_KEY_RIGHT:
            lv_group_focus_next(group);
            break;
        case LV_KEY_UP:
            switch (index)
            {
            case Setting:
            case Update:
            case ProjectorInfo:
                lv_group_focus_obj(lv_obj_get_child(parent, index - 3));
                break;

            default:
                break;
            }
            break;
        case LV_KEY_DOWN:
            switch (index)
            {
            case Projector:
            case Picture:
            case Sound:
                lv_group_focus_obj(lv_obj_get_child(parent, index + 3));
                break;

            default:
                break;
            }
            break;
        case LV_KEY_ENTER:
            switch (index)
            {
            case NetWork:
                break;

            case Projector:
                break;

            case Picture:
                break;

            default:
                break;
            }
            break;
        case LV_KEY_ESC:
            delete_group(SettingGroup);
            lv_obj_del_async(ui_SettingScreen);
        break;

        default:
            break;
        }
    }
}