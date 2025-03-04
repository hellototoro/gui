/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-08-28 21:18:24
 * @FilePath: /gui/application/ui/SyncScreen.cpp
 * @Description: None
 * @other: None
 */
#include <stdio.h>
#include "SyncScreen.h"
#include "LanguageScreen.h"
#include "application/windows.h"

static lv_obj_t* SyncRootScreen;
static lv_obj_t* SyncSubScreen;
static lv_obj_t* CastPanel;
static lv_obj_t* CastQr;
static lv_obj_t* RouterStatusImg;
static lv_group_t* MainGroup;

typedef enum {
    SyncScreen_Miracast = 1,
    SyncScreen_Airplay
} SyncScreenType_t;

static void CreateCastPanel(lv_obj_t* parent, SyncScreenType_t CastType);
static void ExitSync(ActiveScreen screen);

static constexpr const lv_img_dsc_t* root_img_src[] = {
    &cast_es,
    &cast_de,
    &cast_en,
    &cast_it,
    &cast_fr,
    &cast_cn
};

static constexpr const lv_img_dsc_t* mira_img_src[] = {
    &cast_android_es,
    &cast_android_de,
    &cast_android_en,
    &cast_android_it,
    &cast_android_fr,
    &cast_android_cn
};

static constexpr const lv_img_dsc_t* airplay_img_src[] = {
    &cast_ios_es,
    &cast_ios_de,
    &cast_ios_en,
    &cast_ios_it,
    &cast_ios_fr,
    &cast_ios_cn
};

