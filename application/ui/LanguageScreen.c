/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-05-23 13:51:24
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-08-08 21:20:37
 * @FilePath: /gui/application/ui/LanguageScreen.c
 * @Description: None
 * @other: None
 */
#include <unistd.h>
#include "LanguageScreen.h"
#include "ui_com.h"
#include "application/windows.h"
#include "lv_i18n/src/lv_i18n.h"

lv_obj_t* ui_LanguageScreen;
lv_obj_t* ui_LanguagePanel;
static lv_obj_t* ui_LangImage;
lv_group_t* LanguageScreenGroup;

extern void WriteConfigFile_I(const char* ConfigName, int value);
extern void WriteConfigFile_S(const char* ConfigName, const char* value);

static const char* Language[] = {
"es",
"de",
"en-GB",
"it",
"fr",
"zh-CN"
};
const char* DefaultLanguage = "en-GB";

static void event_handler(lv_event_t* event);
static void ExitHome(ActiveScreen screen);

void event_handler(lv_event_t* event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t* target = lv_event_get_target(event);
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
            if (index == English || index == Chinese) {
                DefaultLanguage = Language[index];
                WriteConfigFile_I("guide_flag.flag", 0);
                WriteConfigFile_S("default_language.language", DefaultLanguage);
                lv_i18n_set_locale(DefaultLanguage);
                ExitHome(HomeScreen);
            }
            break;
        case LV_KEY_ESC:
            break;
        default:
            break;
        }
    }
    else if (LV_EVENT_FOCUSED == code) {
        lv_img_set_src(ui_LangImage, target->user_data);
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
        &ui_img_install_bg_chn_png
    };
    const char* language_list[6];
    lv_i18n_init(lv_i18n_language_pack);
    lv_i18n_set_locale(DefaultLanguage);
    language_list[0] = _("language_espaol");
    language_list[1] = _("language_deutsch");
    language_list[2] = _("language_english");
    language_list[3] = _("language_italiano");
    language_list[4] = _("language_france");
    language_list[5] = _("language_chinese");

    // ui_LanguagePanel
    ui_LanguagePanel = lv_obj_create(ui_LanguageScreen);
    lv_obj_set_width(ui_LanguagePanel, 540);
    lv_obj_set_height(ui_LanguagePanel, 520);
    lv_obj_set_x(ui_LanguagePanel, 240);
    lv_obj_set_y(ui_LanguagePanel, 0);
    lv_obj_set_align(ui_LanguagePanel, LV_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(ui_LanguagePanel, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(ui_LanguagePanel, LV_DIR_VER);
    lv_obj_set_style_bg_opa(ui_LanguagePanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_LanguagePanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_flex_flow(ui_LanguagePanel, LV_FLEX_FLOW_COLUMN);

    LanguageScreenGroup = lv_group_create();
    set_group_activity(LanguageScreenGroup);
    for (int i = 0; i < LanguageNumber; i++) {
        // Lang
        lv_obj_t* lang = lv_obj_create(ui_LanguagePanel);
        lv_obj_set_width(lang, 500);
        lv_obj_set_height(lang, 92);
        //lv_obj_set_x(lang, 6);
        //lv_obj_set_y(lang, -200 + i * 100);
        lv_obj_set_align(lang, LV_ALIGN_CENTER);
        lv_obj_clear_flag(lang, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_flag(lang, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
        lv_obj_set_style_bg_img_src(lang, &ui_img_install_norm_png, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_img_src(lang, &ui_img_install_selected_png, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_bg_opa(lang, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_opa(lang, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lang->user_data = (void*)image_src[i];
        lv_group_add_obj(LanguageScreenGroup, lang);
        lv_obj_add_event_cb(lang, event_handler, LV_EVENT_ALL, NULL);

        // Text
        lv_obj_t* text = lv_label_create(lang);
        lv_obj_set_width(text, LV_SIZE_CONTENT);
        lv_obj_set_height(text, LV_SIZE_CONTENT);
        lv_obj_set_x(text, 0);
        lv_obj_set_y(text, 0);
        lv_obj_set_align(text, LV_ALIGN_CENTER);
        lv_label_set_text(text, language_list[i]);
        lv_obj_set_style_text_color(text, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(text, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_letter_space(text, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_line_space(text, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_align(text, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(text, &ui_font_MyFont38, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    lv_group_focus_obj(lv_obj_get_child(ui_LanguagePanel, English));
}

static void LanguageInit(lv_obj_t* parent, void *param)
{
    (void)param;

    ui_LanguageScreen = lv_obj_create(parent);
    lv_obj_set_size(ui_LanguageScreen, 1280, 720);
    lv_obj_set_style_bg_color(ui_LanguageScreen, lv_color_hex(0x3200FE), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_LanguageScreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_LangImage
    ui_LangImage = lv_img_create(ui_LanguageScreen);
    lv_img_set_src(ui_LangImage, &ui_img_install_bg_eng_png);
    lv_obj_set_width(ui_LangImage, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LangImage, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_LangImage, -260);
    lv_obj_set_y(ui_LangImage, -15);
    lv_obj_set_align(ui_LangImage, LV_ALIGN_CENTER);

    // ui_LangDownArrow
    lv_obj_t* ui_LangDownArrow = lv_img_create(ui_LanguageScreen);
    lv_img_set_src(ui_LangDownArrow, &ui_img_img_arrow_down2_png);
    lv_obj_set_width(ui_LangDownArrow, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LangDownArrow, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_LangDownArrow, 240);
    lv_obj_set_y(ui_LangDownArrow, 310);
    lv_obj_set_align(ui_LangDownArrow, LV_ALIGN_CENTER);

    // ui_LangUpArrow
    lv_obj_t* ui_LangUpArrow = lv_img_create(ui_LanguageScreen);
    lv_img_set_src(ui_LangUpArrow, &ui_img_img_arrow_up2_png);
    lv_obj_set_width(ui_LangUpArrow, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LangUpArrow, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_LangUpArrow, 240);
    lv_obj_set_y(ui_LangUpArrow, -310);
    lv_obj_set_align(ui_LangUpArrow, LV_ALIGN_CENTER);

    CreateLanguagePanel(ui_LanguageScreen);
}

static void ExitHome(ActiveScreen screen)
{
    CurrentScreen = screen;
}

static void LanguageClose(void)
{
    lv_group_del(LanguageScreenGroup);
    lv_obj_del(ui_LanguageScreen);
}

static void LoadLanguage(void)
{
    lv_disp_load_scr(ui_LanguageScreen);
}

window LanguageWindow = {
    .ScreenInit = LanguageInit,
    .ScreenLoad = LoadLanguage,
    .ScreenClose = LanguageClose
};
