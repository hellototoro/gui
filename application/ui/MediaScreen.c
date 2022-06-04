/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-05-23 13:51:24
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-06-04 13:55:31
 * @FilePath: /gui/application/ui/MediaScreen.c
 * @Description: None
 * @other: None
 */
#include <unistd.h>
#include "MediaScreen.h"
#include "application/windows.h"

static const lv_coord_t FileListPanelStartPos_x = -402;
static const lv_coord_t FileListPanelStartPos_y = -182;
static const uint8_t FileListPanelOffset_x = 160;
static const uint8_t FileListPanelOffset_y = 182;
static lv_indev_t* keypad_indev;

enum CategoryList {
    All,
    Vedio,
    Music,
    Photo,
    Text,
    CategoryNumber
};

lv_obj_t* ui_MediaScreen;
lv_obj_t* ui_Category_Panel;
lv_obj_t* ui_File_List_Panel;
lv_group_t* Category_Group;
lv_group_t* File_List_Group;

uint8_t DiskCnt = 0;
bool ShowDisk_Falg = false;

static void ShowDisk(void);
static void ShowFile(char *path);

static void base_event_handler(lv_event_t* event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t* ta = lv_event_get_target(event);
    lv_obj_t* parents = lv_obj_get_parent(ta);
    if (LV_EVENT_KEY == code) {
        uint32_t value = lv_indev_get_key(lv_indev_get_act());
        lv_group_t* group = (lv_group_t*)lv_obj_get_group(ta);
        if (LV_KEY_UP == value) {
            if (ui_Category_Panel == parents) {
                lv_group_focus_prev(group);
            }
            else if (ui_File_List_Panel == parents) {
                if (!ShowDisk_Falg) {
                    int index = lv_obj_get_index(ta);
                    if (index - 6 >= 0)
                        lv_group_focus_obj(lv_obj_get_child(ui_File_List_Panel, index - 6));
                }
            }
        }
        else if (LV_KEY_DOWN == value) {
            if (ui_Category_Panel == parents) {
                lv_group_focus_next(group);
            }
            else if (ui_File_List_Panel == parents) {
                if (!ShowDisk_Falg) {
                    int index = lv_obj_get_index(ta);
                    if ((uint32_t)(index + 6) < lv_obj_get_child_cnt(ui_File_List_Panel))
                        lv_group_focus_obj(lv_obj_get_child(ui_File_List_Panel, index + 6));
                    else if ( ((uint32_t)index/6)*6 + 6 < lv_obj_get_child_cnt(ui_File_List_Panel)) {
                        lv_group_focus_obj(lv_obj_get_child(ui_File_List_Panel, lv_obj_get_child_cnt(ui_File_List_Panel) - 1));
                    }
                }
            }
        }
        else if (LV_KEY_LEFT == value) {
            if (ui_File_List_Panel == parents) {
                if (lv_obj_get_child(ui_File_List_Panel, 0) != ta) {
                    lv_group_focus_prev(group);
                }
                else {
                    lv_group_focus_obj(ui_File_List_Panel);
                    lv_group_set_default(Category_Group);
                    lv_indev_set_group(keypad_indev, Category_Group);
                }
            }
        }
        else if (LV_KEY_RIGHT == value) {
            if (ui_Category_Panel == parents) {
                lv_group_set_default(File_List_Group);
                lv_indev_set_group(keypad_indev, File_List_Group);
                lv_group_focus_obj(lv_obj_get_child(ui_File_List_Panel, 0));
            }
            else if (ui_File_List_Panel == parents) {
                if (lv_obj_get_child(ui_File_List_Panel, lv_obj_get_child_cnt(ui_File_List_Panel) - 1) != ta)
                    lv_group_focus_next(group);
                else
                    lv_group_focus_obj(lv_obj_get_child(ui_File_List_Panel, 0));
            }
        }
        /*else if (LV_KEY_ENTER == value) {
            if (ui_File_List_Panel == parents) {
                lv_obj_clean(ui_File_List_Panel);
                ShowFile(NULL);
                lv_group_focus_obj(lv_obj_get_child(ui_File_List_Panel, 0));
            }
        }*/
    }
}

