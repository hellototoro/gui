/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-07-05 11:14:24
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-07-21 13:07:22
 * @FilePath: /gui/application/ui/media/Text.cpp
 * @Description: None
 * @other: None
 */
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include <ctype.h>
#include "Text.h"
#include "MediaFile.h"
#include "MediaCom.h"
#include "application/ui/ui_com.h"

lv_obj_t* TextScreen;
lv_obj_t* TextPanel;
lv_obj_t* TextName;
lv_group_t* Text_Group;
static constexpr lv_coord_t letter_space = 4;
static constexpr lv_coord_t line_space = 10;
static constexpr lv_coord_t TextLabWidth = 1240;
std::vector<std::string> *TextBuff;
std::string buff;

static void key_event_handler(lv_event_t* event);
static lv_obj_t* CreateTextScreen(lv_obj_t* parent);
static void SetStyleForPlayBar(lv_obj_t* bar);
static void CreateTextPanel(lv_obj_t* parent);
void LoadText_t(char* file_name);

lv_obj_t* creat_text_window(lv_obj_t* foucsed_obj)
{
    lv_group_t* old_group = (lv_group_t*)lv_obj_get_group(foucsed_obj);
    Text_Group = create_new_group(old_group);
    set_group_activity(Text_Group);

    TextBuff = new std::vector<std::string>;
    LoadText(((FileStr *)(foucsed_obj->user_data))->name);

    CreateTextScreen(lv_scr_act());
    
    //lv_obj_t* MusicName = lv_label_create(TextPanel);
    //RefreshText(nullptr);
    //lv_label_set_text(MusicName, buff.c_str());
    //lv_textarea_set_text(TextPanel, buff.c_str());
    //SetStyleForPlayBar(CreatePlayBar(TextScreen));
    return TextScreen;
}

void close_text_window(lv_obj_t* text_window)
{
    delete_group(Text_Group);
    Text_Group = NULL;

    TextBuff->clear();
    delete TextBuff;
    TextBuff = nullptr;

    lv_obj_del_async(text_window);
}

void LoadText(char* file_name)
{
    std::ifstream* text_file = new std::ifstream(std::string(std::string(current_path)+ '/'+ std::string(file_name)).data());
    std::string line;
    int line_number = 0;
    int page_number = 0;
    TextBuff->resize(page_number + 1, "");
    while ( std::getline (*text_file, line) ) {
        if (line.size() > 0 && line[line.size()-1] == '\r') line.erase(line.size()-1);
        lv_coord_t pixel_width = lv_txt_get_width(line.c_str(), line.size(), &ui_font_MyFont30, letter_space, LV_TEXT_FLAG_NONE);
        if(pixel_width != 0) line_number += static_cast<int>(ceil(pixel_width/((TextLabWidth)*1.f)));
        else                 ++line_number;
        if (line_number > 13) {
            line_number = 0;
            ++page_number;
            TextBuff->resize(page_number + 1, "");
        }
        TextBuff->at(page_number) += line + '\n';
    }
    if(text_file->is_open())
        text_file->close();
    delete text_file;
}

void RefreshText(const char* text)
{
    (void)text;
    if (TextBuff->size() != 0)
        lv_textarea_set_text(TextPanel, TextBuff[0].data()->data());
}

static void key_event_handler(lv_event_t* event)
{
    lv_obj_t* target = lv_event_get_target(event);
    uint32_t value = lv_indev_get_key(lv_indev_get_act());
    switch (value)
    {
        case LV_KEY_LEFT:
            if (lv_obj_get_index(target) > 0 )
                lv_group_focus_prev(Text_Group);
            break;
        case LV_KEY_RIGHT:
            if (lv_obj_get_index(target) < (lv_obj_get_child_cnt(TextPanel) - 1) )
                lv_group_focus_next(Text_Group);
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
    TextPanel = lv_obj_create(parent);
    lv_obj_set_width(TextPanel, 1280);
    lv_obj_set_height(TextPanel, 720);
    lv_obj_set_align(TextPanel, LV_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(TextPanel, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(TextPanel, LV_DIR_HOR);
    lv_obj_set_style_radius(TextPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(TextPanel, lv_color_hex(0x2D729C), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(TextPanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(TextPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_scroll_snap_x(TextPanel, LV_SCROLL_SNAP_CENTER);

    int page = TextBuff->size();
    for(int i = 0; i < page; ++i) {
        lv_obj_t* obj = lv_obj_create(TextPanel);
        lv_obj_set_width(obj, 1280);
        lv_obj_set_height(obj, 720);
        lv_obj_set_x(obj, i * 1280);
        lv_obj_set_y(obj, 0);
        lv_obj_set_align(obj, LV_ALIGN_CENTER);
        lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
        lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_group_add_obj(Text_Group, obj);
        lv_obj_add_event_cb(obj, key_event_handler, LV_EVENT_KEY, NULL);

        lv_obj_t* ui_Label = lv_label_create(obj);
        lv_obj_set_width(ui_Label, TextLabWidth);
        lv_obj_set_height(ui_Label, 680);
        lv_obj_set_align(ui_Label, LV_ALIGN_TOP_MID);
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
        lv_label_set_text(ui_Label, TextBuff->at(i).c_str());
        lv_obj_set_style_text_color(ui_Label, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(ui_Label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_letter_space(ui_Label, letter_space, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_line_space(ui_Label, line_space, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(ui_Label, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
}
