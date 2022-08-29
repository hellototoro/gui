/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-05-23 13:51:24
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-08-29 09:01:00
 * @FilePath: /gui/application/ui/HomeScreen.c
 * @Description: None
 * @other: None
 */
#include <unistd.h>
#include <stdio.h>
#include "HomeScreen.h"
#include "application/windows.h"
#include "Volume.h"
#include "application/key_map.h"
#include "SettingScreen.h"
#include "HdmiRx.h"
#include "SyncScreen.h"
#include "application/DetectHotPlug.h"

static lv_obj_t* HomeRootScreen;
static lv_obj_t* MainPanel;
static lv_obj_t* UdiskDetectPanel;
static lv_obj_t* SourcePanel;
static lv_obj_t* SourceTypeImg;
static lv_obj_t* SourceTypeLab;
static lv_group_t* MainGroup;
static int LastFocusedObjIndex;

enum CategoryList {
    HOME_MOVIE,
    HOME_MUSIC,
    HOME_PICTURE,
    HOME_MIRRORING,
    HOME_UDISK,
    HOME_HDMI,
    HOME_SETTING,
    CategoryNumber
};

enum SourceList {
    SourceUDisk,
    SourceHDMI,
    SourceListNumber
};

static const lv_img_dsc_t* source_img_src[] = {
    &ui_img_usb2_big_png,
    &ui_img_hdmi_big_png
};

static void CreateMainPanel(lv_obj_t* parent);
static void CreateSourcePanel(lv_obj_t* parent);
static void ExitHome(ActiveScreen screen);

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
            case HOME_UDISK:
            case HOME_HDMI:
                lv_group_focus_obj(lv_obj_get_child(parent, HOME_PICTURE));
                break;
            case HOME_PICTURE:
                lv_group_focus_obj(lv_obj_get_child(parent, HOME_MOVIE));
                break;
            case HOME_SETTING:
                lv_group_focus_obj(lv_obj_get_child(parent, HOME_MIRRORING));
                break;
            case HOME_MIRRORING:
                lv_group_focus_obj(lv_obj_get_child(parent, HOME_MUSIC));
                break;
            default:
                break;
            }
            break;
        case LV_KEY_DOWN:
            switch (lv_obj_get_index(target))
            {
            case HOME_MOVIE:
                lv_group_focus_obj(lv_obj_get_child(parent, HOME_PICTURE));
                break;
            case HOME_PICTURE:
                lv_group_focus_obj(lv_obj_get_child(parent, HOME_UDISK));
                break;
            case HOME_MUSIC:
                lv_group_focus_obj(lv_obj_get_child(parent, HOME_MIRRORING));
                break;
            case HOME_MIRRORING:
                lv_group_focus_obj(lv_obj_get_child(parent, HOME_SETTING));
                break;
            default:
                break;
            }
            break;
        case LV_KEY_ENTER:
            switch (lv_obj_get_index(target))
            {
            case HOME_UDISK:
                ExitHome(MediaScreen);
                break;
            case HOME_SETTING:
                CreateSettingScreen(HomeRootScreen);
                //ExitHome(SettingScreen);
                break;
            //case Source:
                //CreateSourcePanel(HomeRootScreen);
                //break;
            case HOME_MIRRORING:
                CreateSyncScreen(HomeRootScreen);
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
}

static void source_event_handler(lv_event_t* event)
{
    lv_obj_t* target = lv_event_get_target(event);
    uint32_t value = lv_indev_get_key(lv_indev_get_act());
    lv_group_t* group = (lv_group_t*)lv_obj_get_group(target);
    switch (value)
    {
    case LV_KEY_ENTER:
        switch (lv_obj_get_index(target))
        {
        case SourceUDisk:
            ExitHome(MediaScreen);
            break;
         case SourceHDMI:
            //CreateHdmiRxScreen();
            break;
       
        default:
            break;
        }
        return;
    case LV_KEY_LEFT:
        lv_group_focus_prev(group);
        break;
    case LV_KEY_RIGHT:
        lv_group_focus_next(group);
        break;
    case LV_KEY_ESC:
        MainGroup = delete_group(MainGroup);
        lv_obj_del_async(SourcePanel);
        return;
    default:
        break;
    }
    int index = lv_obj_get_index(lv_group_get_focused(group));
    lv_img_set_src(SourceTypeImg, source_img_src[index]);
    if(index == 0)
        SourceTypeLab->user_data = (void*)"source_p_u_disk";
    else if(index == 1)
        SourceTypeLab->user_data = (void*)"source_p_hdim";
    lv_label_set_text(SourceTypeLab, _(SourceTypeLab->user_data));
}

