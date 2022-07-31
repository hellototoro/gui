/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-26 11:13:24
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-07-31 20:11:17
 * @FilePath: /gui/application/ui/SettingScreen.cpp
 * @Description: None
 * @other: None
 */
#include "SettingScreen.h"
#include "setting/Setting.h"
#include "ui_com.h"

lv_obj_t* ui_SettingScreen;
lv_obj_t* SettingPanel;
lv_group_t* SettingGroup;
lv_obj_t* LastFocused;

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


typedef struct LvObjUserData {
    void* SettingData;
    void (*SelectData) (void);
    void (*IncreaseData) (void);
    void (*DecreaseData) (void);
    char* (*GetStr) (void);
} LvObjUserData_t;


const char* PictureItemName[] = {
    "图像设置",
    "图像模式", "标准", 
    "缩放设置", "4:3", 
    "色温", "暖色", 
    "投影缩放", "100%", 
    "充电宝亮度模式", "中"
};

const char* SoundItemName[] = {
    "声音设置",
    "声音模式", "音乐", 
    "环绕声", "SRS环绕", 
    "自动音量", "开"
};

const char* SystemItemName[] = {
    "系统设置",
    "语言", "简体中文", // i = 0
    "菜单显示时间", "关",  // i = 1
    "自动开机", "关",  // i = 2
    "软件升级",  // i = 3
    "恢复出厂设置" // i = 4
};

static void event_handler(lv_event_t* event);
void CreateSettingPanel(void* user_data, bool BgTransp);

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
    lv_obj_set_style_border_opa(ui_SettingScreen, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

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
        lv_obj_add_event_cb(ui_Image, event_handler, LV_EVENT_KEY, NULL);

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
    lv_obj_t* target = lv_event_get_target(event);
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
            break;

        case Position:
            break;

        case Picture:
            CreateSettingPanel(Setting::CreatePictureSettingObj(), false);
            break;

        case Sound:
            //CreateSettingPanel(3, 0, SoundItemName, NULL, false);
            break;

        case System:
            //CreateSettingPanel(5, 2, SystemItemName, NULL, false);
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
        delete_group(SettingGroup);
        lv_obj_del_async(ui_SettingScreen);
    break;

    default:
        break;
    }
}

