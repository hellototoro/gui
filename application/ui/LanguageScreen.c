/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-05-23 13:51:24
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-09-19 00:14:29
 * @FilePath: /gui/application/ui/LanguageScreen.c
 * @Description: None
 * @other: None
 */
#include <unistd.h>
#include "LanguageScreen.h"
#include "application/windows.h"

/**********全局变量***********/
const char* Language[] = {
"es",
"de",
"en-GB",
"it",
"fr",
"zh-CN"
};
int DefaultLanguageIndex = 2;

extern void WriteConfigFile_I(const char* ConfigName, int value);
extern void WriteConfigFile_S(const char* ConfigName, const char* value);

/**********静态变量***********/
static lv_obj_t* LanguageRootScreen;
static lv_obj_t* MainPanel;
static lv_obj_t* LangImage;
static lv_group_t* MainGroup;

static void event_handler(lv_event_t* event);
static void ExitLanguage(ActiveScreen screen);

void SaveCurrentLanguageType(int index)
{
    DefaultLanguageIndex = index;
    WriteConfigFile_I("default_language.index", index);
    WriteConfigFile_S("default_language.language", Language[index]);
    lv_i18n_set_locale(Language[index]);
}

void event_handler(lv_event_t* event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t* target = lv_event_get_current_target(event);
    if (LV_EVENT_KEY == code) {
        uint32_t value = lv_indev_get_key(lv_indev_get_act());
        lv_group_t* group = (lv_group_t*)lv_obj_get_group(target);
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
            WriteConfigFile_I("guide_flag.flag", 0);
            SaveCurrentLanguageType(index);
            ExitLanguage(HomeScreen);
            break;
        case LV_KEY_ESC:
            break;
        default:
            break;
        }
    }
    else if (LV_EVENT_FOCUSED == code) {
        lv_img_set_src(LangImage, target->user_data);
    }
}

static void CreateLanguagePanel(lv_obj_t* parent)
{
    static const lv_img_dsc_t* image_src[] = {
        &ui_img_install_bg_spa_png,
        &ui_img_install_bg_gem_png,
        &ui_img_install_bg_eng_png,
        &ui_img_install_bg_ita_png,
        &ui_img_install_bg_fra_png,
        &ui_img_install_bg_chn_png };
    static const char* language_list[6] = {
    "language_espaol", "language_deutsch", "language_english", 
    "language_italiano", "language_france", "language_chinese" };
    
    lv_i18n_init(lv_i18n_language_pack);
    //lv_i18n_set_locale(Language[DefaultLanguageIndex]);
    //SaveCurrentLanguageType(DefaultLanguageIndex);

    MainPanel = lv_obj_create(LanguageRootScreen);
    lv_obj_set_size(MainPanel, 540, 520);
    lv_obj_set_pos(MainPanel, 240, 0);
    lv_obj_set_align(MainPanel, LV_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(MainPanel, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(MainPanel, LV_DIR_VER);
    lv_obj_set_style_bg_opa(MainPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(MainPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_flex_flow(MainPanel, LV_FLEX_FLOW_COLUMN);

    MainGroup = create_new_group();
    set_group_activity(MainGroup);
    for (int i = 0; i < LanguageNumber; i++) {
        lv_obj_t* obj = lv_obj_create(MainPanel);
        lv_obj_set_size(obj, 500, 92);
        lv_obj_set_align(obj, LV_ALIGN_CENTER);
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
        lv_obj_set_style_bg_img_src(obj, &ui_img_install_norm_png, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_img_src(obj, &ui_img_install_selected_png, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        obj->user_data = (void*)image_src[i];
        lv_group_add_obj(MainGroup, obj);
        lv_obj_add_event_cb(obj, event_handler, LV_EVENT_ALL, NULL);

        lv_obj_t* text = lv_label_create(obj);
        lv_obj_set_size(text, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_pos(text, 0, 0);
        lv_obj_set_align(text, LV_ALIGN_CENTER);
        text->user_data = (void*)language_list[i];
        lv_label_set_text(text, _(language_list[i]));
        lv_obj_set_style_text_color(text, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(text, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_letter_space(text, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_line_space(text, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_align(text, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(text, &ui_font_MyFont38, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    lv_group_focus_obj(lv_obj_get_child(MainPanel, English));
}

static void LanguageInit(void)
{
    LanguageRootScreen = lv_obj_create(NULL);
    lv_obj_set_size(LanguageRootScreen, 1280, 720);
    lv_obj_set_style_bg_color(LanguageRootScreen, lv_color_hex(0x3200FE), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(LanguageRootScreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    LangImage = lv_img_create(LanguageRootScreen);
    lv_img_set_src(LangImage, &ui_img_install_bg_eng_png);
    lv_obj_set_size(LangImage, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_pos(LangImage, -260, -15);
    lv_obj_set_align(LangImage, LV_ALIGN_CENTER);

    lv_obj_t* img = lv_img_create(LanguageRootScreen);
    lv_img_set_src(img, &ui_img_img_arrow_down2_png);
    lv_obj_set_size(img, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_pos(img, 240, 310);
    lv_obj_set_align(img, LV_ALIGN_CENTER);

    img = lv_img_create(LanguageRootScreen);
    lv_img_set_src(img, &ui_img_img_arrow_up2_png);
    lv_obj_set_size(img, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_pos(img, 240, -310);
    lv_obj_set_align(img, LV_ALIGN_CENTER);

    CreateLanguagePanel(LanguageRootScreen);
}

static void LoadLanguage(void)
{
    lv_scr_load_anim(LanguageRootScreen, LV_SCR_LOAD_ANIM_NONE, 0, 0, true);
    //lv_scr_load_anim(LanguageRootScreen, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0, true);
}

static void ExitLanguage(ActiveScreen screen)
{
    delete_all_group();
    //lv_obj_del(LanguageRootScreen);
    CurrentScreen = screen;
}

window LanguageWindow = {
    .ScreenInit = LanguageInit,
    .ScreenLoad = LoadLanguage,
};
