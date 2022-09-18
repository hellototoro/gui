/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-26 11:13:24
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-09-18 23:57:29
 * @FilePath: /gui/application/ui/SettingScreen.cpp
 * @Description: None
 * @other: None
 */
#include "SettingScreen.h"
#include "application/setting/Setting.h"
#include "application/ConfigParam.h"

static lv_obj_t* SettingRootScreen;
static lv_obj_t* MainPanel;
static lv_obj_t* SubPanel;
static lv_group_t* MainGroup;
static lv_timer_t* OsdTimer;

enum CategoryList {
    NetWork,
    Position,
    Picture,
    Sound,
    System,
    Update,
    Info,
    CategoryNumber
};

static void CreateSettingPanel(lv_obj_t* parent, lv_coord_t w, lv_coord_t h, void* setting_obj, bool IsMainPanel, bool BgTransp);
static void InitOsdTimer(int time);

void CreateSettingScreen(lv_obj_t* parent)
{
    auto event_cb = [] (lv_event_t* event) {
        lv_obj_t* target = lv_event_get_current_target(event);
        lv_obj_t* parent = lv_obj_get_parent(target);
        lv_group_t* group = (lv_group_t*)lv_obj_get_group(target);
        uint32_t index = lv_obj_get_index(target);
        uint32_t value = lv_indev_get_key(lv_indev_get_act());
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
            case System:
            case Update:
            case Info:
                lv_group_focus_obj(lv_obj_get_child(parent, index - 3));
                break;

            default:
                break;
            }
            break;
        case LV_KEY_DOWN:
            switch (index)
            {
            case Position:
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
                CreateNetWorkPanel(SettingRootScreen);
                break;

            case Position:
                break;

            case Picture:
                CreateSettingPanel(SettingRootScreen, 800, 90, new Setting::Picture, true, false);
                break;

            case Sound:
                CreateSettingPanel(SettingRootScreen, 800, 90, new Setting::Sound, true, false);
                break;

            case System:
                CreateSettingPanel(SettingRootScreen, 800, 90, new Setting::System, true, false);
                break;

            case Update:
                break;

            case Info:
                break;

            default:
                break;
            }
            break;
        case LV_KEY_ESC:
        case LV_KEY_MENU:
            delete_group(MainGroup);
            lv_obj_del_async(SettingRootScreen);
        break;

        default:
            break;
        }
        if (OsdTimer != nullptr) lv_timer_reset(OsdTimer);
    };

    static const lv_coord_t img_area[CategoryNumber][2] = {//{ x, y}
        {-330,    0},
        {-110, -102},
        { 110, -102},
        { 330, -102},
        {-110,  101},
        { 110,  101},
        { 330,  101} };
    static const lv_coord_t lab_area[CategoryNumber][2] = {//{ x, y}
        { 0, 75},
        { 0, 45},
        { 0, 45},
        { 0, 45},
        { 0, 45},
        { 0, 45},
        { 0, 45} };
    static const char* str[CategoryNumber] =  { 
    "setting_p_net_work", 
    "setting_p_projector", 
    "setting_p_picture", 
    "setting_p_sound", 
    "setting_p_system", 
    "setting_p_upgrade", 
    "setting_p_projector_info" };
    static const lv_img_dsc_t* image_src[CategoryNumber] = {
        & ui_img_network_setting_png,
        & ui_img_projector_setting_png,
        & ui_img_pic_setting_png,
        & ui_img_sound_setting_png,
        & ui_img_system_setting_png,
        & ui_img_update_setting_png,
        & ui_img_projector_info_png };

    SettingRootScreen = lv_obj_create(parent);
    lv_obj_set_size(SettingRootScreen, 1280, 720);
    lv_obj_clear_flag(SettingRootScreen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(SettingRootScreen, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(SettingRootScreen, lv_color_hex(0x3500FE), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(SettingRootScreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(SettingRootScreen, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    MainGroup = create_new_group();
    set_group_activity(MainGroup);
    for (int i = 0; i < CategoryNumber; i++) {
        lv_obj_t* img = lv_img_create(SettingRootScreen);
        lv_img_set_src(img, image_src[i]);
        lv_obj_set_width(img, LV_SIZE_CONTENT);
        lv_obj_set_height(img, LV_SIZE_CONTENT);
        lv_obj_set_pos(img, img_area[i][0], img_area[i][1]);
        lv_obj_set_align(img, LV_ALIGN_CENTER);
        lv_obj_add_flag(img, LV_OBJ_FLAG_ADV_HITTEST);
        lv_obj_clear_flag(img, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_radius(img, 20, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_border_color(img, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_border_opa(img, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_border_width(img, 5, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_group_add_obj(MainGroup, img);
        lv_obj_add_event_cb(img, event_cb, LV_EVENT_KEY, NULL);

        lv_obj_t* lab = lv_label_create(img);
        lv_obj_set_width(lab, LV_SIZE_CONTENT);
        lv_obj_set_height(lab, LV_SIZE_CONTENT);
        lv_obj_set_pos(lab, lab_area[i][0], lab_area[i][1]);
        lv_obj_set_align(lab, LV_ALIGN_CENTER);
        lab->user_data = const_cast<char*>(str[i]);
        lv_label_set_text(lab, _(str[i]));
        lv_obj_set_style_text_color(lab, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(lab, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(lab, &ui_font_MyFont34, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    lv_group_focus_obj(lv_obj_get_child(SettingRootScreen, 0));
    MainPanel = nullptr;
    SubPanel = nullptr;

    boost::property_tree::ptree config;
    ReadConfigFile(config, "system_setting");
    int OsdTime = config.get<int>("OsdTime", 0);
    if(OsdTime > 0) InitOsdTimer(OsdTime*5);
    else            OsdTimer = nullptr;
}

static void CreateSettingPanel(lv_obj_t* parent, lv_coord_t w, lv_coord_t h, void* setting_obj, bool IsMainPanel, bool BgTransp)
{
    auto event_cb = [] (lv_event_t* event) {
        lv_obj_t* target = lv_event_get_current_target(event);
        lv_obj_t* parent = lv_obj_get_parent(target);
        lv_group_t* group = (lv_group_t*)lv_obj_get_group(target);
        uint32_t index = lv_obj_get_index(target);
        uint32_t value = lv_indev_get_key(lv_indev_get_act());
        Setting::Base* Setting = static_cast<Setting::Base*>(parent->user_data);
        switch (value)
        {
        case LV_KEY_ENTER:
            if(Setting->GetDerivedAddress(index) != nullptr)
                CreateSettingPanel(MainPanel, 600, 70, Setting->GetDerivedAddress(index), false, false);
            else
                Setting->SelectedValue(index);
            break;
        case LV_KEY_LEFT:
            if (lv_obj_get_child_cnt(target) == 1) break;//只能响应enter键的控件
            Setting->DecreaseUserValue(index);
            target->spec_attr->children[1]->user_data = const_cast<char*>(Setting->GetStr(index));
            lv_label_set_text(target->spec_attr->children[1], _(Setting->GetStr(index)));
            if(lv_obj_is_valid(SubPanel)) {
                lv_slider_set_value(target->spec_attr->children[2], Setting->GetUserValue(index), LV_ANIM_OFF);
            }
            break;
        case LV_KEY_RIGHT:
            if (lv_obj_get_child_cnt(target) == 1) break;//只能响应enter键的控件
            Setting->IncreaseUserValue(index);
            target->spec_attr->children[1]->user_data = const_cast<char*>(Setting->GetStr(index));
            lv_label_set_text(target->spec_attr->children[1], _(Setting->GetStr(index)));
            if(lv_obj_is_valid(SubPanel)) {
                lv_slider_set_value(target->spec_attr->children[2], Setting->GetUserValue(index), LV_ANIM_OFF);
            }
            break;
        case LV_KEY_UP:
            lv_group_focus_prev(group);
            break;
        case LV_KEY_DOWN:
            lv_group_focus_next(group);
            break;
        case LV_KEY_ESC:
        case LV_KEY_MENU:
            MainGroup = delete_group(MainGroup);
            if (SubPanel != nullptr) {
                lv_obj_del_async(SubPanel);
                SubPanel = nullptr;
            }
            else {
                lv_obj_del_async(MainPanel);
                MainPanel = nullptr;
                delete Setting;
            }
            break;
        default:
            break;
        }
        if (OsdTimer != nullptr) lv_timer_reset(OsdTimer);
    };

    if (setting_obj == nullptr) return;
    Setting::Base* Setting = static_cast<Setting::Base*>(setting_obj);
    uint8_t ItemNum = Setting->GetItemNum();
    uint8_t only_text_item_num = Setting->GetOnlyTextItemNum();
    const char** text = Setting->GetStrArray();

    lv_obj_t* SettingPanel = lv_obj_create(parent);
    if (IsMainPanel) MainPanel = SettingPanel;
    else             SubPanel  = SettingPanel;
    lv_obj_set_size(SettingPanel, 1280, 720);
    lv_obj_clear_flag(SettingPanel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(SettingPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(SettingPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    if (BgTransp)
        lv_obj_set_style_bg_opa(SettingPanel, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    else
        lv_obj_set_style_bg_opa(SettingPanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(SettingPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_align(SettingPanel, LV_ALIGN_CENTER);

    lv_obj_t* SettingPanelLayout = lv_obj_create(SettingPanel);
    lv_obj_set_width(SettingPanelLayout, w);
    lv_obj_set_height(SettingPanelLayout, h * (ItemNum + 1));
    lv_obj_set_pos(SettingPanelLayout, 0, 0);
    lv_obj_set_align(SettingPanelLayout, LV_ALIGN_CENTER);
    lv_obj_clear_flag(SettingPanelLayout, LV_OBJ_FLAG_SCROLLABLE);
    SettingPanelLayout->user_data = setting_obj;

    lv_obj_t* title = lv_obj_create(SettingPanelLayout);
    lv_obj_set_width(title, w);
    lv_obj_set_height(title, h);
    lv_obj_set_pos(title, 0, -(h / 2) * ItemNum);
    lv_obj_set_align(title, LV_ALIGN_CENTER);
    lv_obj_clear_flag(title, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_color(title, lv_color_hex(0xBBBBBB), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(title, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(title, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(title, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* title_text = lv_label_create(title);
    lv_obj_set_width(title_text, LV_SIZE_CONTENT);
    lv_obj_set_height(title_text, LV_SIZE_CONTENT);
    lv_obj_set_pos(title_text, 0, 0);
    lv_obj_set_align(title_text, LV_ALIGN_CENTER);
    title_text->user_data = const_cast<char*>(text[0]);
    lv_label_set_text(title_text, _(text[0]));
    lv_obj_set_style_text_color(title_text, lv_color_hex(0x1438FF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(title_text, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(title_text, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);

    MainGroup = create_new_group();
    set_group_activity(MainGroup);
    for(int i = 0; i < ItemNum; ++i) {
        lv_obj_t* item_panel = lv_obj_create(SettingPanelLayout);
        lv_obj_set_width(item_panel, w);
        lv_obj_set_height(item_panel, h);
        lv_obj_set_pos(item_panel, 0, -(h / 2) * ItemNum + (i + 1) * h);
        lv_obj_set_align(item_panel, LV_ALIGN_CENTER);
        lv_obj_clear_flag(item_panel, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_border_color(item_panel, lv_color_hex(0xBBBBBB), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_opa(item_panel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(item_panel, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_side(item_panel, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(item_panel, lv_color_hex(0xEF16E9), LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_bg_opa(item_panel, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_group_add_obj(MainGroup, item_panel);
        lv_obj_add_event_cb(item_panel, event_cb, LV_EVENT_KEY, NULL);
        lv_obj_add_event_cb(item_panel, [] (lv_event_t* event) {
            lv_obj_t* target = lv_event_get_current_target(event);
            int count = target->spec_attr->child_cnt;
            for(int i = 0; i < count; ++i) {
                lv_obj_t* child = target->spec_attr->children[i];
                if(lv_obj_check_type(child, &lv_label_class))
                    lv_obj_set_style_text_color(child, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
                else if(lv_obj_check_type(child, &lv_img_class))
                    lv_obj_set_style_img_recolor(child, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
            }
        }, LV_EVENT_FOCUSED, nullptr);
        lv_obj_add_event_cb(item_panel, [] (lv_event_t* event) {
            lv_obj_t* target = lv_event_get_current_target(event);
            int count = target->spec_attr->child_cnt;
            for(int i = 0; i < count; ++i) {
                lv_obj_t* child = target->spec_attr->children[i];
                if(lv_obj_check_type(child, &lv_label_class))
                    lv_obj_set_style_text_color(child, lv_color_hex(0x1438FF), LV_PART_MAIN | LV_STATE_DEFAULT);
                else if(lv_obj_check_type(child, &lv_img_class))
                    lv_obj_set_style_img_recolor(child, lv_color_hex(0x1438FF), LV_PART_MAIN | LV_STATE_DEFAULT);
            }
        }, LV_EVENT_DEFOCUSED, nullptr);

        lv_obj_t* item_text1 = lv_label_create(item_panel);
        lv_obj_set_width(item_text1, LV_SIZE_CONTENT);
        lv_obj_set_height(item_text1, LV_SIZE_CONTENT);
        lv_obj_set_y(item_text1, 0);
        lv_obj_set_align(item_text1, LV_ALIGN_CENTER);
        lv_obj_set_style_text_color(item_text1, lv_color_hex(0x1438FF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(item_text1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(item_text1, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);
        if (i < ItemNum - only_text_item_num) {
            int index = 0;
            if (IsMainPanel) index = 2*i+1;
            else             index = i+1;
            item_text1->user_data = const_cast<char*>(text[index]);
            lv_label_set_text(item_text1, _(text[index]));
            lv_obj_set_x(item_text1, -200);
        }
        else {
            lv_obj_set_x(item_text1, 0);
            item_text1->user_data = const_cast<char*>(text[ItemNum - only_text_item_num + 1 + i]);
            lv_label_set_text(item_text1, _(text[ItemNum - only_text_item_num + 1 + i]));
            continue;
        }
        lv_obj_t* item_text2 = lv_label_create(item_panel);
        lv_obj_set_width(item_text2, LV_SIZE_CONTENT);
        lv_obj_set_height(item_text2, LV_SIZE_CONTENT);
        uint8_t mid = 0;
        if (IsMainPanel) mid = (100+320)/2;
        else             mid = 230;
        lv_obj_set_pos(item_text2, mid, 0);
        lv_obj_set_align(item_text2, LV_ALIGN_CENTER);
        const char* str = nullptr;
        if (IsMainPanel) str = text[2*i+2];
        else             str = Setting->GetStr(i+1);
        item_text2->user_data = const_cast<char*>(str);
        lv_label_set_text(item_text2, _(str));
        lv_obj_set_style_text_color(item_text2, lv_color_hex(0x1438FF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(item_text2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(item_text2, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);

        if (IsMainPanel) {
            lv_obj_t* item_image1 = lv_img_create(item_panel);
            lv_img_set_src(item_image1, &ui_img_menu_arrow_right32_png);
            lv_obj_set_width(item_image1, LV_SIZE_CONTENT);
            lv_obj_set_height(item_image1, LV_SIZE_CONTENT);
            lv_obj_set_pos(item_image1, 320, 0);
            lv_obj_set_align(item_image1, LV_ALIGN_CENTER);
            lv_obj_add_flag(item_image1, LV_OBJ_FLAG_ADV_HITTEST);
            lv_obj_clear_flag(item_image1, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_img_recolor(item_image1, lv_color_hex(0x1438FF), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_img_recolor_opa(item_image1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

            lv_obj_t* item_image2 = lv_img_create(item_panel);
            lv_img_set_src(item_image2, &ui_img_menu_arrow_left32_png);
            lv_obj_set_width(item_image2, LV_SIZE_CONTENT);
            lv_obj_set_height(item_image2, LV_SIZE_CONTENT);
            lv_obj_set_pos(item_image2, 100, 0);
            lv_obj_set_align(item_image2, LV_ALIGN_CENTER);
            lv_obj_add_flag(item_image2, LV_OBJ_FLAG_ADV_HITTEST);
            lv_obj_clear_flag(item_image2, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_img_recolor(item_image2, lv_color_hex(0x1438FF), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_img_recolor_opa(item_image2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        else {
            lv_obj_t* slider = lv_slider_create(item_panel);
            lv_slider_set_range(slider, 0, 100);
            lv_slider_set_value(slider, Setting->GetUserValue(i+1), LV_ANIM_OFF);
            if(lv_slider_get_mode(slider) == LV_SLIDER_MODE_RANGE) lv_slider_set_left_value(slider, 0, LV_ANIM_OFF);
            lv_obj_set_width(slider, 200);
            lv_obj_set_height(slider, 7);
            lv_obj_set_pos(slider, 80, 0);
            lv_obj_set_align(slider, LV_ALIGN_CENTER);
            lv_obj_clear_state(slider, LV_STATE_FOCUS_KEY);
        }
    }
    lv_group_focus_obj(lv_obj_get_child(SettingPanelLayout, 1));
}

static void InitOsdTimer(int time)
{
    OsdTimer = lv_timer_create([] (lv_timer_t * timer) {
        if (SubPanel != nullptr) {
            MainGroup = delete_group(MainGroup);
        }
        if (MainPanel != nullptr) {
            MainGroup = delete_group(MainGroup);
            lv_obj_t* SettingObj = lv_obj_get_child(MainPanel, 0);
            Setting::Base* Setting = static_cast<Setting::Base*>(SettingObj->user_data);
            delete Setting;
            SettingObj->user_data = nullptr;
        }
        delete_group(MainGroup);
        lv_obj_del_async(SettingRootScreen);
        lv_timer_del(timer);
        OsdTimer = nullptr;
    }, time*1000, NULL);
}

void SetOsdTime(int time)
{
    if (time == 0) {
        if (OsdTimer != nullptr) 
            lv_timer_del(OsdTimer);
        OsdTimer = nullptr;
    }
    else {
        if (OsdTimer == nullptr) 
            InitOsdTimer(time*5);
        else                     
            lv_timer_set_period(OsdTimer, time*1000*5);
    }
}