void CreateSyncScreen(lv_obj_t* parent)
{
    auto event_cb = [] (lv_event_t* event) {
        lv_obj_t* target = lv_event_get_current_target(event);
        //lv_obj_t* parent = lv_obj_get_parent(target);
        uint32_t value = lv_indev_get_key(lv_indev_get_act());
        lv_group_t* group = get_activity_group();
        int index = lv_obj_get_index(target);
        switch (value)
        {
            case LV_KEY_LEFT:
                lv_group_focus_prev(group);
                break;
            case LV_KEY_RIGHT:
                lv_group_focus_next(group);
                break;
            case LV_KEY_ENTER:
                switch (index)
                {
                case SyncScreen_Miracast:
                    CreateCastPanel(SyncSubScreen, static_cast<SyncScreenType_t>(index));
                    break;
                case SyncScreen_Airplay:
                    CreateCastPanel(SyncSubScreen, static_cast<SyncScreenType_t>(index));
                    break;
                default:
                    break;
                }
                break;
            case LV_KEY_ESC:
                ExitSync(HomeScreen);
                break;
            default:
                base_event_handler(event);
                break;
        }
    };

    SyncSubScreen = lv_obj_create(parent);
    lv_obj_set_size(SyncSubScreen, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_radius(SyncSubScreen, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(SyncSubScreen, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_border_width(SyncSubScreen, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(SyncSubScreen, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* img = lv_img_create(SyncSubScreen);
    lv_img_set_src(img, root_img_src[DefaultLanguageIndex]);
    lv_obj_set_size(img, LV_SIZE_CONTENT, LV_SIZE_CONTENT); 
    lv_obj_set_pos(img, 0, 100);
    lv_obj_set_align(img, LV_ALIGN_TOP_MID);
    lv_obj_add_flag(img, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(img, LV_OBJ_FLAG_SCROLLABLE);

    MainGroup = create_new_group();
    set_group_activity(MainGroup);

    lv_obj_t* MiracastObj = lv_obj_create(SyncSubScreen);
    lv_obj_set_size(MiracastObj, 300, 309);
    lv_obj_set_pos(MiracastObj, -250, 60);
    lv_obj_set_align(MiracastObj, LV_ALIGN_CENTER);
    lv_obj_clear_flag(MiracastObj, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(MiracastObj, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(MiracastObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(MiracastObj, &ui_img_home_new_mircast_n_png, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(MiracastObj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(MiracastObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(MiracastObj, &ui_img_home_new_mircast_f_png, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_group_add_obj(MainGroup, MiracastObj);
    lv_obj_add_event_cb(MiracastObj, event_cb, LV_EVENT_KEY, nullptr);
    lv_obj_add_event_cb(MiracastObj, [] (lv_event_t* event) { 
        lv_obj_t* target = lv_event_get_current_target(event);
        lv_obj_set_style_text_color(lv_obj_get_child(target, 0), lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    }, LV_EVENT_FOCUSED, nullptr);
    lv_obj_add_event_cb(MiracastObj, [] (lv_event_t* event) { 
        lv_obj_t* target = lv_event_get_current_target(event);
        lv_obj_set_style_text_color(lv_obj_get_child(target, 0), lv_color_hex(0x0084FF), LV_PART_MAIN | LV_STATE_DEFAULT);
    }, LV_EVENT_DEFOCUSED, nullptr);

    lv_obj_t* MircastLab = lv_label_create(MiracastObj);
    lv_obj_set_size(MircastLab, LV_SIZE_CONTENT, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_pos(MircastLab, 0, 100);
    lv_obj_set_align(MircastLab, LV_ALIGN_CENTER);
    MircastLab->user_data = (void*)"cast_mira";
    lv_label_set_text(MircastLab, _(static_cast<const char*>(MircastLab->user_data)));
    lv_obj_set_style_text_color(MircastLab, lv_color_hex(0x0084FF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(MircastLab, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(MircastLab, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* AirplayObj = lv_obj_create(SyncSubScreen);
    lv_obj_set_size(AirplayObj, 300, 309);
    lv_obj_set_pos(AirplayObj, 250, 60);
    lv_obj_set_align(AirplayObj, LV_ALIGN_CENTER);
    lv_obj_clear_flag(AirplayObj, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(AirplayObj, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(AirplayObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(AirplayObj, &ui_img_home_new_airplay_n_png, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(AirplayObj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(AirplayObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(AirplayObj, &ui_img_home_new_airplay_f_png, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_group_add_obj(MainGroup, AirplayObj);
    lv_obj_add_event_cb(AirplayObj, event_cb, LV_EVENT_KEY, nullptr);
    lv_obj_add_event_cb(AirplayObj, [] (lv_event_t* event) { 
        lv_obj_t* target = lv_event_get_current_target(event);
        lv_obj_set_style_text_color(lv_obj_get_child(target, 0), lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    }, LV_EVENT_FOCUSED, nullptr);
    lv_obj_add_event_cb(AirplayObj, [] (lv_event_t* event) { 
        lv_obj_t* target = lv_event_get_current_target(event);
        lv_obj_set_style_text_color(lv_obj_get_child(target, 0), lv_color_hex(0x0084FF), LV_PART_MAIN | LV_STATE_DEFAULT);
    }, LV_EVENT_DEFOCUSED, nullptr);

    lv_obj_t* AirplayLab = lv_label_create(AirplayObj);
    lv_obj_set_size(AirplayLab, LV_SIZE_CONTENT, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_pos(AirplayLab, 0, 100);
    lv_obj_set_align(AirplayLab, LV_ALIGN_CENTER);
    AirplayLab->user_data = (void*)"cast_airplay";
    lv_label_set_text(AirplayLab, _(static_cast<const char*>(AirplayLab->user_data)));
    lv_obj_set_style_text_color(AirplayLab, lv_color_hex(0x0084FF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(AirplayLab, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(AirplayLab, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_group_focus_obj(lv_obj_get_child(SyncSubScreen, SyncScreen_Miracast));
}

static void CreateCastPanel(lv_obj_t* parent, SyncScreenType_t CastType)
{
    MainGroup = create_new_group();
    set_group_activity(MainGroup);
    CastPanel = lv_obj_create(parent);
    lv_obj_set_size(CastPanel, LV_HOR_RES, LV_VER_RES);
    lv_obj_clear_flag(CastPanel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(CastPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(CastPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_center(CastPanel);
    lv_group_add_obj(MainGroup, CastPanel);
    lv_obj_add_event_cb(CastPanel, [] (lv_event_t* event) { 
        uint32_t value = lv_indev_get_key(lv_indev_get_act());
        switch (value)
        {
            case LV_KEY_ENTER:
                break;
            case LV_KEY_ESC:
                if (lv_obj_has_flag(SyncSubScreen, LV_OBJ_FLAG_HIDDEN)) 
                    lv_obj_clear_flag(SyncSubScreen, LV_OBJ_FLAG_HIDDEN);
                delete_group(MainGroup);
                lv_obj_del_async(CastPanel);
                break;
            default:
                base_event_handler(event);
                break;
        }
    }, LV_EVENT_KEY, nullptr);

    lv_obj_t* WifiName1 = lv_label_create(CastPanel);
    lv_obj_set_size(WifiName1, LV_SIZE_CONTENT, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(WifiName1, LV_ALIGN_LEFT_MID);
    //lv_label_set_text(WifiName1, data_mgr_get_device_name());
    lv_obj_set_style_text_font(WifiName1, &ui_font_MyFont26, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* img = lv_img_create(CastPanel);
    lv_obj_set_size(img, LV_SIZE_CONTENT, LV_SIZE_CONTENT); 
    lv_obj_set_pos(img, 1040, 135);
    lv_obj_set_align(img, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(img, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(img, LV_OBJ_FLAG_SCROLLABLE); 

    if(CastType == SyncScreen_Miracast) {
        lv_obj_set_style_bg_img_src(CastPanel, &AndroidCast, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_pos(WifiName1, 540, -55);
        lv_img_set_src(img, &Android_QR);

        img = lv_img_create(CastPanel);
        lv_img_set_src(img, mira_img_src[DefaultLanguageIndex]);
        lv_obj_set_size(img, LV_SIZE_CONTENT, LV_SIZE_CONTENT); 
        lv_obj_set_pos(img, 10, 20);
        lv_obj_set_align(img, LV_ALIGN_TOP_LEFT);
        lv_obj_add_flag(img, LV_OBJ_FLAG_ADV_HITTEST);
        lv_obj_clear_flag(img, LV_OBJ_FLAG_SCROLLABLE);

        lv_obj_t* text1_lab = lv_label_create(CastPanel);
        lv_obj_set_size(text1_lab, 780, LV_SIZE_CONTENT);    /// 1
        lv_obj_set_pos(text1_lab, 90, 410);
        lv_obj_set_align(text1_lab, LV_ALIGN_TOP_LEFT);

        text1_lab->user_data = (void*)"cast_mira_str1";
        lv_label_set_text_fmt(text1_lab, "%s%s", _(static_cast<const char*>(text1_lab->user_data)), "Project_3A4D");
        lv_obj_set_style_text_letter_space(text1_lab, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_line_space(text1_lab, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(text1_lab, &ui_font_MyFont26, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_t* text2_lab = lv_label_create(CastPanel);
        lv_obj_set_size(text2_lab, LV_HOR_RES, LV_SIZE_CONTENT);    /// 1
        lv_obj_set_pos(text2_lab, -10, 580);
        lv_obj_set_align(text2_lab, LV_ALIGN_TOP_LEFT);
        text2_lab->user_data = (void*)"cast_mira_str2";
        lv_label_set_text(text2_lab, _(static_cast<const char*>(text2_lab->user_data)));
        lv_obj_set_style_text_letter_space(text2_lab, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_line_space(text2_lab, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(text2_lab, &ui_font_MyFont26, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    else {
        lv_obj_set_style_bg_img_src(CastPanel, &iOSCast, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_pos(WifiName1, 130, -116);
        lv_img_set_src(img, &iOS_QR);

        img = lv_img_create(CastPanel);
        lv_img_set_src(img, airplay_img_src[DefaultLanguageIndex]);
        lv_obj_set_size(img, LV_SIZE_CONTENT, LV_SIZE_CONTENT); 
        lv_obj_set_pos(img, 10, 20);
        lv_obj_set_align(img, LV_ALIGN_TOP_LEFT);
        lv_obj_add_flag(img, LV_OBJ_FLAG_ADV_HITTEST);
        lv_obj_clear_flag(img, LV_OBJ_FLAG_SCROLLABLE);

        RouterStatusImg = lv_img_create(CastPanel);
        lv_img_set_src(RouterStatusImg, &router_disconnected);
        lv_obj_set_size(RouterStatusImg, LV_SIZE_CONTENT, LV_SIZE_CONTENT); 
        lv_obj_set_pos(RouterStatusImg, 900, 20);
        lv_obj_set_align(RouterStatusImg, LV_ALIGN_TOP_LEFT);
        lv_obj_add_flag(RouterStatusImg, LV_OBJ_FLAG_ADV_HITTEST);
        lv_obj_clear_flag(RouterStatusImg, LV_OBJ_FLAG_SCROLLABLE);

        lv_obj_t* WifiName2 = lv_label_create(CastPanel);
        lv_obj_set_size(WifiName2, LV_SIZE_CONTENT, LV_SIZE_CONTENT);    /// 1
        lv_obj_set_pos(WifiName2, 945, -90);
        lv_obj_set_align(WifiName2, LV_ALIGN_LEFT_MID);
        //lv_label_set_text(WifiName2, data_mgr_get_device_name());
        lv_obj_set_style_text_font(WifiName2, &ui_font_MyFont26, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_t* text1_lab = lv_label_create(CastPanel);
        lv_obj_set_size(text1_lab, 880, LV_SIZE_CONTENT);    /// 1
        lv_obj_set_pos(text1_lab, 90, 410);
        lv_obj_set_align(text1_lab, LV_ALIGN_TOP_LEFT);
        text1_lab->user_data = (void*)"cast_airplay_str1";
        lv_label_set_text_fmt(text1_lab, "%s%s", _(static_cast<const char*>(text1_lab->user_data)), "Project_3A4D");
        lv_obj_set_style_text_letter_space(text1_lab, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_line_space(text1_lab, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(text1_lab, &ui_font_MyFont26, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_t* text2_lab = lv_label_create(CastPanel);
        lv_obj_set_size(text2_lab, 780, LV_SIZE_CONTENT);    /// 1
        lv_obj_set_pos(text2_lab, 90, 445);
        lv_obj_set_align(text2_lab, LV_ALIGN_TOP_LEFT);
        text2_lab->user_data = (void*)"cast_airplay_str2";
        //lv_label_set_text_fmt(text2_lab, "%s%s", _(static_cast<const char*>(text2_lab->user_data)), data_mgr_get_device_name());
        lv_obj_set_style_text_letter_space(text2_lab, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_line_space(text2_lab, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(text2_lab, &ui_font_MyFont26, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_t* text3_lab = lv_label_create(CastPanel);
        lv_obj_set_size(text3_lab, LV_HOR_RES, LV_SIZE_CONTENT);    /// 1
        lv_obj_set_pos(text3_lab, 60, 580);
        lv_obj_set_align(text3_lab, LV_ALIGN_TOP_LEFT);
        text3_lab->user_data = (void*)"cast_airplay_str3";
        lv_label_set_text(text3_lab, _(static_cast<const char*>(text3_lab->user_data)));
        lv_obj_set_style_text_letter_space(text3_lab, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_line_space(text3_lab, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(text3_lab, &ui_font_MyFont26, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
}

static void SyncInit(void)
{
    SyncRootScreen = lv_obj_create(NULL);
    lv_obj_set_style_bg_opa(SyncRootScreen, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);

    CreateSyncScreen(SyncRootScreen);
}

static void LoadSync(void)
{
    lv_scr_load_anim(SyncRootScreen, LV_SCR_LOAD_ANIM_NONE, 0, 0, true);
    //lv_scr_load_anim(SyncRootScreen, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0, true);
}

static void ExitSync(ActiveScreen screen)
{
    CurrentScreen = screen;
    delete_all_group();
}

window SyncWindow = {
    .ScreenInit = SyncInit,
    .ScreenLoad = LoadSync,
};
