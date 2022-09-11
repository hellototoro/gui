/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-26 11:13:24
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-09-11 21:12:49
 * @FilePath: /gui/application/ui/SettingScreen.cpp
 * @Description: None
 * @other: None
 */
#include "SettingScreen.h"
#include "application/setting/Setting.h"

static lv_obj_t* SettingRootScreen;
static lv_obj_t* MainPanel;
static lv_obj_t* SubPanel;
static lv_group_t* MainGroup;
static lv_obj_t* LastFocused;
static lv_obj_t* SubLastFocused;

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

void CreateSettingPanel(void* user_data, bool BgTransp);
void CreateSubSettingPanel(void* user_data, bool BgTransp);

static void event_handler(lv_event_t* event)
{
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
            CreateSettingPanel(new Setting::Picture, false);
            break;

        case Sound:
            CreateSettingPanel(new Setting::Sound, false);
            break;

        case System:
            CreateSettingPanel(new Setting::System, false);
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
        delete_group(MainGroup);
        lv_obj_del_async(SettingRootScreen);
    break;

    default:
        break;
    }
}

static void setting_item_key_event_handler(lv_event_t* event)
{
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
            CreateSubSettingPanel(Setting->GetDerivedAddress(index), false);
        else
            Setting->SelectedValue(index);
        break;
    case LV_KEY_LEFT:
        if (lv_obj_get_child_cnt(target) == 1) {//只能响应enter键的控件
            break;
        }
        Setting->DecreaseUserValue(index);
        target->spec_attr->children[1]->user_data = const_cast<char*>(Setting->GetStr(index));
        lv_label_set_text(target->spec_attr->children[1], _(Setting->GetStr(index)));
        if(lv_obj_is_valid(SubPanel)) {
            lv_slider_set_value(target->spec_attr->children[2], Setting->GetUserValue(index), LV_ANIM_OFF);
        }
        break;
    case LV_KEY_RIGHT:
        if (lv_obj_get_child_cnt(target) == 1) {
            break;
        }
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
        if(lv_obj_is_valid(SubPanel)) {
            LastFocused = SubLastFocused;
            lv_obj_del_async(SubPanel);
        }
        else {
            lv_obj_del_async(MainPanel);
            delete Setting;
        }
        break;
    default:
        break;
    }
}

