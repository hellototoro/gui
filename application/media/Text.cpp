/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-07-05 11:14:24
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

static lv_obj_t* TextWindow;
static lv_obj_t* TextPanel;
static lv_obj_t* PlayBar;
static lv_group_t* Page_Group;
static constexpr lv_coord_t letter_space = 4;
static constexpr lv_coord_t line_space = 10;
static lv_coord_t TextLabWidth;
static std::vector<std::string> *TextBuff;
static std::string buff;

static void CreateTextWindow(lv_obj_t* parent);
static void CreateTextPanel(lv_obj_t* parent);
static void LoadText(char* file_name);
static void SetStyleForPlayBar(lv_obj_t* bar);

void creat_text_window(lv_obj_t* parent, char* file_name)
{
    TextLabWidth = LV_HOR_RES * 0.96875;

    TextBuff = new std::vector<std::string>;
    LoadText(file_name);

    CreateTextWindow(parent);

    MediaComInit(MEDIA_TEXT, nullptr);
    CreateMediaArray();
    LocateMediaIndex(file_name);
    PlayBar = CreatePlayBar(lv_scr_act());
    SetStyleForPlayBar(PlayBar);
}

void close_text_window(void)
{
    lv_group_del(Page_Group);

    TextBuff->clear();
    delete TextBuff;
    TextBuff = nullptr;

    //step2 清理播放列表
    DestroyMediaArray();
    MediaComDeinit();

    lv_obj_del_async(PlayBar);
    lv_obj_del_async(TextWindow);
}

void Text_NextPage(void)
{
    lv_group_focus_next(Page_Group);
}

void Text_PrePage(void)
{
    lv_group_focus_prev(Page_Group);
}

void ReLoadText(char* file_name)
{
    lv_obj_del(TextPanel);
    TextBuff->clear();
    LoadText(file_name);
    CreateTextPanel(TextWindow);
}

static void LoadText(char* file_name)
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

static void CreateTextWindow(lv_obj_t* parent)
{
    TextWindow = lv_obj_create(parent);
    lv_obj_set_size(TextWindow, LV_HOR_RES, LV_VER_RES);
    lv_obj_center(TextWindow);
    lv_obj_set_style_radius(TextWindow, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(TextWindow, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(TextWindow, lv_color_hex(0x2D729C), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(TextWindow, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(TextWindow, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    Page_Group = lv_group_create();//对于类别组做一般处理
    lv_group_set_wrap(Page_Group, false);
    CreateTextPanel(TextWindow);
}

static void CreateTextPanel(lv_obj_t* parent)
{
    TextPanel = lv_obj_create(parent);
    lv_obj_set_size(TextPanel, LV_HOR_RES, LV_VER_RES);
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
        lv_obj_set_size(obj, LV_HOR_RES, LV_VER_RES);
        lv_obj_set_pos(obj, i * LV_HOR_RES, 0);
        lv_obj_set_align(obj, LV_ALIGN_CENTER);
        lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_group_add_obj(Page_Group, obj);

        lv_obj_t* TextLab = lv_label_create(obj);
        lv_obj_set_size(TextLab, TextLabWidth, 680);
        lv_obj_set_align(TextLab, LV_ALIGN_TOP_MID);
        lv_label_set_text(TextLab, TextBuff->at(i).c_str());
        lv_obj_set_style_text_color(TextLab, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(TextLab, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_letter_space(TextLab, letter_space, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_line_space(TextLab, line_space, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(TextLab, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_t* PageLab = lv_label_create(obj);
        lv_obj_set_size(PageLab, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_align(PageLab, LV_ALIGN_BOTTOM_RIGHT);
        lv_label_set_text_fmt(PageLab, "%d / %d", i + 1, page);
        lv_obj_set_style_text_color(PageLab, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(PageLab, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(PageLab, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
}

static void SetStyleForPlayBar(lv_obj_t* bar)
{
    lv_obj_set_height(bar, 100);
    lv_obj_set_y(bar, 310);
    lv_obj_set_style_bg_color(bar, lv_color_hex(0x303030), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(bar, LV_OPA_90, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(bar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_pos(lv_obj_get_child(bar, Previous), -100, 0);
    lv_obj_set_pos(lv_obj_get_child(bar, Next), 0, 0);
    lv_obj_set_pos(lv_obj_get_child(bar, PlayList), 100, 0);

    lv_obj_add_flag(lv_obj_get_child(bar, PlayMode), LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(lv_obj_get_child(bar, Play), LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(lv_obj_get_child(bar, PlayedTime), LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(lv_obj_get_child(bar, ProgressSlider), LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(lv_obj_get_child(bar, TotalTime), LV_OBJ_FLAG_HIDDEN);
}
