/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-08-15 13:36:10
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-08-15 16:21:31
 * @FilePath: /gui/application/setting/NetWork.cpp
 * @Description: None
 * @other: None
 */
#include "NetWork.h"

lv_obj_t* ui_NetWorkPanel;
lv_obj_t* ui_ConnectedPanel;
lv_obj_t* ui_AvailablePanel;

void CreateNetWorkPanel(lv_obj_t* parent)
{
    ui_NetWorkPanel = lv_obj_create(parent);
    lv_obj_set_width(ui_NetWorkPanel, 1280);
    lv_obj_set_height(ui_NetWorkPanel, 720);
    lv_obj_set_x(ui_NetWorkPanel, 0);
    lv_obj_set_y(ui_NetWorkPanel, 0);
    lv_obj_set_align(ui_NetWorkPanel, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_NetWorkPanel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_color(ui_NetWorkPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_NetWorkPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_NetWorkPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* text1 = lv_label_create(ui_NetWorkPanel);
    lv_obj_set_width(text1, LV_SIZE_CONTENT);
    lv_obj_set_height(text1, LV_SIZE_CONTENT);
    lv_obj_set_x(text1, -400);
    lv_obj_set_y(text1, -225);
    lv_obj_set_align(text1, LV_ALIGN_CENTER);
    lv_label_set_text(text1, "已连接WLAN");
    lv_obj_set_style_text_font(text1, &ui_font_MyFont26, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* ui_ConnectedPanel = lv_obj_create(ui_NetWorkPanel);
    lv_obj_set_width(ui_ConnectedPanel, 980);
    lv_obj_set_height(ui_ConnectedPanel, 70);
    lv_obj_set_x(ui_ConnectedPanel, 0);
    lv_obj_set_y(ui_ConnectedPanel, -170);
    lv_obj_set_align(ui_ConnectedPanel, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_ConnectedPanel, LV_OBJ_FLAG_SCROLLABLE);

    // WifiName
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

    // WifiIcon
    lv_obj_t* WifiIcon = lv_img_create(ui_ConnectedPanel);
    lv_img_set_src(WifiIcon, &ui_img_wifi_64_png);
    lv_obj_set_width(WifiIcon, LV_SIZE_CONTENT);
    lv_obj_set_height(WifiIcon, LV_SIZE_CONTENT);
    lv_obj_set_x(WifiIcon, 430);
    lv_obj_set_y(WifiIcon, 0);
    lv_obj_set_align(WifiIcon, LV_ALIGN_CENTER);
    lv_obj_add_flag(WifiIcon, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(WifiIcon, LV_OBJ_FLAG_SCROLLABLE);

    // ui_AvailablePanel
    ui_AvailablePanel = lv_obj_create(ui_NetWorkPanel);
    lv_obj_set_width(ui_AvailablePanel, 1000);
    lv_obj_set_height(ui_AvailablePanel, 500);
    lv_obj_set_x(ui_AvailablePanel, 0);
    lv_obj_set_y(ui_AvailablePanel, 120);
    lv_obj_set_align(ui_AvailablePanel, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_AvailablePanel, LV_OBJ_FLAG_SCROLLABLE);
    //lv_obj_set_style_border_opa(ui_AvailablePanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_flex_flow(ui_AvailablePanel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_AvailablePanel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // text3
    lv_obj_t* text3 = lv_label_create(ui_NetWorkPanel);
    lv_obj_set_width(text3, LV_SIZE_CONTENT);
    lv_obj_set_height(text3, LV_SIZE_CONTENT);
    lv_obj_set_x(text3, -410);
    lv_obj_set_y(text3, -95);
    lv_obj_set_align(text3, LV_ALIGN_CENTER);
    lv_label_set_text(text3, "可用WLAN");
    lv_obj_set_style_text_font(text3, &ui_font_MyFont26, LV_PART_MAIN | LV_STATE_DEFAULT);

    for(int i = 0; i < 6; ++i) {
        // AvailableSubPanel
        lv_obj_t* AvailableSubPanel = lv_obj_create(ui_AvailablePanel);
        lv_obj_set_width(AvailableSubPanel, 980);
        lv_obj_set_height(AvailableSubPanel, 70);
        //lv_obj_set_x(AvailableSubPanel, 0);
        //lv_obj_set_y(AvailableSubPanel, 70*i);
        lv_obj_set_align(AvailableSubPanel, LV_ALIGN_CENTER);
        lv_obj_clear_flag(AvailableSubPanel, LV_OBJ_FLAG_SCROLLABLE);
        //lv_obj_set_style_border_side(AvailableSubPanel, LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);

        // text5
        lv_obj_t* text5 = lv_label_create(AvailableSubPanel);
        lv_obj_set_width(text5, 600);
        lv_obj_set_height(text5, LV_SIZE_CONTENT);
        lv_obj_set_x(text5, -170);
        lv_obj_set_y(text5, 0);
        lv_obj_set_align(text5, LV_ALIGN_CENTER);
        lv_label_set_text(text5, "HONOR");
        lv_obj_set_style_text_font(text5, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);

        // ui_Image8
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

    lv_obj_t* ui_Keyboard = lv_keyboard_create(ui_NetWorkPanel);
    lv_keyboard_set_mode(ui_Keyboard, LV_KEYBOARD_MODE_USER_2);
    lv_obj_set_width(ui_Keyboard, 1130);
    lv_obj_set_height(ui_Keyboard, 411);
    lv_obj_set_x(ui_Keyboard, -28);
    lv_obj_set_y(ui_Keyboard, 43);
    lv_obj_set_align(ui_Keyboard, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Keyboard, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_text_font(ui_Keyboard, &ui_font_MyFont30, LV_PART_ITEMS | LV_STATE_DEFAULT);

    lv_obj_t* ui_NetWorkSwitch = lv_switch_create(ui_NetWorkPanel);
    lv_obj_set_width(ui_NetWorkSwitch, 101);
    lv_obj_set_height(ui_NetWorkSwitch, 49);
    lv_obj_set_x(ui_NetWorkSwitch, -445);
    lv_obj_set_y(ui_NetWorkSwitch, -280);
    lv_obj_set_align(ui_NetWorkSwitch, LV_ALIGN_CENTER);

    lv_obj_t* ui_NetWorkRefresh = lv_img_create(ui_NetWorkPanel);
    lv_img_set_src(ui_NetWorkRefresh, &ui_img_refresh_png);
    lv_obj_set_width(ui_NetWorkRefresh, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_NetWorkRefresh, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_NetWorkRefresh, 334);
    lv_obj_set_y(ui_NetWorkRefresh, -280);
    lv_obj_set_align(ui_NetWorkRefresh, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_NetWorkRefresh, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_NetWorkRefresh, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* ui_NetWorkAdd = lv_img_create(ui_NetWorkPanel);
    lv_img_set_src(ui_NetWorkAdd, &ui_img_plus_png);
    lv_obj_set_width(ui_NetWorkAdd, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_NetWorkAdd, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_NetWorkAdd, 421);
    lv_obj_set_y(ui_NetWorkAdd, -280);
    lv_obj_set_align(ui_NetWorkAdd, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_NetWorkAdd, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_NetWorkAdd, LV_OBJ_FLAG_SCROLLABLE);
}