static void focused_handler(lv_group_t* group)
{
    if (LastFocused != NULL) {
        int count = LastFocused->spec_attr->child_cnt;
        for(int i = 0; i < count; ++i) {
            lv_obj_t* child = LastFocused->spec_attr->children[i];
            if(lv_obj_check_type(child, &lv_label_class))
                lv_obj_set_style_text_color(child, lv_color_hex(0x1438FF), LV_PART_MAIN | LV_STATE_DEFAULT);
            else if(lv_obj_check_type(child, &lv_img_class))
                lv_obj_set_style_img_recolor(child, lv_color_hex(0x1438FF), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
    lv_obj_t* obj = lv_group_get_focused(group);
    int count = obj->spec_attr->child_cnt;
    for(int i = 0; i < count; ++i) {
        lv_obj_t* child = obj->spec_attr->children[i];
        if(lv_obj_check_type(child, &lv_label_class))
            lv_obj_set_style_text_color(child, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
        else if(lv_obj_check_type(child, &lv_img_class))
            lv_obj_set_style_img_recolor(child, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    LastFocused = obj;
}

void CreateSettingScreen(lv_obj_t* parent)
{
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
        lv_obj_set_x(img, img_area[i][0]);
        lv_obj_set_y(img, img_area[i][1]);
        lv_obj_set_align(img, LV_ALIGN_CENTER);
        lv_obj_add_flag(img, LV_OBJ_FLAG_ADV_HITTEST);
        lv_obj_clear_flag(img, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_radius(img, 20, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_border_color(img, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_border_opa(img, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_border_width(img, 5, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_group_add_obj(MainGroup, img);
        lv_obj_add_event_cb(img, event_handler, LV_EVENT_KEY, NULL);

        lv_obj_t* lab = lv_label_create(img);
        lv_obj_set_width(lab, LV_SIZE_CONTENT);
        lv_obj_set_height(lab, LV_SIZE_CONTENT);
        lv_obj_set_x(lab, lab_area[i][0]);
        lv_obj_set_y(lab, lab_area[i][1]);
        lv_obj_set_align(lab, LV_ALIGN_CENTER);
        lab->user_data = const_cast<char*>(str[i]);
        lv_label_set_text(lab, _(str[i]));
        lv_obj_set_style_text_color(lab, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(lab, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(lab, &ui_font_MyFont34, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    lv_group_focus_obj(lv_obj_get_child(SettingRootScreen, 0));
}

void CreateSettingPanel(void* user_data, bool BgTransp)
{
    Setting::Base* Setting = static_cast<Setting::Base*>(user_data);
    uint8_t ItemNum = Setting->GetItemNum();
    uint8_t only_text_item_num = Setting->GetOnlyTextItemNum();
    const char** text = Setting->GetStrArray();

    MainPanel = lv_obj_create(lv_scr_act());
    lv_obj_set_size(MainPanel, 1280, 720);
    lv_obj_clear_flag(MainPanel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(MainPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(MainPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    if (BgTransp)
        lv_obj_set_style_bg_opa(MainPanel, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    else
        lv_obj_set_style_bg_opa(MainPanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(MainPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* SettingPanelLayout = lv_obj_create(MainPanel);
    lv_obj_set_width(SettingPanelLayout, 800);
    lv_obj_set_height(SettingPanelLayout, 90 * (ItemNum + 1));
    lv_obj_set_x(SettingPanelLayout, 0);
    lv_obj_set_y(SettingPanelLayout, 0);
    lv_obj_set_align(SettingPanelLayout, LV_ALIGN_CENTER);
    lv_obj_clear_flag(SettingPanelLayout, LV_OBJ_FLAG_SCROLLABLE);
    SettingPanelLayout->user_data = user_data;

    lv_obj_t* title = lv_obj_create(SettingPanelLayout);
    lv_obj_set_width(title, 800);
    lv_obj_set_height(title, 90);
    lv_obj_set_x(title, 0);
    lv_obj_set_y(title, -(90 / 2) * ItemNum);
    lv_obj_set_align(title, LV_ALIGN_CENTER);
    lv_obj_clear_flag(title, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_color(title, lv_color_hex(0xBBBBBB), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(title, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(title, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(title, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* title_text = lv_label_create(title);
    lv_obj_set_width(title_text, LV_SIZE_CONTENT);
    lv_obj_set_height(title_text, LV_SIZE_CONTENT);
    lv_obj_set_x(title_text, 0);
    lv_obj_set_y(title_text, 0);
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
        lv_obj_set_width(item_panel, 800);
        lv_obj_set_height(item_panel, 90);
        lv_obj_set_x(item_panel, 0);
        lv_obj_set_y(item_panel, -(90 / 2) * ItemNum + (i + 1) * 90);
        lv_obj_set_align(item_panel, LV_ALIGN_CENTER);
        lv_obj_clear_flag(item_panel, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_border_color(item_panel, lv_color_hex(0xBBBBBB), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_opa(item_panel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(item_panel, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_side(item_panel, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(item_panel, lv_color_hex(0xEF16E9), LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_bg_opa(item_panel, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_group_add_obj(MainGroup, item_panel);
        lv_obj_add_event_cb(item_panel, setting_item_key_event_handler, LV_EVENT_KEY, NULL);
        lv_group_set_focus_cb(MainGroup, focused_handler);

        lv_obj_t* item_text1 = lv_label_create(item_panel);
        lv_obj_set_width(item_text1, LV_SIZE_CONTENT);
        lv_obj_set_height(item_text1, LV_SIZE_CONTENT);
        lv_obj_set_y(item_text1, 0);
        lv_obj_set_align(item_text1, LV_ALIGN_CENTER);
        lv_obj_set_style_text_color(item_text1, lv_color_hex(0x1438FF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(item_text1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(item_text1, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);
        if (i < ItemNum - only_text_item_num) {
            lv_obj_set_x(item_text1, -200);
            item_text1->user_data = const_cast<char*>(text[2*i+1]);
            lv_label_set_text(item_text1, _(text[2*i+1]));
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
        static constexpr uint8_t mid = (100+320)/2;
        lv_obj_set_x(item_text2, mid);
        lv_obj_set_y(item_text2, 0);
        lv_obj_set_align(item_text2, LV_ALIGN_CENTER);
        item_text2->user_data = const_cast<char*>(text[2*i+2]);
        lv_label_set_text(item_text2, _(text[2*i+2]));
        lv_obj_set_style_text_color(item_text2, lv_color_hex(0x1438FF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(item_text2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(item_text2, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_t* item_image1 = lv_img_create(item_panel);
        lv_img_set_src(item_image1, &ui_img_menu_arrow_right32_png);
        lv_obj_set_width(item_image1, LV_SIZE_CONTENT);
        lv_obj_set_height(item_image1, LV_SIZE_CONTENT);
        lv_obj_set_x(item_image1, 320);
        lv_obj_set_y(item_image1, 0);
        lv_obj_set_align(item_image1, LV_ALIGN_CENTER);
        lv_obj_add_flag(item_image1, LV_OBJ_FLAG_ADV_HITTEST);
        lv_obj_clear_flag(item_image1, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_img_recolor(item_image1, lv_color_hex(0x1438FF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_img_recolor_opa(item_image1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_t* item_image2 = lv_img_create(item_panel);
        lv_img_set_src(item_image2, &ui_img_menu_arrow_left32_png);
        lv_obj_set_width(item_image2, LV_SIZE_CONTENT);
        lv_obj_set_height(item_image2, LV_SIZE_CONTENT);
        lv_obj_set_x(item_image2, 100);
        lv_obj_set_y(item_image2, 0);
        lv_obj_set_align(item_image2, LV_ALIGN_CENTER);
        lv_obj_add_flag(item_image2, LV_OBJ_FLAG_ADV_HITTEST);
        lv_obj_clear_flag(item_image2, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_img_recolor(item_image2, lv_color_hex(0x1438FF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_img_recolor_opa(item_image2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    LastFocused = nullptr;
    lv_group_focus_obj(lv_obj_get_child(SettingPanelLayout, 1));
}

void CreateSubSettingPanel(void* user_data, bool BgTransp)
{
    if (user_data == nullptr) return;
    Setting::Base* Setting = static_cast<Setting::Base*>(user_data);
    uint8_t ItemNum = Setting->GetItemNum();
    const char** text = Setting->GetStrArray();

    SubPanel = lv_obj_create(MainPanel);
    lv_obj_set_size(SubPanel, 1280, 720);
    lv_obj_clear_flag(SubPanel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(SubPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(SubPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    if (BgTransp)
        lv_obj_set_style_bg_opa(SubPanel, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    else
        lv_obj_set_style_bg_opa(SubPanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(SubPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* SettingPanelLayout = lv_obj_create(SubPanel);
    lv_obj_set_width(SettingPanelLayout, 600);
    lv_obj_set_height(SettingPanelLayout, 70 * (ItemNum + 1));
    lv_obj_set_x(SettingPanelLayout, 0);
    lv_obj_set_y(SettingPanelLayout, 0);
    lv_obj_set_align(SettingPanelLayout, LV_ALIGN_CENTER);
    lv_obj_clear_flag(SettingPanelLayout, LV_OBJ_FLAG_SCROLLABLE);
    SettingPanelLayout->user_data = user_data;

    lv_obj_t* title = lv_obj_create(SettingPanelLayout);
    lv_obj_set_width(title, 600);
    lv_obj_set_height(title, 70);
    lv_obj_set_x(title, 0);
    lv_obj_set_y(title, -(70 / 2) * ItemNum);
    lv_obj_set_align(title, LV_ALIGN_CENTER);
    lv_obj_clear_flag(title, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_color(title, lv_color_hex(0xBBBBBB), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(title, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(title, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(title, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* title_text = lv_label_create(title);
    lv_obj_set_width(title_text, LV_SIZE_CONTENT);
    lv_obj_set_height(title_text, LV_SIZE_CONTENT);
    lv_obj_set_x(title_text, 0);
    lv_obj_set_y(title_text, 0);
    lv_obj_set_align(title_text, LV_ALIGN_CENTER);
    title_text->user_data = const_cast<char*>(text[0]);
    lv_label_set_text(title_text, _(text[0]));
    lv_obj_set_style_text_color(title_text, lv_color_hex(0x1438FF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(title_text, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(title_text, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);

    MainGroup = create_new_group();
    set_group_activity(MainGroup);
    for(int i = 0; i < ItemNum; ++i) {
        // item_panel
        lv_obj_t* item_panel = lv_obj_create(SettingPanelLayout);
        lv_obj_set_width(item_panel, 600);
        lv_obj_set_height(item_panel, 70);
        lv_obj_set_x(item_panel, 0);
        lv_obj_set_y(item_panel, -(70 / 2) * ItemNum + (i + 1) * 70);
        lv_obj_set_align(item_panel, LV_ALIGN_CENTER);
        lv_obj_clear_flag(item_panel, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_border_color(item_panel, lv_color_hex(0xBBBBBB), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_opa(item_panel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(item_panel, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_side(item_panel, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(item_panel, lv_color_hex(0xEF16E9), LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_bg_opa(item_panel, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_group_add_obj(MainGroup, item_panel);
        lv_obj_add_event_cb(item_panel, setting_item_key_event_handler, LV_EVENT_KEY, NULL);
        lv_group_set_focus_cb(MainGroup, focused_handler);

        // item_text1
        lv_obj_t* item_text1 = lv_label_create(item_panel);
        lv_obj_set_width(item_text1, LV_SIZE_CONTENT);
        lv_obj_set_height(item_text1, LV_SIZE_CONTENT);
        lv_obj_set_x(item_text1, -200);
        lv_obj_set_y(item_text1, 0);
        item_text1->user_data = const_cast<char*>(text[i+1]);
        lv_label_set_text(item_text1, _(text[i+1]));
        lv_obj_set_align(item_text1, LV_ALIGN_CENTER);
        lv_obj_set_style_text_color(item_text1, lv_color_hex(0x1438FF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(item_text1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(item_text1, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);

        // item_text2
        lv_obj_t* item_text2 = lv_label_create(item_panel);
        lv_obj_set_width(item_text2, LV_SIZE_CONTENT);
        lv_obj_set_height(item_text2, LV_SIZE_CONTENT);
        lv_obj_set_x(item_text2, 230);
        lv_obj_set_y(item_text2, 0);
        lv_obj_set_align(item_text2, LV_ALIGN_CENTER);
        item_text2->user_data = const_cast<char*>(Setting->GetStr(i+1));
        lv_label_set_text(item_text2, _(Setting->GetStr(i+1)));
        lv_obj_set_style_text_color(item_text2, lv_color_hex(0x1438FF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(item_text2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(item_text2, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_t* slider = lv_slider_create(item_panel);
        lv_slider_set_range(slider, 0, 100);
        lv_slider_set_value(slider, Setting->GetUserValue(i+1), LV_ANIM_OFF);
        if(lv_slider_get_mode(slider) == LV_SLIDER_MODE_RANGE) lv_slider_set_left_value(slider, 0, LV_ANIM_OFF);
        lv_obj_set_width(slider, 200);
        lv_obj_set_height(slider, 7);
        lv_obj_set_x(slider, 80);
        lv_obj_set_y(slider, 0);
        lv_obj_set_align(slider, LV_ALIGN_CENTER);
        lv_obj_clear_state(slider, LV_STATE_FOCUS_KEY);
    }
    SubLastFocused = LastFocused;
    LastFocused = nullptr;
    lv_group_focus_obj(lv_obj_get_child(SettingPanelLayout, 1));
}