static void enter_disk_handler(lv_event_t* event)
{
    lv_event_code_t code = lv_event_get_code(event);
    //lv_obj_t* ta = lv_event_get_target(event);
    //lv_obj_t* parents = lv_obj_get_parent(ta);
    if (LV_EVENT_KEY == code) {
        uint32_t value = lv_indev_get_key(lv_indev_get_act());
        //lv_group_t* group = (lv_group_t*)lv_obj_get_group(ta);
        if (LV_KEY_ENTER == value) {
            lv_obj_clean(ui_File_List_Panel);
            ShowFile(NULL);
            lv_group_focus_obj(lv_obj_get_child(ui_File_List_Panel, 0));
        }
        else
            base_event_handler(event);
    }
}

static void GroupInit(void)
{
    Category_Group = lv_group_create();
    File_List_Group = lv_group_create();
}

static void CreateCategoryPanel(lv_obj_t* parent)
{
    static const char* lab_text[] = {
        "全部分类",
        "视频文件",
        "音乐文件",
        "图片文件",
        "文本文件" 
    };
    // ui_Category_Panel
    ui_Category_Panel = lv_obj_create(parent);
    lv_obj_set_size(ui_Category_Panel, 252, 420);
    lv_obj_set_pos(ui_Category_Panel, -515, 0);
    lv_obj_set_align(ui_Category_Panel, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_Category_Panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_Category_Panel, lv_color_hex(0x36589D), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Category_Panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_Category_Panel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_Category_Panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    for (int i = 0; i < CategoryNumber; i++) {
        lv_obj_t* ui_BTN = lv_btn_create(ui_Category_Panel);
        lv_obj_set_size(ui_BTN, 265, 50);
        lv_obj_set_pos(ui_BTN, -13, -160 + i * 80);
        lv_obj_set_align(ui_BTN, LV_ALIGN_CENTER);
        //lv_obj_add_flag(ui_BTN_All, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
        //lv_obj_clear_flag(ui_BTN_All, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_radius(ui_BTN, 25, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(ui_BTN, lv_color_hex(0xFF3700), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(ui_BTN, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_color(ui_BTN, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_opa(ui_BTN, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_radius(ui_BTN, 25, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_bg_color(ui_BTN, lv_color_hex(0xFF3700), LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_bg_opa(ui_BTN, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_shadow_color(ui_BTN, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_shadow_opa(ui_BTN, 0, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_group_add_obj(Category_Group, ui_BTN);
        lv_obj_add_event_cb(ui_BTN, base_event_handler, LV_EVENT_KEY, NULL);

        // ui_LAB
        lv_obj_t* ui_LAB = lv_label_create(ui_BTN);
        lv_obj_set_size(ui_LAB, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_pos(ui_LAB, 25, 0);
        lv_obj_set_align(ui_LAB, LV_ALIGN_CENTER);
        lv_label_set_text(ui_LAB, lab_text[i]);
        lv_obj_set_style_text_color(ui_LAB, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(ui_LAB, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(ui_LAB, &ui_font_MyFont34, LV_PART_MAIN | LV_STATE_DEFAULT);

        // ui_IMG
        lv_obj_t* ui_IMG = lv_img_create(ui_BTN);
        lv_img_set_src(ui_IMG, &ui_img_arrow_right_png);
        lv_obj_set_size(ui_IMG, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_pos(ui_IMG, 104, 0);
        lv_obj_set_align(ui_IMG, LV_ALIGN_CENTER);
        //lv_obj_add_flag(ui_IMG_All, LV_OBJ_FLAG_HIDDEN);
        //lv_obj_add_flag(ui_IMG, LV_OBJ_FLAG_ADV_HITTEST);
        //lv_obj_clear_flag(ui_IMG, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_img_recolor(ui_IMG, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_img_recolor_opa(ui_IMG, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
}

static void DrawCell(lv_obj_t* ui_BTN, lv_coord_t w, lv_coord_t h, lv_coord_t x, lv_coord_t y, const void* pic, const char* str)
{
    lv_obj_set_size(ui_BTN, w, h);
    lv_obj_set_pos(ui_BTN, x, y);
    lv_obj_set_align(ui_BTN, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_BTN, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_BTN, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_BTN, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_BTN, lv_color_hex(0xE23920), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_BTN, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(ui_BTN, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(ui_BTN, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_BTN, 20, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_bg_color(ui_BTN, lv_color_hex(0xE23920), LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_bg_opa(ui_BTN, 255, LV_PART_MAIN | LV_STATE_FOCUSED);

    lv_obj_t* ui_IMG = lv_img_create(ui_BTN);
    lv_img_set_src(ui_IMG, pic);
    lv_obj_set_size(ui_IMG, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_pos(ui_IMG, 0, -25);
    lv_obj_set_align(ui_IMG, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_IMG, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_IMG, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* ui_LAB = lv_label_create(ui_BTN);
    lv_obj_set_size(ui_LAB, w, 40);
    lv_obj_set_pos(ui_LAB, 0, 62);
    lv_obj_set_align(ui_LAB, LV_ALIGN_CENTER);
    lv_label_set_long_mode(ui_LAB, LV_LABEL_LONG_SCROLL_CIRCULAR);//LV_LABEL_LONG_SCROLL_CIRCULAR  LV_LABEL_LONG_DOT
    lv_label_set_text(ui_LAB, str);
    lv_obj_set_style_text_align(ui_LAB, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LAB, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);
}

static void ShowDisk(void)
{
    static const char* disk[] = { "C盘", "D盘", "E盘"};
    //step1. opendir
    ShowDisk_Falg = true;
    lv_obj_t* ui_BTN = lv_btn_create(ui_File_List_Panel);
    DrawCell(ui_BTN, 155, 180, FileListPanelStartPos_x, FileListPanelStartPos_y, &ui_img_delivery_png, "返回上一级");
    lv_group_add_obj(File_List_Group, ui_BTN);
    lv_obj_add_event_cb(ui_BTN, enter_disk_handler, LV_EVENT_KEY, NULL);
    DiskCnt = 3;
    for (int i = 0; i < DiskCnt; i++) {
        ui_BTN = lv_btn_create(ui_File_List_Panel);
        lv_coord_t x = FileListPanelStartPos_x + ((i + 1) % 6) * FileListPanelOffset_x;
        lv_coord_t y = FileListPanelStartPos_y + ((i + 1) / 6) * FileListPanelOffset_y;
        DrawCell(ui_BTN, 155, 180, x, y, &ui_img_drive_png, disk[i]);
        lv_group_add_obj(File_List_Group, ui_BTN);
        lv_obj_add_event_cb(ui_BTN, enter_disk_handler, LV_EVENT_KEY, NULL);
    }
}

static void ShowFile(char *path)
{
    static const lv_img_dsc_t* image_src[] = {
        & ui_img_delivery_png ,
        & ui_img_drive_png,
        & ui_img_folder2_png ,
        & ui_img_filetype_mp4_png ,
        & ui_img_filetype_mp3_png ,
        & ui_img_filetype_jpg_png ,
        & ui_img_filetype_text_png ,
        & ui_img_filetype_other_png,
        & ui_img_filetype_other_png,
        & ui_img_filetype_other_png,
        & ui_img_filetype_other_png,
        & ui_img_filetype_other_png,
        & ui_img_filetype_other_png,
        & ui_img_filetype_other_png,
        & ui_img_filetype_other_png,
        & ui_img_filetype_other_png,
        & ui_img_filetype_other_png,
        & ui_img_filetype_other_png,
        & ui_img_filetype_other_png,
        & ui_img_filetype_other_png,
        & ui_img_filetype_other_png,
        & ui_img_filetype_other_png,
        & ui_img_filetype_other_png,
        & ui_img_filetype_other_png,
        & ui_img_filetype_other_png,
        & ui_img_filetype_other_png,
        & ui_img_filetype_other_png,
        & ui_img_filetype_other_png,
        & ui_img_filetype_other_png,
        & ui_img_filetype_other_png
    };
    static const char* lab_text[] = {
        "返回上一级",
        "C盘",
        "高清电影",
        "莫斯科不相信眼泪.mp4",
        "孤勇者.mp3",
        "向日葵.png",
        "钢铁是怎样炼成的.txt",
        "other",
        "other",
        "other",
        "other",
        "other",
        "other",
        "other",
        "other",
        "other",
        "other",
        "other",
        "other",
        "other",
        "other",
        "other",
        "other",
        "other",
        "other",
        "other",
        "other",
        "other",
        "other",
        "other"
    };
    //openddir(path);
    ShowDisk_Falg = false;
    //lv_group_remove_obj(ui_File_List_Panel);
    lv_obj_t* ui_BTN = lv_btn_create(ui_File_List_Panel);
    DrawCell(ui_BTN, 155, 180, FileListPanelStartPos_x, FileListPanelStartPos_y, &ui_img_delivery_png, "返回上一级");
    lv_group_add_obj(File_List_Group, ui_BTN);
    lv_obj_add_event_cb(ui_BTN, base_event_handler, LV_EVENT_KEY, NULL);
    int n = 20;
    for (int i = 0; i < n; i++) {
        lv_coord_t x = FileListPanelStartPos_x + ((i + 1) % 6) * FileListPanelOffset_x;
        lv_coord_t y = FileListPanelStartPos_y + ((i + 1) / 6) * FileListPanelOffset_y;
        ui_BTN = lv_btn_create(ui_File_List_Panel);
        DrawCell(ui_BTN, 140, 180, x, y, image_src[i + 2], lab_text[i+2]);
        lv_group_add_obj(File_List_Group, ui_BTN);
        lv_obj_add_event_cb(ui_BTN, base_event_handler, LV_EVENT_KEY, NULL);
    }
}

static void CreateFilePanel(lv_obj_t* parent)
{
    // ui_File_List_Panel
    ui_File_List_Panel = lv_obj_create(parent);
    lv_obj_set_size(ui_File_List_Panel, 1000, 580);
    lv_obj_set_pos(ui_File_List_Panel, 110, 20);
    lv_obj_set_align(ui_File_List_Panel, LV_ALIGN_CENTER);
    //lv_obj_clear_flag(ui_File_List_Panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_File_List_Panel, lv_color_hex(0x0D6D96), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_File_List_Panel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_File_List_Panel, lv_color_hex(0x009DFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_File_List_Panel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_File_List_Panel, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_group_add_obj(File_List_Group, ui_File_List_Panel);
}

static void LoadMedia(void)
{
    lv_disp_load_scr(ui_MediaScreen);
}

static void MediaWait(void)
{
    do {
        usleep(5000);
    } while (CurrentScreen == MediaScreen);
}

static void MediaClose(void)
{
    lv_obj_del(ui_MediaScreen);
}

static void MediaInit(lv_obj_t* parent, void *param)
{
    (void)param;

    ui_MediaScreen = lv_obj_create(parent);
    lv_obj_set_size(ui_MediaScreen, 1280, 720);
    //设置背景
    lv_obj_set_style_bg_color(ui_MediaScreen, lv_color_hex(0x0C9D89), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_MediaScreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_MediaScreen, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_MediaScreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_LAB_Path
    lv_obj_t* ui_LAB_Path = lv_label_create(ui_MediaScreen);
    lv_obj_set_size(ui_LAB_Path, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_pos(ui_LAB_Path, -336, -289);
    lv_obj_set_align(ui_LAB_Path, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LAB_Path, "路径：");
    lv_obj_set_style_text_color(ui_LAB_Path, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LAB_Path, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LAB_Path, &ui_font_MyFont34, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_LAB_Real_Path
    lv_obj_t* ui_LAB_Real_Path = lv_label_create(ui_MediaScreen);
    lv_obj_set_size(ui_LAB_Real_Path, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_pos(ui_LAB_Real_Path, 100, -289);
    lv_obj_set_align(ui_LAB_Real_Path, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LAB_Real_Path, "");
    lv_obj_set_style_text_color(ui_LAB_Real_Path, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LAB_Real_Path, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_LAB_Real_Path, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LAB_Real_Path, &ui_font_MyFont34, LV_PART_MAIN | LV_STATE_DEFAULT);

    GroupInit();
    CreateCategoryPanel(ui_MediaScreen);
    CreateFilePanel(ui_MediaScreen);
    ShowDisk();

    lv_group_set_default(Category_Group);
    #ifdef HOST_GCC
    keypad_indev = NULL;
    for (;;) {
        keypad_indev = lv_indev_get_next(keypad_indev);
        if (keypad_indev->driver->type == LV_INDEV_TYPE_KEYPAD || keypad_indev == NULL) {
            lv_indev_set_group(keypad_indev, Category_Group);
            break;
        }
    }
    #else
    keypad_indev = lv_indev_get_next(NULL);
    lv_indev_set_group(keypad_indev, Category_Group);
    #endif
    lv_group_focus_obj(lv_obj_get_child(ui_Category_Panel, 0));
}

window MediaWindow = {
    .ScreenInit = MediaInit,
    .ScreenLoad = LoadMedia,
    .ScreenWait = MediaWait,
    .ScreenClose = MediaClose
};

