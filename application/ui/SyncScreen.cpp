/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-08-28 21:18:24
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-09-11 06:18:01
 * @FilePath: /gui/application/ui/SyncScreen.cpp
 * @Description: None
 * @other: None
 */
#include <stdio.h>
#include "SyncScreen.h"
#include "LanguageScreen.h"
#ifdef HCCHIP_GCC
#include "hcscreen/cast_api.h"
#include "hcscreen/data_mgr.h"
#endif

static lv_obj_t* SyncRootScreen;
static lv_obj_t* CastPanel;
static lv_obj_t* CastQr;
static lv_group_t* MainGroup;

typedef enum {
    SyncScreen_Miracast = 1,
    SyncScreen_Airplay
} SyncScreenType_t;

static void CreateMiracastPanel(lv_obj_t* parent, SyncScreenType_t CastType);
#if 0//def HCCHIP_GCC
static volatile int m_first_flag = 1;
static void SyncScreenOpen(void);
static void win_cast_connect_state_upate(bool force_station);
#endif

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
                    CreateMiracastPanel(SyncRootScreen, static_cast<SyncScreenType_t>(index));
                    break;
                case SyncScreen_Airplay:
                    CreateMiracastPanel(SyncRootScreen, static_cast<SyncScreenType_t>(index));
                    break;
                default:
                    break;
                }
                break;
            case LV_KEY_ESC:
                delete_group(group);
                lv_obj_del_async(SyncRootScreen);
                break;
            default:
                break;
        }
    };

    SyncRootScreen = lv_obj_create(parent);
    lv_obj_set_size(SyncRootScreen, 1280, 720);
    lv_obj_set_style_radius(SyncRootScreen, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(SyncRootScreen, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    lv_obj_t* img = lv_img_create(SyncRootScreen);
    lv_img_set_src(img, root_img_src[DefaultLanguageIndex]);
    lv_obj_set_width(img, LV_SIZE_CONTENT);
    lv_obj_set_height(img, LV_SIZE_CONTENT); 
    lv_obj_set_x(img, 0);
    lv_obj_set_y(img, 100);
    lv_obj_set_align(img, LV_ALIGN_TOP_MID);
    lv_obj_add_flag(img, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(img, LV_OBJ_FLAG_SCROLLABLE);

    MainGroup = create_new_group();
    set_group_activity(MainGroup);

    lv_obj_t* MiracastObj = lv_obj_create(SyncRootScreen);
    lv_obj_set_width(MiracastObj, 300);
    lv_obj_set_height(MiracastObj, 309);
    lv_obj_set_x(MiracastObj, -250);
    lv_obj_set_y(MiracastObj, 60);
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
    lv_obj_set_width(MircastLab, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(MircastLab, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(MircastLab, 0);
    lv_obj_set_y(MircastLab, 100);
    lv_obj_set_align(MircastLab, LV_ALIGN_CENTER);
    lv_label_set_text(MircastLab, _("cast_mira"));
    lv_obj_set_style_text_color(MircastLab, lv_color_hex(0x0084FF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(MircastLab, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(MircastLab, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* AirplayObj = lv_obj_create(SyncRootScreen);
    lv_obj_set_width(AirplayObj, 300);
    lv_obj_set_height(AirplayObj, 309);
    lv_obj_set_x(AirplayObj, 250);
    lv_obj_set_y(AirplayObj, 60);
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
    lv_obj_set_width(AirplayLab, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(AirplayLab, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(AirplayLab, 0);
    lv_obj_set_y(AirplayLab, 100);
    lv_obj_set_align(AirplayLab, LV_ALIGN_CENTER);
    lv_label_set_text(AirplayLab, _("cast_airplay"));
    lv_obj_set_style_text_color(AirplayLab, lv_color_hex(0x0084FF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(AirplayLab, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(AirplayLab, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_group_focus_obj(lv_obj_get_child(SyncRootScreen, SyncScreen_Miracast));
}

static void CreateMiracastPanel(lv_obj_t* parent, SyncScreenType_t CastType)
{
    MainGroup = create_new_group();
    set_group_activity(MainGroup);
    CastPanel = lv_obj_create(parent);
    lv_obj_set_size(CastPanel, 1280, 720);
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
                delete_group(MainGroup);
                lv_obj_del_async(CastPanel);
                break;
            default:
                break;
        }
    }, LV_EVENT_KEY, nullptr);

    lv_obj_t* WifiName1 = lv_label_create(CastPanel);
    lv_obj_set_width(WifiName1, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(WifiName1, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(WifiName1, LV_ALIGN_LEFT_MID);
    #ifdef HCCHIP_GCC
    lv_label_set_text(WifiName1, data_mgr_get_device_name());
    #else
    lv_label_set_text(WifiName1, "Project_3A4D");
    #endif
    lv_obj_set_style_text_font(WifiName1, &ui_font_MyFont26, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* img = lv_img_create(CastPanel);
    lv_obj_set_width(img, LV_SIZE_CONTENT);
    lv_obj_set_height(img, LV_SIZE_CONTENT); 
    lv_obj_set_x(img, 1040);
    lv_obj_set_y(img, 135);
    lv_obj_set_align(img, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(img, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(img, LV_OBJ_FLAG_SCROLLABLE); 

    if(CastType == SyncScreen_Miracast) {
        lv_obj_set_style_bg_img_src(CastPanel, &AndroidCast, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_x(WifiName1, 540);
        lv_obj_set_y(WifiName1, -55);
        lv_img_set_src(img, &Android_QR);

        img = lv_img_create(CastPanel);
        lv_img_set_src(img, mira_img_src[DefaultLanguageIndex]);
        lv_obj_set_width(img, LV_SIZE_CONTENT);
        lv_obj_set_height(img, LV_SIZE_CONTENT); 
        lv_obj_set_x(img, 10);
        lv_obj_set_y(img, 20);
        lv_obj_set_align(img, LV_ALIGN_TOP_LEFT);
        lv_obj_add_flag(img, LV_OBJ_FLAG_ADV_HITTEST);
        lv_obj_clear_flag(img, LV_OBJ_FLAG_SCROLLABLE);

        lv_obj_t* text1_lab = lv_label_create(CastPanel);
        lv_obj_set_width(text1_lab, 780);   /// 1
        lv_obj_set_height(text1_lab, LV_SIZE_CONTENT);    /// 1
        lv_obj_set_x(text1_lab, 90);
        lv_obj_set_y(text1_lab, 410);
        lv_obj_set_align(text1_lab, LV_ALIGN_TOP_LEFT);

        #ifdef HCCHIP_GCC
        lv_label_set_text_fmt(text1_lab, "%s%s", _("cast_mira_str1"), data_mgr_get_device_name());
        #else
        lv_label_set_text_fmt(text1_lab, "%s%s", _("cast_mira_str1"), "Project_3A4D");
        #endif
        lv_obj_set_style_text_letter_space(text1_lab, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_line_space(text1_lab, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(text1_lab, &ui_font_MyFont26, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_t* text2_lab = lv_label_create(CastPanel);
        lv_obj_set_width(text2_lab, 1280);   /// 1
        lv_obj_set_height(text2_lab, LV_SIZE_CONTENT);    /// 1
        lv_obj_set_x(text2_lab, -10);
        lv_obj_set_y(text2_lab, 580);
        lv_obj_set_align(text2_lab, LV_ALIGN_TOP_LEFT);
        lv_label_set_text(text2_lab, _("cast_mira_str2"));
        lv_obj_set_style_text_letter_space(text2_lab, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_line_space(text2_lab, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(text2_lab, &ui_font_MyFont26, LV_PART_MAIN | LV_STATE_DEFAULT);
        #if 0//def HCCHIP_GCC
        hccast_mira_service_init(hccast_mira_callback_func);
        hccast_mira_service_start();
        #endif
    }
    else {
        lv_obj_set_style_bg_img_src(CastPanel, &iOSCast, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_x(WifiName1, 130);
        lv_obj_set_y(WifiName1, -116);
        lv_img_set_src(img, &iOS_QR);

        img = lv_img_create(CastPanel);
        lv_img_set_src(img, airplay_img_src[DefaultLanguageIndex]);
        lv_obj_set_width(img, LV_SIZE_CONTENT);
        lv_obj_set_height(img, LV_SIZE_CONTENT); 
        lv_obj_set_x(img, 10);
        lv_obj_set_y(img, 20);
        lv_obj_set_align(img, LV_ALIGN_TOP_LEFT);
        lv_obj_add_flag(img, LV_OBJ_FLAG_ADV_HITTEST);
        lv_obj_clear_flag(img, LV_OBJ_FLAG_SCROLLABLE);

        lv_obj_t* WifiName2 = lv_label_create(CastPanel);
        lv_obj_set_width(WifiName2, LV_SIZE_CONTENT);   /// 1
        lv_obj_set_height(WifiName2, LV_SIZE_CONTENT);    /// 1
        lv_obj_set_x(WifiName2, 945);
        lv_obj_set_y(WifiName2, -90);
        lv_obj_set_align(WifiName2, LV_ALIGN_LEFT_MID);
        #ifdef HCCHIP_GCC
        lv_label_set_text(WifiName2, data_mgr_get_device_name());
        #else
        lv_label_set_text(WifiName2, "Project_3A4D");
        #endif
        lv_obj_set_style_text_font(WifiName2, &ui_font_MyFont26, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_t* text1_lab = lv_label_create(CastPanel);
        lv_obj_set_width(text1_lab, 880);   /// 1
        lv_obj_set_height(text1_lab, LV_SIZE_CONTENT);    /// 1
        lv_obj_set_x(text1_lab, 90);
        lv_obj_set_y(text1_lab, 410);
        lv_obj_set_align(text1_lab, LV_ALIGN_TOP_LEFT);
        #ifdef HCCHIP_GCC
        lv_label_set_text_fmt(text1_lab, "%s%s", _("cast_airplay_str1"), data_mgr_get_device_name());
        #else
        lv_label_set_text_fmt(text1_lab, "%s%s", _("cast_airplay_str1"), "Project_3A4D");
        #endif
        //lv_label_set_text(text1_lab, _("cast_airplay_str1"));
        lv_obj_set_style_text_letter_space(text1_lab, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_line_space(text1_lab, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(text1_lab, &ui_font_MyFont26, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_t* text2_lab = lv_label_create(CastPanel);
        lv_obj_set_width(text2_lab, 780);   /// 1
        lv_obj_set_height(text2_lab, LV_SIZE_CONTENT);    /// 1
        lv_obj_set_x(text2_lab, 90);
        lv_obj_set_y(text2_lab, 445);
        lv_obj_set_align(text2_lab, LV_ALIGN_TOP_LEFT);
        #ifdef HCCHIP_GCC
        lv_label_set_text_fmt(text2_lab, "%s%s", _("cast_airplay_str2"), data_mgr_get_device_name());
        #else
        lv_label_set_text_fmt(text2_lab, "%s%s", _("cast_airplay_str2"), "Project_3A4D");
        #endif
        //lv_label_set_text(text2_lab, _("cast_airplay_str2"));
        lv_obj_set_style_text_letter_space(text2_lab, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_line_space(text2_lab, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(text2_lab, &ui_font_MyFont26, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_t* text3_lab = lv_label_create(CastPanel);
        lv_obj_set_width(text3_lab, 1280);   /// 1
        lv_obj_set_height(text3_lab, LV_SIZE_CONTENT);    /// 1
        lv_obj_set_x(text3_lab, 60);
        lv_obj_set_y(text3_lab, 580);
        lv_obj_set_align(text3_lab, LV_ALIGN_TOP_LEFT);
        lv_label_set_text(text3_lab, _("cast_airplay_str3"));
        lv_obj_set_style_text_letter_space(text3_lab, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_line_space(text3_lab, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(text3_lab, &ui_font_MyFont26, LV_PART_MAIN | LV_STATE_DEFAULT);
        #if 0//def HCCHIP_GCC
        hccast_air_service_init(hccast_air_callback_event);
        hccast_air_service_start();
        #endif
    }
}

#if 0//def HCCHIP_GCC
static void active_connect_timer(bool active)
{
    if (NULL == m_connect_timer)
        m_connect_timer = lv_timer_create(connect_timer_cb, 800, NULL);

    if (active){
        lv_timer_resume(m_connect_timer);
    }
    else{
        lv_timer_pause(m_connect_timer);
        lv_obj_clear_flag(m_label_wifi_ssid, LV_OBJ_FLAG_HIDDEN);
    }
}
//The first time enter screen application, WiFi should be station mode
//if there is wifi ap information in data node.
static void win_cast_connect_state_upate(bool force_station)
{
    char show_txt[64] = {0};
    int station_mode = 0;

    active_connect_timer(false);
    lv_label_set_text(m_label_state_msg, "");
    if (!network_wifi_module_get()){
        win_cast_no_wifi_device_show();
        return;
    }
    //lv_label_set_text(m_label_local_ssid, data_mgr_get_device_name());
    lv_label_set_text_fmt(m_label_local_ssid, "SSID: %s", data_mgr_get_device_name());

    //lv_label_set_text(m_label_password, "12345678");
    lv_label_set_text_fmt(m_label_password, "Password: %s", data_mgr_get_device_psk());

    if (hccast_wifi_mgr_get_connect_status()){
        station_mode = 1;
    }else{
        if (m_first_flag){
            hccast_wifi_ap_info_t wifi_ap;
            if (data_mgr_wifi_ap_get(&wifi_ap)){
                station_mode = 1;
            }
        }
    }
    m_first_flag = 0;

    
    if (force_station)
        station_mode = 1;

    printf("%s(), line:%d, force_station:%d, station_mode:%d\n", __func__, __LINE__, force_station, station_mode);

    if (station_mode){
        //connect to wifi
        hccast_wifi_ap_info_t wifi_ap;
        if (data_mgr_wifi_ap_get(&wifi_ap)){
            lv_label_set_text_fmt(m_label_wifi_ssid, "%s %s", LV_SYMBOL_WIFI, wifi_ap.ssid);
        }

        char *local_ip = wifi_local_ip_get();
        sprintf(show_txt, "IP: %s", local_ip);
        if (local_ip[0]){
            lv_label_set_text_fmt(m_label_connect_state, "%s %s", LV_SYMBOL_OK, "Connected");
            lv_label_set_text_fmt(m_label_ip, "%s %s", LV_SYMBOL_HOME, show_txt);
            win_cast_update_qr_code(QR_CONFIG);
        }else{
            lv_label_set_text(m_label_ip, "");
            lv_label_set_text(m_label_state_msg, "WiFi connecting ...");
            active_connect_timer(true);
            lv_label_set_text_fmt(m_label_connect_state, "%s %s", LV_SYMBOL_CLOSE, "No connection");
            win_cast_update_qr_code(QR_CLEAR);
        }


        lv_label_set_text(m_label_wifi_mode, "WiFi mode: Station");


    }else{
        lv_label_set_text(m_label_wifi_ssid, "");
        lv_label_set_text(m_label_wifi_mode, "WiFi mode: AP");

        int connected_cnt = hostap_get_connect_count();
        if ( connected_cnt > 0){
        //AP mode, phone has connected.
            sprintf(show_txt, "IP: %s", HCCAST_HOSTAP_IP);
            //lv_label_set_text(m_label_ip, show_txt);
            lv_label_set_text_fmt(m_label_ip, "%s %s", LV_SYMBOL_HOME, show_txt);
            //lv_label_set_text(m_label_connect_state, "Connected");
            lv_label_set_text_fmt(m_label_connect_state, "%s [%d] %s", LV_SYMBOL_OK, 
                connected_cnt, "Connected");

            win_cast_update_qr_code(QR_SCAN_WIFI);
        }else{
            //lv_label_set_text(m_label_connect_state, "No connection");
            lv_label_set_text_fmt(m_label_connect_state, "%s %s", LV_SYMBOL_CLOSE, "No connection");


            win_cast_update_qr_code(QR_CONNECT_AP);
        }
    }

}

static void SyncScreenOpen(void)
{
    sys_data_t *sys_data;
    sys_data = data_mgr_sys_get();
    printf("%s(), line: %d!, fw_ver: 0x%x, product_id:%s\n", 
        __func__, __LINE__, sys_data->firmware_version, sys_data->product_id);
    //api_logo_show(NULL);
    win_cast_connect_state_upate(false);
}
#endif