static void CreateMainPanel(lv_obj_t* parent)
{
    static const lv_coord_t img_area[CategoryNumber][4] = {//{ x, y, w, h}
        { -220, -180, 401, 167},
        {  220, -180, 401, 167},
        { -220,   20, 401, 167},
        {  220,   20, 401, 167},
        { -292,  220, 254, 164},
        {    0,  220, 254, 164},
        {  292,  220, 254, 164} };
    static const lv_coord_t lab_area[CategoryNumber][2] = {//{ x, y}
        { -90,  0},
        { -90,  0},
        { -90,  0},
        { -90,  0},
        {   0, 50},
        {   0, 50},
        {   0, 50} };
    static const char* str[CategoryNumber] = {
        "home_movie", 
        "home_music", 
        "home_picture", 
        "home_mirroring", 
        "home_udisk", 
        "home_hdmi", 
        "home_setting" };
    static const lv_img_dsc_t* image_src[] = {
        & ui_img_home_new_movie_png,
        & ui_img_home_new_music_png,
        & ui_img_home_new_photo_png,
        & ui_img_home_new_sync_screen_png,
        & ui_img_home_new_udisk_png,
        & ui_img_home_new_hdmi_in_png,
        & ui_img_home_new_setting_png };

    MainPanel = lv_obj_create(parent);
    lv_obj_set_size(MainPanel, 1280, 720);
    lv_obj_set_pos(MainPanel, 0, 0);
    lv_obj_set_align(MainPanel, LV_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(MainPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(MainPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    CreateDetectTask();
    UdiskDetectPanel = lv_obj_create(parent);
    lv_obj_set_width(UdiskDetectPanel, 189);
    lv_obj_set_height(UdiskDetectPanel, 83);
    lv_obj_set_x(UdiskDetectPanel, 516);
    lv_obj_set_y(UdiskDetectPanel, -313);
    lv_obj_set_align(UdiskDetectPanel, LV_ALIGN_CENTER);
    lv_obj_clear_flag(UdiskDetectPanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(UdiskDetectPanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(UdiskDetectPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(UdiskDetectPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(UdiskDetectPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* img = lv_img_create(UdiskDetectPanel);
    lv_img_set_src(img, &ui_img_usb_png);
    lv_obj_set_width(img, LV_SIZE_CONTENT);   /// 50
    lv_obj_set_height(img, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_x(img, 30);
    lv_obj_set_y(img, 0);
    lv_obj_set_align(img, LV_ALIGN_CENTER);
    lv_obj_add_flag(img, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(img, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_img_set_angle(img, 900);

    lv_obj_t* lab = lv_label_create(UdiskDetectPanel);
    lv_obj_set_width(lab, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(lab, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(lab, -40);
    lv_obj_set_y(lab, 0);
    lv_obj_set_align(lab, LV_ALIGN_CENTER);
    lv_label_set_text(lab, "U盘");
    lv_obj_set_style_text_color(lab, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(lab, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lab, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);

    MainGroup = create_new_group();
    set_group_activity(MainGroup);
    for (int i = 0; i < CategoryNumber; i++) {
        lv_obj_t* lv_obj = lv_obj_create(MainPanel);
        lv_obj_set_size(lv_obj, img_area[i][2], img_area[i][3]);
        lv_obj_set_pos(lv_obj, img_area[i][0], img_area[i][1]);
        lv_obj_set_align(lv_obj, LV_ALIGN_CENTER);
        lv_obj_clear_flag(lv_obj, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_radius(lv_obj, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(lv_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_opa(lv_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_opa(lv_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_img_src(lv_obj, image_src[i], LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(lv_obj, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_border_opa(lv_obj, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_border_width(lv_obj, 5, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_group_add_obj(MainGroup, lv_obj);
        lv_obj_add_event_cb(lv_obj, event_handler, LV_EVENT_KEY, NULL);

        lv_obj_t* lv_lab = lv_label_create(lv_obj);
        lv_obj_set_size(lv_lab, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_pos(lv_lab, lab_area[i][0], lab_area[i][1]);
        lv_obj_set_align(lv_lab, LV_ALIGN_CENTER);
        lv_obj_clear_flag(lv_lab, LV_OBJ_FLAG_SCROLLABLE);
        lv_lab->user_data = (void*)str[i];
        lv_label_set_text(lv_lab, _(str[i]));
        lv_obj_set_style_text_color(lv_lab, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(lv_lab, &ui_font_MyFont38, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    lv_group_focus_obj(lv_obj_get_child(MainPanel, LastFocusedObjIndex));
}

static void CreateSourcePanel(lv_obj_t* parent)
{
    SourcePanel = lv_obj_create(parent);
    lv_obj_set_size(SourcePanel, 1280, 720);
    lv_obj_clear_flag(SourcePanel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(SourcePanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(SourcePanel, lv_color_hex(0x3500FE), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(SourcePanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(SourcePanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    MainGroup = create_new_group();
    set_group_activity(MainGroup);

    lv_obj_t* btn = lv_btn_create(SourcePanel);
    lv_obj_set_size(btn, 113, 107);
    lv_obj_set_pos(btn, -87, 160);
    lv_obj_set_align(btn, LV_ALIGN_CENTER);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(btn, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(btn, &ui_img_usb2_n_png, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(btn, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_border_opa(btn, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(btn, 5, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_group_add_obj(MainGroup, btn);
    lv_obj_add_event_cb(btn, source_event_handler, LV_EVENT_KEY, NULL);

    btn = lv_btn_create(SourcePanel);
    lv_obj_set_size(btn, 113, 107);
    lv_obj_set_pos(btn, 87, 160);
    lv_obj_set_align(btn, LV_ALIGN_CENTER);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(btn, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(btn, &ui_img_hdmi_n_png, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(btn, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_border_opa(btn, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(btn, 5, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_group_add_obj(MainGroup, btn);
    lv_obj_add_event_cb(btn, source_event_handler, LV_EVENT_KEY, NULL);

    SourceTypeImg = lv_img_create(SourcePanel);
    lv_img_set_src(SourceTypeImg, &ui_img_usb2_big_png);
    lv_obj_set_size(SourceTypeImg, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_pos(SourceTypeImg, 0, -80);
    lv_obj_set_align(SourceTypeImg, LV_ALIGN_CENTER);
    lv_obj_add_flag(SourceTypeImg, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(SourceTypeImg, LV_OBJ_FLAG_SCROLLABLE);

    SourceTypeLab = lv_label_create(SourceTypeImg);
    lv_obj_set_size(SourceTypeLab, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_pos(SourceTypeLab, 0, 84);
    lv_obj_set_align(SourceTypeLab, LV_ALIGN_CENTER);
    SourceTypeLab->user_data = "source_p_u_disk";
    lv_label_set_text(SourceTypeLab, _(SourceTypeLab->user_data));
    lv_obj_set_style_text_color(SourceTypeLab, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(SourceTypeLab, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(SourceTypeLab, &ui_font_MyFont38, LV_PART_MAIN | LV_STATE_DEFAULT);
}

static void HomeInit(void)
{
    HomeRootScreen = lv_obj_create(NULL);
    lv_obj_set_size(HomeRootScreen, 1280, 720);
    lv_obj_set_style_bg_color(HomeRootScreen, lv_color_hex(0x3200FE), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(HomeRootScreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    CreateMainPanel(HomeRootScreen);
}

static void LoadHome(void)
{
    lv_scr_load_anim(HomeRootScreen, LV_SCR_LOAD_ANIM_NONE, 0, 0, true);
    //lv_scr_load_anim(HomeRootScreen, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0, true);
}

static void ExitHome(ActiveScreen screen)
{
    LastFocusedObjIndex = lv_obj_get_index(lv_group_get_focused(MainGroup));
    delete_all_group();
    //lv_obj_del_async(HomeRootScreen);
    CurrentScreen = screen;
}

window HomeWindow = {
    .ScreenInit = HomeInit,
    .ScreenLoad = LoadHome,
};
