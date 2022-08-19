/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-08-15 13:36:10
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-08-19 21:53:41
 * @FilePath: /gui/application/setting/NetWork.cpp
 * @Description: None
 * @other: None
 */
#include <string.h>
#include "NetWork.h"
#include "application/NetWorkApi.h"

static lv_obj_t* NetWorkPanel;
static lv_obj_t* WiFiPanel;
static lv_obj_t* ui_ConnectedPanel;
static lv_obj_t* ui_AvailablePanel;
static lv_obj_t* ConnectPanel;
static lv_obj_t* PwdArea;
static lv_obj_t* Checkbox;
static lv_obj_t* Keyboard;
static lv_group_t* MainGroup;
static char password[21];

enum {
    NetWork_WiFi,
    NetWork_Bluetooth
};

enum {
    WiFiPanel_Switch = 0,
    WiFiPanel_Refresh,
    WiFiPanel_Add,
    WiFiPanel_ConnectedPanel = 4
};

enum {
    ConnectPanel_CodeArea = 1,
    ConnectPanel_Checkbox,
    ConnectPanel_Ok,
    ConnectPanel_Cancel
};

void CreateWiFiPanel(lv_obj_t* parent);
void CreateConnectPanel(lv_obj_t* parent);

void CreateNetWorkPanel(lv_obj_t* parent)
{
    auto event_cb = [] (lv_event_t* event) {
        lv_obj_t* target = lv_event_get_target(event);
        //lv_obj_t* parent = lv_obj_get_parent(target);
        uint32_t value = lv_indev_get_key(lv_indev_get_act());
        lv_group_t* group = get_activity_group();
        int index = lv_obj_get_index(target);
        switch (value)
        {
            case LV_KEY_UP:
                lv_group_focus_prev(group);
                break;
            case LV_KEY_DOWN:
                lv_group_focus_next(group);
                break;
            case LV_KEY_ENTER:
                switch (index)
                {
                case NetWork_WiFi:
                    CreateWiFiPanel(NetWorkPanel);
                    break;
                case NetWork_Bluetooth:
                    
                    break;
                
                default:
                    break;
                }
                break;
            case LV_KEY_ESC:
                delete_group(group);
                lv_obj_del_async(NetWorkPanel);
                break;
            default:
                break;
        }
    };

    NetWorkPanel = lv_obj_create(parent);
    lv_obj_set_width(NetWorkPanel, 1280);
    lv_obj_set_height(NetWorkPanel, 720);
    lv_obj_center(NetWorkPanel);
    lv_obj_clear_flag(NetWorkPanel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_color(NetWorkPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(NetWorkPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(NetWorkPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    MainGroup = create_new_group();
    set_group_activity(MainGroup);
    lv_obj_t* obj = lv_obj_create(NetWorkPanel);
    lv_obj_set_width(obj, 800);
    lv_obj_set_height(obj, 90);
    lv_obj_set_x(obj, 10);
    lv_obj_set_y(obj, -119);
    lv_obj_set_align(obj, LV_ALIGN_CENTER);
    lv_obj_add_state(obj, LV_STATE_FOCUSED);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(obj, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x61B09D), LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_group_add_obj(MainGroup, obj);
    lv_obj_add_event_cb(obj, event_cb, LV_EVENT_KEY, nullptr);

    lv_obj_t* img = lv_img_create(obj);
    lv_img_set_src(img, &ui_img_wifi_free_64_png);
    lv_obj_set_width(img, LV_SIZE_CONTENT);
    lv_obj_set_height(img, LV_SIZE_CONTENT);
    lv_obj_set_x(img, -200);
    lv_obj_set_y(img, 0);
    lv_obj_set_align(img, LV_ALIGN_CENTER);
    lv_obj_add_flag(img, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(img, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* lab = lv_label_create(obj);
    lv_obj_set_width(lab, LV_SIZE_CONTENT);
    lv_obj_set_height(lab, LV_SIZE_CONTENT);
    lv_obj_set_x(lab, 250);
    lv_obj_set_y(lab, 0);
    lv_obj_set_align(lab, LV_ALIGN_LEFT_MID);
    lv_label_set_text(lab, "WiFi Setting");
    lv_obj_set_style_text_font(lab, &ui_font_MyFont38, LV_PART_MAIN | LV_STATE_DEFAULT);

    obj = lv_obj_create(NetWorkPanel);
    lv_obj_set_width(obj, 800);
    lv_obj_set_height(obj, 90);
    lv_obj_set_x(obj, 14);
    lv_obj_set_y(obj, 10);
    lv_obj_set_align(obj, LV_ALIGN_CENTER);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(obj, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x61B09D), LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_group_add_obj(MainGroup, obj);
    lv_obj_add_event_cb(obj, event_cb, LV_EVENT_KEY, nullptr);

    img = lv_img_create(obj);
    lv_img_set_src(img, &ui_img_bluetooth_64_png);
    lv_obj_set_width(img, LV_SIZE_CONTENT);
    lv_obj_set_height(img, LV_SIZE_CONTENT);
    lv_obj_set_x(img, -200);
    lv_obj_set_y(img, 0);
    lv_obj_set_align(img, LV_ALIGN_CENTER);
    lv_obj_add_flag(img, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(img, LV_OBJ_FLAG_SCROLLABLE);

    lab = lv_label_create(obj);
    lv_obj_set_width(lab, LV_SIZE_CONTENT);
    lv_obj_set_height(lab, LV_SIZE_CONTENT);
    lv_obj_set_x(lab, 250);
    lv_obj_set_y(lab, 0);
    lv_obj_set_align(lab, LV_ALIGN_LEFT_MID);
    lv_label_set_text(lab, "Bluetooth Setting");
    lv_obj_set_style_text_align(lab, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lab, &ui_font_MyFont38, LV_PART_MAIN | LV_STATE_DEFAULT);
}

void CreateWiFiPanel(lv_obj_t* parent)
{
    auto event_cb = [] (lv_event_t* event) {
        lv_obj_t* target = lv_event_get_target(event);
        lv_obj_t* parent = lv_obj_get_parent(target);
        uint32_t value = lv_indev_get_key(lv_indev_get_act());
        lv_group_t* group = get_activity_group();
        int index = lv_obj_get_index(target);
        switch (value)
        {
            case LV_KEY_UP:
                if (parent == WiFiPanel) {
                    if (index == WiFiPanel_ConnectedPanel)
                        lv_group_focus_obj(lv_obj_get_child(WiFiPanel, WiFiPanel_Switch));
                    else if (index != WiFiPanel_Switch)
                        lv_group_focus_prev(group);
                }
                else
                    lv_group_focus_prev(group);
                break;
            case LV_KEY_DOWN:
                if (parent == WiFiPanel) {
                    switch (index)
                    {
                    case WiFiPanel_Switch:
                    case WiFiPanel_Refresh:
                    case WiFiPanel_Add:
                        lv_group_focus_obj(lv_obj_get_child(WiFiPanel, WiFiPanel_ConnectedPanel));
                        break;
                    
                    default:
                        lv_group_focus_next(group);
                        break;
                    }
                }
                else if (parent == ui_AvailablePanel) {
                    if (index != (lv_obj_get_child_cnt(ui_AvailablePanel) - 1))
                        lv_group_focus_next(group);
                }
                break;
            case LV_KEY_LEFT:
                if (!(parent == WiFiPanel && index == WiFiPanel_Switch))
                    lv_group_focus_prev(group);
                break;
            case LV_KEY_RIGHT:
                if (!(parent == ui_AvailablePanel && (index == (lv_obj_get_child_cnt(ui_AvailablePanel) - 1))))
                    lv_group_focus_next(group);
                break;
            case LV_KEY_ENTER:
                if (WiFiPanel == lv_obj_get_parent(target)) {
                    switch (index)
                    {
                    case WiFiPanel_Switch:
                        if (lv_obj_has_state(target->spec_attr->children[0], LV_STATE_CHECKED))
                            lv_obj_clear_state(target->spec_attr->children[0], LV_STATE_CHECKED);
                        else
                            lv_obj_add_state(target->spec_attr->children[0], LV_STATE_CHECKED);
                        break;
                    case WiFiPanel_Refresh:
                        break;
                    case WiFiPanel_Add:
                        break;
                    case WiFiPanel_ConnectedPanel:
                        break;
                    default:
                        break;
                    }
                }
                else if (ui_AvailablePanel == lv_obj_get_parent(target)) {
                    CreateConnectPanel(NetWorkPanel);
                }
                break;
            case LV_KEY_ESC:
                delete_group(group);
                lv_obj_del_async(WiFiPanel);
                break;
            default:
                break;
        }
    };
    
    WiFiPanel = lv_obj_create(parent);
    lv_obj_set_width(WiFiPanel, 1280);
    lv_obj_set_height(WiFiPanel, 720);
    lv_obj_set_x(WiFiPanel, 0);
    lv_obj_set_y(WiFiPanel, 0);
    lv_obj_set_align(WiFiPanel, LV_ALIGN_CENTER);
    lv_obj_clear_flag(WiFiPanel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_color(WiFiPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(WiFiPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(WiFiPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    MainGroup = create_new_group();
    set_group_activity(MainGroup);

    lv_obj_t* ui_NetWorkSwitch = lv_obj_create(WiFiPanel);
    lv_obj_set_width(ui_NetWorkSwitch, 114);
    lv_obj_set_height(ui_NetWorkSwitch, 64);
    lv_obj_set_x(ui_NetWorkSwitch, -445);
    lv_obj_set_y(ui_NetWorkSwitch, -300);
    lv_obj_set_align(ui_NetWorkSwitch, LV_ALIGN_CENTER);
    lv_obj_add_state(ui_NetWorkSwitch, LV_STATE_FOCUSED);
    lv_obj_clear_flag(ui_NetWorkSwitch, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_color(ui_NetWorkSwitch, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_NetWorkSwitch, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_NetWorkSwitch, 20, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_border_color(ui_NetWorkSwitch, lv_color_hex(0xEA9253), LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_border_opa(ui_NetWorkSwitch, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui_NetWorkSwitch, 5, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_group_add_obj(MainGroup, ui_NetWorkSwitch);
    lv_obj_add_event_cb(ui_NetWorkSwitch, event_cb, LV_EVENT_KEY, nullptr);

    lv_obj_t* ui_Switch = lv_switch_create(ui_NetWorkSwitch);
    lv_obj_set_width(ui_Switch, 100);
    lv_obj_set_height(ui_Switch, 50);
    lv_obj_set_x(ui_Switch, 0);
    lv_obj_set_y(ui_Switch, 0);
    lv_obj_set_align(ui_Switch, LV_ALIGN_CENTER);

    lv_obj_t* ui_NetWorkRefresh = lv_img_create(WiFiPanel);
    lv_img_set_src(ui_NetWorkRefresh, &ui_img_refresh_png);
    lv_obj_set_width(ui_NetWorkRefresh, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_NetWorkRefresh, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_NetWorkRefresh, 334);
    lv_obj_set_y(ui_NetWorkRefresh, -300);
    lv_obj_set_align(ui_NetWorkRefresh, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_NetWorkRefresh, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_NetWorkRefresh, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_NetWorkRefresh, 30, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_border_color(ui_NetWorkRefresh, lv_color_hex(0xEA9253), LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_border_opa(ui_NetWorkRefresh, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui_NetWorkRefresh, 5, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_group_add_obj(MainGroup, ui_NetWorkRefresh);
    lv_obj_add_event_cb(ui_NetWorkRefresh, event_cb, LV_EVENT_KEY, nullptr);

    lv_obj_t* ui_NetWorkAdd = lv_img_create(WiFiPanel);
    lv_img_set_src(ui_NetWorkAdd, &ui_img_plus_png);
    lv_obj_set_width(ui_NetWorkAdd, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_NetWorkAdd, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_NetWorkAdd, 420);
    lv_obj_set_y(ui_NetWorkAdd, -300);
    lv_obj_set_align(ui_NetWorkAdd, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_NetWorkAdd, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_NetWorkAdd, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_NetWorkAdd, 30, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_border_color(ui_NetWorkAdd, lv_color_hex(0xEA9253), LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_border_opa(ui_NetWorkAdd, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui_NetWorkAdd, 5, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_group_add_obj(MainGroup, ui_NetWorkAdd);
    lv_obj_add_event_cb(ui_NetWorkAdd, event_cb, LV_EVENT_KEY, nullptr);

    lv_obj_t* text1 = lv_label_create(WiFiPanel);
    lv_obj_set_width(text1, LV_SIZE_CONTENT);
    lv_obj_set_height(text1, LV_SIZE_CONTENT);
    lv_obj_set_x(text1, -416);
    lv_obj_set_y(text1, -245);
    lv_obj_set_align(text1, LV_ALIGN_CENTER);
    lv_label_set_text(text1, "已连接WLAN");
    lv_obj_set_style_text_font(text1, &ui_font_MyFont26, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_ConnectedPanel = lv_obj_create(WiFiPanel);
    lv_obj_set_width(ui_ConnectedPanel, 980);
    lv_obj_set_height(ui_ConnectedPanel, 70);
    lv_obj_set_x(ui_ConnectedPanel, 0);
    lv_obj_set_y(ui_ConnectedPanel, -180);
    lv_obj_set_align(ui_ConnectedPanel, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_ConnectedPanel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_ConnectedPanel, lv_color_hex(0xEA9253), LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_bg_opa(ui_ConnectedPanel, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_group_add_obj(MainGroup, ui_ConnectedPanel);
    lv_obj_add_event_cb(ui_ConnectedPanel, event_cb, LV_EVENT_KEY, nullptr);

    lv_obj_t* WifiName = lv_label_create(ui_ConnectedPanel);
    lv_obj_set_width(WifiName, 600);
    lv_obj_set_height(WifiName, LV_SIZE_CONTENT);
    lv_obj_set_x(WifiName, -170);
    lv_obj_set_y(WifiName, 0);
    lv_obj_set_align(WifiName, LV_ALIGN_CENTER);
    lv_label_set_text(WifiName, "HONOR");
    lv_obj_set_style_text_color(WifiName, lv_color_hex(0x231AAB), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(WifiName, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(WifiName, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(WifiName, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* WifiIcon = lv_img_create(ui_ConnectedPanel);
    lv_img_set_src(WifiIcon, &ui_img_wifi_64_png);
    lv_obj_set_width(WifiIcon, LV_SIZE_CONTENT);
    lv_obj_set_height(WifiIcon, LV_SIZE_CONTENT);
    lv_obj_set_x(WifiIcon, 430);
    lv_obj_set_y(WifiIcon, 0);
    lv_obj_set_align(WifiIcon, LV_ALIGN_CENTER);
    lv_obj_add_flag(WifiIcon, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(WifiIcon, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* text3 = lv_label_create(WiFiPanel);
    lv_obj_set_width(text3, LV_SIZE_CONTENT);
    lv_obj_set_height(text3, LV_SIZE_CONTENT);
    lv_obj_set_x(text3, -416);
    lv_obj_set_y(text3, -105);
    lv_obj_set_align(text3, LV_ALIGN_CENTER);
    lv_label_set_text(text3, "可用WLAN");
    lv_obj_set_style_text_font(text3, &ui_font_MyFont26, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_AvailablePanel = lv_obj_create(WiFiPanel);
    lv_obj_set_width(ui_AvailablePanel, 1020);
    lv_obj_set_height(ui_AvailablePanel, 425);
    lv_obj_set_x(ui_AvailablePanel, 0);
    lv_obj_set_y(ui_AvailablePanel, 120);
    lv_obj_set_align(ui_AvailablePanel, LV_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(ui_AvailablePanel,LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_border_opa(ui_AvailablePanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_flex_flow(ui_AvailablePanel, LV_FLEX_FLOW_COLUMN);

    #ifdef HCCHIP_GCC
    wifi_ap_info_t *wifi_list = nullptr;
    int ap_count = 0;
    WiFi_Scan();
    WiFi_GetAPList(&wifi_list, &ap_count);
    for(int i = 0; i < ap_count; ++i) {
    #else
    for(int i = 0; i < 10; ++i) {
    #endif
        #ifdef HCCHIP_GCC
        if (wifi_list[i].ssid[0] == '\0')
            continue;
        #endif
        lv_obj_t* AvailableSubPanel = lv_obj_create(ui_AvailablePanel);
        lv_obj_set_width(AvailableSubPanel, 980);
        lv_obj_set_height(AvailableSubPanel, 70);
        lv_obj_set_x(AvailableSubPanel, 0);
        lv_obj_set_y(AvailableSubPanel, -170 + 75 * i);
        lv_obj_set_align(AvailableSubPanel, LV_ALIGN_CENTER);
        lv_obj_clear_flag(AvailableSubPanel, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_flag(AvailableSubPanel, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
        lv_obj_set_style_bg_color(AvailableSubPanel, lv_color_hex(0xEA9253), LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_bg_opa(AvailableSubPanel, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_group_add_obj(MainGroup, AvailableSubPanel);
        lv_obj_add_event_cb(AvailableSubPanel, event_cb, LV_EVENT_KEY, nullptr);

        lv_obj_t* text5 = lv_label_create(AvailableSubPanel);
        lv_obj_set_width(text5, 600);
        lv_obj_set_height(text5, LV_SIZE_CONTENT);
        lv_obj_set_x(text5, -170);
        lv_obj_set_y(text5, 0);
        lv_obj_set_align(text5, LV_ALIGN_CENTER);
        #ifdef HCCHIP_GCC
        lv_label_set_text(text5, wifi_list[i].ssid);
        #else
        lv_label_set_text(text5, "HONOR");
        #endif
        lv_obj_set_style_text_font(text5, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_t* ui_Image8 = lv_img_create(AvailableSubPanel);
        lv_img_set_src(ui_Image8, &ui_img_wifi_64_png);
        lv_obj_set_width(ui_Image8, LV_SIZE_CONTENT);
        lv_obj_set_height(ui_Image8, LV_SIZE_CONTENT);
        lv_obj_set_x(ui_Image8, 430);
        lv_obj_set_y(ui_Image8, 0);
        lv_obj_set_align(ui_Image8, LV_ALIGN_CENTER);
        lv_obj_add_flag(ui_Image8, LV_OBJ_FLAG_ADV_HITTEST);
        lv_obj_clear_flag(ui_Image8, LV_OBJ_FLAG_SCROLLABLE);
    }
}

void CreateInputCodePanel(lv_obj_t* parent)
{
    MainGroup = create_new_group();
    set_group_activity(MainGroup);
    Keyboard = lv_keyboard_create(parent);
    lv_keyboard_set_mode(Keyboard, LV_KEYBOARD_MODE_TEXT_LOWER);
    lv_obj_set_width(Keyboard, 1130);
    lv_obj_set_height(Keyboard, 410);
    lv_obj_set_x(Keyboard, 0);
    lv_obj_set_y(Keyboard, 120);
    lv_obj_set_align(Keyboard, LV_ALIGN_CENTER);
    lv_obj_set_style_text_font(Keyboard, &lv_font_montserrat_30, LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_group_add_obj(MainGroup, Keyboard);
    lv_obj_add_event_cb(Keyboard, [] (lv_event_t* event) {
        uint32_t value = lv_indev_get_key(lv_indev_get_act());
        if (value == LV_KEY_ESC) {
            delete_group(MainGroup);
            lv_obj_del_async(Keyboard);
            lv_obj_set_y(ConnectPanel, 0);//-150
        }
    }, LV_EVENT_KEY, nullptr);
    lv_keyboard_set_textarea(Keyboard, PwdArea);
    //lv_group_focus_obj(Keyboard);
}

void CreateConnectPanel(lv_obj_t* parent)
{
    auto event_cb = [] (lv_event_t* event) {
        lv_event_code_t code = lv_event_get_code(event);
        lv_obj_t* target = lv_event_get_target(event);
        //lv_obj_t* parent = lv_obj_get_parent(target);
        lv_group_t* group = get_activity_group();
        if(code == LV_EVENT_KEY) {
            uint32_t value = lv_indev_get_key(lv_indev_get_act());
            int index = lv_obj_get_index(target);
            switch (value)
            {
                case LV_KEY_UP:
                    if (target != lv_obj_get_child(ConnectPanel, 1))
                        lv_group_focus_prev(group);
                    break;
                case LV_KEY_DOWN:
                    if (target != lv_obj_get_child(ConnectPanel, lv_obj_get_child_cnt(ConnectPanel) - 1))
                        lv_group_focus_next(group);
                    break;
                case LV_KEY_LEFT:
                    if (target == lv_obj_get_child(ConnectPanel, lv_obj_get_child_cnt(ConnectPanel) - 1))
                        lv_group_focus_prev(group);
                    break;
                case LV_KEY_RIGHT:
                    if (target == lv_obj_get_child(ConnectPanel, lv_obj_get_child_cnt(ConnectPanel) - 2))
                        lv_group_focus_next(group);
                    break;
                case LV_KEY_ENTER:
                    switch (index)
                    {
                    case ConnectPanel_CodeArea://
                        lv_obj_set_y(ConnectPanel, -150);//-150
                        CreateInputCodePanel(NetWorkPanel);
                        break;
                    case ConnectPanel_Checkbox:

                        break;
                    case ConnectPanel_Ok:

                        break;
                    case ConnectPanel_Cancel:
                        delete_group(group);
                        lv_obj_del_async(ConnectPanel);
                        break;
                    default:
                        break;
                    }
                    break;
                case LV_KEY_ESC:
                    delete_group(group);
                    lv_obj_del_async(ConnectPanel);
                    break;
                default:
                    break;
            }
        }
        else if (code == LV_EVENT_READY) {
            if (target == PwdArea) {
                if (lv_obj_is_valid(Keyboard)) {
                    memcpy(password, lv_textarea_get_text(PwdArea),20);
                    delete_group(group);
                    lv_obj_del_async(Keyboard);
                    lv_obj_set_y(ConnectPanel, 0);//-150
                }
            }
        }
    };

    MainGroup = create_new_group();
    set_group_activity(MainGroup);

    ConnectPanel = lv_obj_create(parent);
    lv_obj_set_width(ConnectPanel, 900);
    lv_obj_set_height(ConnectPanel, 400);
    lv_obj_set_x(ConnectPanel, 0);
    lv_obj_set_y(ConnectPanel, 0);//-150
    lv_obj_set_align(ConnectPanel, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ConnectPanel, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* WiFiName = lv_label_create(ConnectPanel);
    lv_obj_set_width(WiFiName, LV_SIZE_CONTENT);
    lv_obj_set_height(WiFiName, LV_SIZE_CONTENT);
    lv_obj_set_x(WiFiName, 0);
    lv_obj_set_y(WiFiName, -135);
    lv_obj_set_align(WiFiName, LV_ALIGN_CENTER);
    lv_label_set_text(WiFiName, "HONOR");
    lv_obj_set_style_text_font(WiFiName, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);

    PwdArea = lv_textarea_create(ConnectPanel);
    lv_obj_set_width(PwdArea, 600);
    lv_obj_set_height(PwdArea, 50);
    lv_obj_set_x(PwdArea, 0);
    lv_obj_set_y(PwdArea, -50);
    lv_obj_set_align(PwdArea, LV_ALIGN_CENTER);
    lv_textarea_set_text(PwdArea, "");
    lv_textarea_set_password_mode(PwdArea, true);
    lv_textarea_set_one_line(PwdArea, true);
    if("" == "") lv_textarea_set_accepted_chars(PwdArea, NULL);
    else lv_textarea_set_accepted_chars(PwdArea, "");
    lv_textarea_set_text(PwdArea, "");
    lv_textarea_set_placeholder_text(PwdArea, "code");
    lv_group_add_obj(MainGroup, PwdArea);
    lv_obj_add_event_cb(PwdArea, event_cb, LV_EVENT_ALL, nullptr);

    Checkbox = lv_checkbox_create(ConnectPanel);
    lv_checkbox_set_text(Checkbox, "show code");
    lv_obj_set_width(Checkbox, LV_SIZE_CONTENT);
    lv_obj_set_height(Checkbox, LV_SIZE_CONTENT);
    lv_obj_set_x(Checkbox, -200);
    lv_obj_set_y(Checkbox, 0);
    lv_obj_set_align(Checkbox, LV_ALIGN_CENTER);
    lv_obj_add_flag(Checkbox, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_set_style_text_font(Checkbox, &ui_font_MyFont26, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_group_add_obj(MainGroup, Checkbox);
    lv_obj_add_event_cb(Checkbox, event_cb, LV_EVENT_KEY, nullptr);

    lv_obj_t* ConnectButton = lv_btn_create(ConnectPanel);
    lv_obj_set_width(ConnectButton, 100);
    lv_obj_set_height(ConnectButton, 50);
    lv_obj_set_x(ConnectButton, -180);
    lv_obj_set_y(ConnectButton, 125);
    lv_obj_set_align(ConnectButton, LV_ALIGN_CENTER);
    lv_obj_add_flag(ConnectButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ConnectButton, LV_OBJ_FLAG_SCROLLABLE);
    lv_group_add_obj(MainGroup, ConnectButton);
    lv_obj_add_event_cb(ConnectButton, event_cb, LV_EVENT_KEY, nullptr);

    lv_obj_t* ConnectLab = lv_label_create(ConnectButton);
    lv_obj_set_width(ConnectLab, LV_SIZE_CONTENT);
    lv_obj_set_height(ConnectLab, LV_SIZE_CONTENT);
    lv_obj_set_x(ConnectLab, 0);
    lv_obj_set_y(ConnectLab, 0);
    lv_obj_set_align(ConnectLab, LV_ALIGN_CENTER);
    lv_label_set_text(ConnectLab, "连接");
    lv_obj_set_style_text_font(ConnectLab, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* CancelButton = lv_btn_create(ConnectPanel);
    lv_obj_set_width(CancelButton, 100);
    lv_obj_set_height(CancelButton, 50);
    lv_obj_set_x(CancelButton, 180);
    lv_obj_set_y(CancelButton, 125);
    lv_obj_set_align(CancelButton, LV_ALIGN_CENTER);
    lv_obj_add_flag(CancelButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(CancelButton, LV_OBJ_FLAG_SCROLLABLE);
    lv_group_add_obj(MainGroup, CancelButton);
    lv_obj_add_event_cb(CancelButton, event_cb, LV_EVENT_KEY, nullptr);

    lv_obj_t* CancelLab = lv_label_create(CancelButton);
    lv_obj_set_width(CancelLab, LV_SIZE_CONTENT);
    lv_obj_set_height(CancelLab, LV_SIZE_CONTENT);
    lv_obj_set_x(CancelLab, 0);
    lv_obj_set_y(CancelLab, 0);
    lv_obj_set_align(CancelLab, LV_ALIGN_CENTER);
    lv_label_set_text(CancelLab, "取消");
    lv_obj_set_style_text_font(CancelLab, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);
}