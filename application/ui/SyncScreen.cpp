/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-08-28 21:18:24
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-08-28 21:51:12
 * @FilePath: /gui/application/ui/SyncScreen.cpp
 * @Description: None
 * @other: None
 */
#include "SyncScreen.h"

static lv_obj_t* SyncRootScreen;
static lv_obj_t* MircastPanel;
static lv_obj_t* AirplayPanel;
static lv_group_t* MainGroup;

enum {
    SyncScreen_Mircast,
    SyncScreen_Airplay
};

void CreateSyncScreen(lv_obj_t* parent)
{
    auto event_cb = [] (lv_event_t* event) {
        lv_obj_t* target = lv_event_get_target(event);
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
                case SyncScreen_Mircast:
                    break;
                case SyncScreen_Airplay:
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
    lv_obj_clear_flag(SyncRootScreen, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    lv_obj_t* lab = lv_label_create(SyncRootScreen);
    lv_obj_set_width(lab, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(lab, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(lab, 0);
    lv_obj_set_y(lab, -210);
    lv_obj_set_align(lab, LV_ALIGN_CENTER);
    lv_label_set_text(lab, "多屏互动");
    lv_obj_set_style_text_font(lab, &ui_font_MyFont38, LV_PART_MAIN | LV_STATE_DEFAULT);

    MainGroup = create_new_group();
    set_group_activity(MainGroup);

    MircastPanel = lv_obj_create(SyncRootScreen);
    lv_obj_set_width(MircastPanel, 300);
    lv_obj_set_height(MircastPanel, 309);
    lv_obj_set_x(MircastPanel, -250);
    lv_obj_set_y(MircastPanel, 60);
    lv_obj_set_align(MircastPanel, LV_ALIGN_CENTER);
    lv_obj_clear_flag(MircastPanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(MircastPanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(MircastPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(MircastPanel, &ui_img_home_new_mircast_n_png, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(MircastPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(MircastPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(MircastPanel, &ui_img_home_new_mircast_f_png, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_group_add_obj(MainGroup, MircastPanel);
    lv_obj_add_event_cb(MircastPanel, event_cb, LV_EVENT_KEY, nullptr);
    lv_obj_add_event_cb(MircastPanel, [] (lv_event_t* event) { 
        lv_obj_t* target = lv_event_get_target(event);
        lv_obj_set_style_text_color(lv_obj_get_child(target, 0), lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    }, LV_EVENT_FOCUSED, nullptr);
    lv_obj_add_event_cb(MircastPanel, [] (lv_event_t* event) { 
        lv_obj_t* target = lv_event_get_target(event);
        lv_obj_set_style_text_color(lv_obj_get_child(target, 0), lv_color_hex(0x0084FF), LV_PART_MAIN | LV_STATE_DEFAULT);
    }, LV_EVENT_DEFOCUSED, nullptr);

    lab = lv_label_create(MircastPanel);
    lv_obj_set_width(lab, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(lab, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(lab, 0);
    lv_obj_set_y(lab, 100);
    lv_obj_set_align(lab, LV_ALIGN_CENTER);
    lv_label_set_text(lab, "安卓同屏");
    lv_obj_set_style_text_color(lab, lv_color_hex(0x0084FF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(lab, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lab, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);

    AirplayPanel = lv_obj_create(SyncRootScreen);
    lv_obj_set_width(AirplayPanel, 300);
    lv_obj_set_height(AirplayPanel, 309);
    lv_obj_set_x(AirplayPanel, 250);
    lv_obj_set_y(AirplayPanel, 60);
    lv_obj_set_align(AirplayPanel, LV_ALIGN_CENTER);
    lv_obj_clear_flag(AirplayPanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(AirplayPanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(AirplayPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(AirplayPanel, &ui_img_home_new_airplay_n_png, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(AirplayPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(AirplayPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(AirplayPanel, &ui_img_home_new_airplay_f_png, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_group_add_obj(MainGroup, AirplayPanel);
    lv_obj_add_event_cb(AirplayPanel, event_cb, LV_EVENT_KEY, nullptr);
    lv_obj_add_event_cb(AirplayPanel, [] (lv_event_t* event) { 
        lv_obj_t* target = lv_event_get_target(event);
        lv_obj_set_style_text_color(lv_obj_get_child(target, 0), lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    }, LV_EVENT_FOCUSED, nullptr);
    lv_obj_add_event_cb(AirplayPanel, [] (lv_event_t* event) { 
        lv_obj_t* target = lv_event_get_target(event);
        lv_obj_set_style_text_color(lv_obj_get_child(target, 0), lv_color_hex(0x0084FF), LV_PART_MAIN | LV_STATE_DEFAULT);
    }, LV_EVENT_DEFOCUSED, nullptr);

    lab = lv_label_create(AirplayPanel);
    lv_obj_set_width(lab, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(lab, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(lab, 0);
    lv_obj_set_y(lab, 100);
    lv_obj_set_align(lab, LV_ALIGN_CENTER);
    lv_label_set_text(lab, "苹果同屏");
    lv_obj_set_style_text_color(lab, lv_color_hex(0x0084FF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(lab, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lab, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_group_focus_obj(lv_obj_get_child(SyncRootScreen, 1));
}