static void setting_item_key_event_handler(lv_event_t* event)
{
    lv_obj_t* target = lv_event_get_target(event);
    lv_obj_t* parent = lv_obj_get_parent(target);
    lv_group_t* group = (lv_group_t*)lv_obj_get_group(target);
    uint32_t index = lv_obj_get_index(target);
    uint32_t value = lv_indev_get_key(lv_indev_get_act());
    Setting::Base* Setting = static_cast<Setting::Base*>(parent->user_data);
    switch (value)
    {
    case LV_KEY_ENTER:
        Setting->SelectData(index);
        break;
    case LV_KEY_LEFT:
        if (lv_obj_get_child_cnt(target) == 1) {
            break;
        }
        Setting->DecreaseData(index);
        lv_label_set_text(target->spec_attr->children[1], Setting->GetStr(index));
        break;
    case LV_KEY_RIGHT:
        if (lv_obj_get_child_cnt(target) == 1) {
            break;
        }
        Setting->IncreaseData(index);
        lv_label_set_text(target->spec_attr->children[1], Setting->GetStr(index));
        break;
    case LV_KEY_UP:
        lv_group_focus_prev(group);
        break;
    case LV_KEY_DOWN:
        lv_group_focus_next(group);
        break;
    case LV_KEY_ESC:
        SettingGroup = delete_group(SettingGroup);
        lv_obj_del_async(SettingPanel);
        delete Setting;
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
            else
                lv_obj_set_style_img_recolor(child, lv_color_hex(0x1438FF), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
    lv_obj_t* obj = lv_group_get_focused(group);
    int count = obj->spec_attr->child_cnt;
    for(int i = 0; i < count; ++i) {
        lv_obj_t* child = obj->spec_attr->children[i];
        if(lv_obj_check_type(child, &lv_label_class))
            lv_obj_set_style_text_color(child, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
        else
            lv_obj_set_style_img_recolor(child, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    LastFocused = obj;
}

void CreateSettingPanel(void* user_data, bool BgTransp)
{
    Setting::Base* Setting = static_cast<Setting::Base*>(user_data);
    uint8_t item_num = Setting->GetItemNum();
    uint8_t only_text_item_num = Setting->GetOnlyTextItemNum();
    const char** text = Setting->GetStrArray();

    SettingPanel = lv_obj_create(lv_scr_act());
    lv_obj_set_size(SettingPanel, 1280, 720);
    lv_obj_clear_flag(SettingPanel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(SettingPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(SettingPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    if (BgTransp)
        lv_obj_set_style_bg_opa(SettingPanel, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    else
        lv_obj_set_style_bg_opa(SettingPanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(SettingPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // SettingPanelLayout
    lv_obj_t* SettingPanelLayout = lv_obj_create(SettingPanel);
    lv_obj_set_width(SettingPanelLayout, 800);
    lv_obj_set_height(SettingPanelLayout, 90 * (item_num + 1));
    lv_obj_set_x(SettingPanelLayout, 0);
    lv_obj_set_y(SettingPanelLayout, 0);
    lv_obj_set_align(SettingPanelLayout, LV_ALIGN_CENTER);
    lv_obj_clear_flag(SettingPanelLayout, LV_OBJ_FLAG_SCROLLABLE);
    SettingPanelLayout->user_data = user_data;

    lv_obj_t* title = lv_obj_create(SettingPanelLayout);
    lv_obj_set_width(title, 800);
    lv_obj_set_height(title, 90);
    lv_obj_set_x(title, 0);
    lv_obj_set_y(title, -(90 / 2) * item_num);
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
    lv_label_set_text(title_text, text[0]);
    lv_obj_set_style_text_color(title_text, lv_color_hex(0x1438FF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(title_text, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(title_text, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);

    SettingGroup = create_new_group(SettingGroup);
    set_group_activity(SettingGroup);
    for(int i = 0; i < item_num; ++i) {
        // item_panel
        lv_obj_t* item_panel = lv_obj_create(SettingPanelLayout);
        lv_obj_set_width(item_panel, 800);
        lv_obj_set_height(item_panel, 90);
        lv_obj_set_x(item_panel, 0);
        lv_obj_set_y(item_panel, -(90 / 2) * item_num + (i + 1) * 90);
        lv_obj_set_align(item_panel, LV_ALIGN_CENTER);
        lv_obj_clear_flag(item_panel, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_border_color(item_panel, lv_color_hex(0xBBBBBB), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_opa(item_panel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(item_panel, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_side(item_panel, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(item_panel, lv_color_hex(0xEF16E9), LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_bg_opa(item_panel, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_group_add_obj(SettingGroup, item_panel);
        lv_obj_add_event_cb(item_panel, setting_item_key_event_handler, LV_EVENT_KEY, NULL);
        lv_group_set_focus_cb(SettingGroup, focused_handler);

        // item_text1
        lv_obj_t* item_text1 = lv_label_create(item_panel);
        lv_obj_set_width(item_text1, LV_SIZE_CONTENT);
        lv_obj_set_height(item_text1, LV_SIZE_CONTENT);
        lv_obj_set_y(item_text1, 0);
        lv_obj_set_align(item_text1, LV_ALIGN_CENTER);
        lv_obj_set_style_text_color(item_text1, lv_color_hex(0x1438FF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(item_text1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(item_text1, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);
        if (i < item_num - only_text_item_num) {
            lv_obj_set_x(item_text1, -200);
            lv_label_set_text(item_text1, text[2*i+1]);
        }
        else {
            lv_obj_set_x(item_text1, 0);
            lv_label_set_text(item_text1, text[item_num - only_text_item_num + 1 + i]);
            continue;
        }

        // item_text2
        lv_obj_t* item_text2 = lv_label_create(item_panel);
        lv_obj_set_width(item_text2, LV_SIZE_CONTENT);
        lv_obj_set_height(item_text2, LV_SIZE_CONTENT);
        static constexpr uint8_t mid = (120+300)/2;
        lv_obj_set_x(item_text2, mid);
        lv_obj_set_y(item_text2, 0);
        lv_obj_set_align(item_text2, LV_ALIGN_CENTER);
        lv_label_set_text(item_text2, text[2*i+2]);
        lv_obj_set_style_text_color(item_text2, lv_color_hex(0x1438FF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(item_text2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(item_text2, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);

        // item_image1
        lv_obj_t* item_image1 = lv_img_create(item_panel);
        lv_img_set_src(item_image1, &ui_img_menu_arrow_right32_png);
        lv_obj_set_width(item_image1, LV_SIZE_CONTENT);
        lv_obj_set_height(item_image1, LV_SIZE_CONTENT);
        lv_obj_set_x(item_image1, 300);
        lv_obj_set_y(item_image1, 0);
        lv_obj_set_align(item_image1, LV_ALIGN_CENTER);
        lv_obj_add_flag(item_image1, LV_OBJ_FLAG_ADV_HITTEST);
        lv_obj_clear_flag(item_image1, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_img_recolor(item_image1, lv_color_hex(0x1438FF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_img_recolor_opa(item_image1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

        // item_image2
        lv_obj_t* item_image2 = lv_img_create(item_panel);
        lv_img_set_src(item_image2, &ui_img_menu_arrow_left32_png);
        lv_obj_set_width(item_image2, LV_SIZE_CONTENT);
        lv_obj_set_height(item_image2, LV_SIZE_CONTENT);
        lv_obj_set_x(item_image2, 120);
        lv_obj_set_y(item_image2, 0);
        lv_obj_set_align(item_image2, LV_ALIGN_CENTER);
        lv_obj_add_flag(item_image2, LV_OBJ_FLAG_ADV_HITTEST);
        lv_obj_clear_flag(item_image2, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_img_recolor(item_image2, lv_color_hex(0x1438FF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_img_recolor_opa(item_image2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    LastFocused = NULL;
    lv_group_focus_obj(lv_obj_get_child(SettingPanelLayout, 1));
}
