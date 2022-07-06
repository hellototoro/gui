/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-07-05 11:14:24
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-07-05 13:04:29
 * @FilePath: /gui/application/ui/media/Text.cpp
 * @Description: None
 * @other: None
 */
#include <fstream>
#include <iostream>
#include <string>
#include <queue>
#include "Text.h"
#include "MediaFile.h"
#include "MediaCom.h"

lv_obj_t* TextScreen;
lv_obj_t* TextPanel;
lv_obj_t* TextName;
lv_group_t* Text_Group;

std::queue<std::string> *TextBuff;

LV_FONT_DECLARE(ui_font_MyFont30);
LV_FONT_DECLARE(ui_font_MyFont34);
LV_FONT_DECLARE(ui_font_MyFont38);

static void key_event_handler(lv_event_t* event);
static lv_obj_t* CreateTextScreen(lv_obj_t* parent);
static void SetStyleForPlayBar(lv_obj_t* bar);
static void CreateTextPanel(lv_obj_t* parent);

lv_obj_t* creat_text_window(lv_obj_t* foucsed_obj)
{
    CreateTextScreen(lv_scr_act());
    TextBuff = new std::queue<std::string>;
    LoadText(((FileStr *)(foucsed_obj->user_data))->name);
    //SetStyleForPlayBar(CreatePlayBar(TextScreen));
    return TextScreen;
}

void close_text_window(lv_obj_t* text_window)
{
    delete TextBuff;
    TextBuff = nullptr;
    //step4 关闭窗口
    lv_obj_del_async(text_window);
}

void LoadText(char* file_name)
{
    std::ifstream* text_file = new std::ifstream(std::string(std::string(current_path)+ '/'+ std::string(file_name)).data());
    std::string line;
    while ( std::getline (*text_file, line) ) {
        TextBuff->push(line);
        std::cout << line << std::endl;
    }
    if(text_file->is_open())
        text_file->close();
    delete text_file;
}

static void key_event_handler(lv_event_t* event)
{
    //lv_obj_t* target = lv_event_get_target(event);
    //lv_obj_t* parents = lv_obj_get_parent(target);
    //lv_group_t* group = (lv_group_t*)lv_obj_get_group(target);
    (void)event;
    uint32_t value = lv_indev_get_key(lv_indev_get_act());
    switch (value)
    {
        case LV_KEY_UP:
            break;
        case LV_KEY_DOWN:
            break;
        case LV_KEY_ESC:
            close_text_window(TextScreen);
        break;

        default:
        break;
    }
}

static void SetStyleForPlayBar(lv_obj_t* bar)
{
    lv_obj_set_style_bg_opa(bar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(bar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
}

static lv_obj_t* CreateTextScreen(lv_obj_t* parent)
{
    TextScreen = lv_obj_create(parent);
    lv_obj_set_size(TextScreen, 1280, 720);
    lv_obj_set_style_radius(TextScreen, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(TextScreen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(TextScreen, lv_color_hex(0x2D729C), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(TextScreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(TextScreen, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    CreateTextPanel(TextScreen);
    return TextScreen;
}

static void CreateTextPanel(lv_obj_t* parent)
{
    TextPanel = lv_textarea_create(parent);
    lv_obj_set_width(TextPanel, 1280);
    lv_obj_set_height(TextPanel, 720);
    lv_obj_set_align(TextPanel, LV_ALIGN_CENTER);
    lv_obj_clear_flag(TextPanel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(TextPanel, lv_color_hex(0x2D729C), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(TextPanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(TextPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(TextPanel, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(TextPanel, key_event_handler, LV_EVENT_KEY, NULL);
}
