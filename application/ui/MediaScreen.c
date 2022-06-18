/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-05-23 13:51:24
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-06-18 18:06:04
 * @FilePath: /gui/application/ui/MediaScreen.c
 * @Description: None
 * @other: None
 */
#include <unistd.h>
#include <string.h>
#include "MediaScreen.h"
#include "application/windows.h"
#include "MediaFile.h"
#include "MediaCom.h"
#include "Video.h"
#ifdef HCCHIP_GCC
#include "hcapi/media_player.h"
#endif

static const lv_coord_t FileListPanelWidth = 1010;
static const lv_coord_t FileListPanelHeight = 615;
static const lv_coord_t FileWidth = 150;
static const lv_coord_t FileHeight = 180;
static const uint16_t FileListPanelRowNumber = FileListPanelWidth/FileWidth;

int lsat_focused_item = 0;

typedef enum {
    All,
    Vedio = FILE_VIDEO,
    Music,
    Photo,
    Text,
    CategoryNumber
} CategoryList;

static CategoryList FileFilter;

lv_obj_t* ui_MediaScreen;
lv_obj_t* ui_Category_Panel;
lv_obj_t* ui_File_List_Panel;
lv_obj_t* ui_LAB_Real_Path;
lv_group_t* Category_Group;
lv_group_t * default_group;
lv_group_t* File_List_Group;
lv_indev_t* keypad_indev;

lv_obj_t* CurrentMediaWindow;

static void ShowDisk(void);
static void ShowFileList(FileList *file_list);
static void DrawCell(lv_obj_t* ui_BTN, lv_coord_t w, lv_coord_t h, const void* pic, const char* str);
static void ReturnUpper(void);
static void RefreshFileWithFile(CategoryList filter);
static void return_handler(lv_event_t* event);
static void FilterFile(CategoryList file_type);

static void key_base_event_handler(lv_obj_t* target, lv_obj_t* parents)
{
    static uint16_t Category_Panel_Index = 0;
    uint32_t value = lv_indev_get_key(lv_indev_get_act());
    lv_group_t* group = (lv_group_t*)lv_obj_get_group(target);
    switch (value)
    {
    case LV_KEY_UP:
        if (ui_Category_Panel == parents) {
            uint16_t number = lv_obj_get_child_cnt(parents);
            Category_Panel_Index = (Category_Panel_Index == 0) ? number : --Category_Panel_Index;
            lv_group_focus_obj(lv_obj_get_child(ui_Category_Panel, Category_Panel_Index));
        }
        else if (ui_File_List_Panel == parents) {
            int index = lv_obj_get_index(target);
            if (index - FileListPanelRowNumber >= 0)
                lv_group_focus_obj(lv_obj_get_child(ui_File_List_Panel, index - FileListPanelRowNumber));
        }
        break;
    case LV_KEY_DOWN:
        if (ui_Category_Panel == parents) {
            uint16_t number = lv_obj_get_child_cnt(parents);
            Category_Panel_Index = (++Category_Panel_Index)%number;
            lv_group_focus_obj(lv_obj_get_child(ui_Category_Panel, Category_Panel_Index));
        }
        else if (ui_File_List_Panel == parents) {
            int index = lv_obj_get_index(target);
            if ((uint32_t)(index + FileListPanelRowNumber) < lv_obj_get_child_cnt(ui_File_List_Panel))
                lv_group_focus_obj(lv_obj_get_child(ui_File_List_Panel, index + FileListPanelRowNumber));
            else if ( ((uint32_t)index/FileListPanelRowNumber)*FileListPanelRowNumber + FileListPanelRowNumber < lv_obj_get_child_cnt(ui_File_List_Panel)) {
                lv_group_focus_obj(lv_obj_get_child(ui_File_List_Panel, lv_obj_get_child_cnt(ui_File_List_Panel) - 1));
            }
        }
        break;
    case LV_KEY_LEFT:
        if (ui_File_List_Panel == parents) {
            if (lv_obj_get_child(ui_File_List_Panel, 0) != target) {
                lv_group_focus_prev(group);
            }
            else {
                lv_group_focus_obj(ui_File_List_Panel);//隐藏file list panel 的聚焦状态
                lv_group_set_default(Category_Group);
                lv_indev_set_group(keypad_indev, Category_Group);
            }
        }
    break;
    case LV_KEY_RIGHT:
        if (ui_Category_Panel == parents) {
            lv_group_set_default(File_List_Group);
            lv_indev_set_group(keypad_indev, File_List_Group);
            lv_group_focus_obj(lv_obj_get_child(ui_File_List_Panel, 0));
        }
        else if (ui_File_List_Panel == parents) {
            if (lv_obj_get_child(ui_File_List_Panel, lv_obj_get_child_cnt(ui_File_List_Panel) - 1) != target)
                lv_group_focus_next(group);
            else
                lv_group_focus_obj(lv_obj_get_child(ui_File_List_Panel, 0));
        }
    break;
    case LV_KEY_ESC:
        if (!lv_obj_is_valid(CurrentMediaWindow) && !IsRootPath(current_path)) {
            ReturnUpper();
            //lv_group_focus_obj(lv_obj_get_child(ui_File_List_Panel, lsat_focused_item));
            lv_group_focus_obj(lv_obj_get_child(ui_File_List_Panel, 0));
        }
        break;

    default:
        break;
    }
}

static void focused_base_event_handler(lv_obj_t* target, lv_obj_t* parents)
{
     if (parents == ui_File_List_Panel) {
        if (lv_obj_get_child(parents, 0) != target) {
            lv_label_set_long_mode(target->spec_attr->children[1], LV_LABEL_LONG_SCROLL_CIRCULAR);//LV_LABEL_LONG_SCROLL_CIRCULAR
        }
        /*
        static lv_obj_t* last_target = NULL;
        if (lv_obj_is_valid(last_target))//设置上一个对象，为点模式
            lv_label_set_long_mode(last_target->spec_attr->children[1], LV_LABEL_LONG_DOT);//LV_LABEL_LONG_SCROLL_CIRCULAR

        if (target->spec_attr->children[1] != NULL) {
            lv_label_set_long_mode(target->spec_attr->children[1], LV_LABEL_LONG_SCROLL_CIRCULAR);//LV_LABEL_LONG_SCROLL_CIRCULAR
            last_target = target;
        }*/
    }
    else if (parents == ui_Category_Panel) {
        FileFilter = lv_obj_get_index(target);
        FilterFile(FileFilter);
    }

}

static void defocused_base_event_handler(lv_obj_t* target, lv_obj_t* parents)
{
     if (parents == ui_File_List_Panel) {
        if (lv_obj_get_child(parents, 0) != target) {
            lv_label_set_long_mode(target->spec_attr->children[1], LV_LABEL_LONG_DOT);//LV_LABEL_LONG_SCROLL_CIRCULAR
        }
    }
}

static void category_list_handler(lv_event_t* event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t* target = lv_event_get_target(event);
    lv_obj_t* parents = lv_obj_get_parent(target);
    if (LV_EVENT_KEY == code) {
        key_base_event_handler(target, parents);
    }
    else if (LV_EVENT_FOCUSED == code) {
        focused_base_event_handler(target, parents);
    }
}

static void file_list_handler(lv_event_t* event)
{
    //static lv_obj_t* last_target = NULL;
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t* target = lv_event_get_target(event);
    lv_obj_t* parents = lv_obj_get_parent(target);
    if (LV_EVENT_KEY == code) {
        uint32_t value = lv_indev_get_key(lv_indev_get_act());
        //lv_group_t* group = (lv_group_t*)lv_obj_get_group(target);
        if (LV_KEY_ENTER == value) {
            if (target->user_data != NULL) {
                switch (((FileStr *)(target->user_data))->type)
                {
                case FILE_DIR:
                    lsat_focused_item = lv_obj_get_index(target);
                    lv_obj_clean(ui_File_List_Panel);
                    strcat(current_path, "/");
                    strcat(current_path, ((FileStr *)(target->user_data))->name);
                    ShowFileList(GetFileList(current_path));
                    lv_group_focus_obj(lv_obj_get_child(ui_File_List_Panel, 0));
                    break;
                case FILE_VIDEO:
                    CurrentMediaWindow = creat_video_window(target);
                    break;
                        
                case FILE_MUSIC:
                    CurrentMediaWindow = creat_music_window(target);
                    break;
                case FILE_PHOTO:

                    break;
                case FILE_TEXT:

                    break;
                
                default:
                    break;
                }
            }
            //last_target = NULL;
        }
        else {
            key_base_event_handler(target, parents);
        }
    }
    else if (LV_EVENT_FOCUSED == code) {
        focused_base_event_handler(target,parents);
    }
    else if (LV_EVENT_DEFOCUSED == code) {
        defocused_base_event_handler(target,parents);
    }
}

static void return_handler(lv_event_t* event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t* target = lv_event_get_target(event);
    lv_obj_t* parents = lv_obj_get_parent(target);
    if (LV_EVENT_KEY == code) {
        uint32_t value = lv_indev_get_key(lv_indev_get_act());
        if (LV_KEY_ENTER == value) {
            if ( !IsRootPath(current_path)) {
                ReturnUpper();
                //lv_group_focus_obj(lv_obj_get_child(ui_File_List_Panel, lsat_focused_item));
                lv_group_focus_obj(lv_obj_get_child(ui_File_List_Panel, 0));
            }
        }
        else {
            key_base_event_handler(target, parents);
        }
    }
    else if (LV_EVENT_FOCUSED == code) {
        focused_base_event_handler(target,parents);
    }
}

static void FilterFile(CategoryList filter_type)
{
    static const lv_img_dsc_t* image_src[] = {
        NULL,
        &ui_img_filetype_mp4_png,
        &ui_img_filetype_mp3_png,
        &ui_img_filetype_jpg_png,
        &ui_img_filetype_text_png };
    static CategoryList last_filter_type = All;
    
    if (last_filter_type != filter_type) {
        FileStr* file = NULL;
        lv_obj_t* child = NULL;
        uint16_t dir_number = GetDirNumber(current_list);
        uint16_t non_dir_number = GetNonDirNumber(current_list);
        uint16_t media_number = GetMediaListSize(filter_type);
        int first_file_index = dir_number + 1;//第一个非文件夹对象
        int end_index = first_file_index + ((filter_type == All) ? non_dir_number : media_number);
        GetNextFile(NULL);
        for (int i = first_file_index; i < end_index; i++) {
            file = GetNextFile(current_list->NonDirList);
            if (file == NULL) return;
            if (filter_type != All) {//筛选出指定类型
                while( file != NULL && file->type != filter_type) {
                    file = GetNextFile(current_list->NonDirList);
                }
            }
            child = lv_obj_get_child(ui_File_List_Panel, i);
            if (child != NULL) {
                child->user_data = file;
                lv_img_set_src(child->spec_attr->children[0], image_src[file->type]);
                lv_label_set_text(child->spec_attr->children[1], file->name);
            }
            else {
                child = lv_btn_create(ui_File_List_Panel);
                child->user_data = file;
                DrawCell(child, FileWidth, FileHeight, image_src[file->type], file->name);
                lv_group_add_obj(File_List_Group, child);
                lv_obj_add_event_cb(child, file_list_handler, LV_EVENT_ALL, NULL);
            }
        }
        if (filter_type != All) {
            uint16_t obj_number = lv_obj_get_child_cnt(ui_File_List_Panel);
            for (int i = end_index; i < obj_number; i++) {
                lv_obj_del_async(lv_obj_get_child(ui_File_List_Panel, i));
            }
        }
        last_filter_type = filter_type;
    }
}

static void GroupInit(void)
{
    Category_Group = lv_group_create();
    File_List_Group = lv_group_create();
    default_group = lv_group_get_default();
    lv_group_remove_all_objs(default_group);
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
    lv_group_remove_all_objs(Category_Group);
    for (int i = 0; i < CategoryNumber; i++) {
        lv_obj_t* ui_BTN = lv_btn_create(ui_Category_Panel);
        lv_obj_set_size(ui_BTN, 265, 50);
        lv_obj_set_pos(ui_BTN, -30, -160 + i * 80);
        lv_obj_set_align(ui_BTN, LV_ALIGN_CENTER);
        //lv_obj_add_flag(ui_BTN, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
        lv_obj_clear_flag(ui_BTN, LV_OBJ_FLAG_SCROLLABLE);
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
        lv_obj_add_event_cb(ui_BTN, category_list_handler, LV_EVENT_ALL, NULL);

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
    FileFilter = All;
}

static void DrawCell(lv_obj_t* ui_BTN, lv_coord_t w, lv_coord_t h, const void* pic, const char* str)
{
    lv_obj_set_size(ui_BTN, w, h);
    //lv_obj_set_pos(ui_BTN, x, y);
    lv_obj_set_align(ui_BTN, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_BTN, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    lv_obj_set_style_radius(ui_BTN, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_BTN, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_obj_set_style_shadow_color(ui_BTN, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(ui_BTN, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_obj_set_style_bg_color(ui_BTN, lv_color_hex(0xE23920), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_BTN, 20, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_bg_color(ui_BTN, lv_color_hex(0xE23920), LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_bg_opa(ui_BTN, 255, LV_PART_MAIN | LV_STATE_FOCUSED); 

    //lv_obj_set_style_shadow_color(ui_BTN, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_FOCUSED);
    //lv_obj_set_style_shadow_opa(ui_BTN, 0, LV_PART_MAIN | LV_STATE_FOCUSED);

    /*lv_obj_clear_flag(ui_BTN, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_BTN, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_BTN, lv_color_hex(0xE23920), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_BTN, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_BTN, 20, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_bg_color(ui_BTN, lv_color_hex(0xE23920), LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_bg_opa(ui_BTN, 255, LV_PART_MAIN | LV_STATE_FOCUSED);*/

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
    lv_label_set_long_mode(ui_LAB, LV_LABEL_LONG_DOT);//LV_LABEL_LONG_SCROLL_CIRCULAR
    lv_label_set_text(ui_LAB, str);
    lv_obj_set_style_text_align(ui_LAB, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LAB, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);
}

static void ShowFileList(FileList *file_list)
{
    //lv_group_remove_obj(ui_File_List_Panel);
    lv_obj_t* ui_back = lv_btn_create(ui_File_List_Panel);
    ui_back->user_data = NULL;
    DrawCell(ui_back, FileWidth, FileHeight, &ui_img_delivery_png, "返回上一级");
    lv_group_add_obj(File_List_Group, ui_back);
    lv_obj_add_event_cb(ui_back, return_handler, LV_EVENT_ALL, NULL);
    //current_list = GetFileList(path);
    int FileCnt = GetFileNumber(file_list);
    DestroyAllMediaList();
    GetNextFileFromFileList(NULL);//清理前一次使用痕迹
    for (int i = 0; i < FileCnt; i++) {
        lv_img_dsc_t* image;
        FileStr* file = GetNextFileFromFileList(file_list);
        switch (file->type)
        {
        case FILE_DIR:
            if (IsRootPath(current_path))
                image = (lv_img_dsc_t* )&ui_img_drive_png;
            else
                image = (lv_img_dsc_t* )&ui_img_folder2_png;
            break;
        case FILE_VIDEO:
            //AddToVideoList(video_list, file->name);
            AddToMediaList(MEDIA_VIDEO, file->name);
            image = (lv_img_dsc_t* )&ui_img_filetype_mp4_png;
            break;
        case FILE_MUSIC:
            AddToMediaList(MEDIA_MUSIC, file->name);
            image = (lv_img_dsc_t* )&ui_img_filetype_mp3_png;
            break;
        case FILE_PHOTO:
            AddToMediaList(MEDIA_PHOTO, file->name);
            image = (lv_img_dsc_t* )&ui_img_filetype_jpg_png;
            break;
        case FILE_TEXT:
            AddToMediaList(MEDIA_TEXT, file->name);
            image = (lv_img_dsc_t* )&ui_img_filetype_text_png;
            break;
        default:
            image = (lv_img_dsc_t* )&ui_img_filetype_other_png;
            break;
        }
        if (FileFilter != All) {//必须放在 AddToMediaList 函数后面，因为就算过滤掉此类型，也要将其加入媒体文件列表里面
            if ((file->type != FILE_DIR) && ((uint8_t)file->type != (uint8_t)FileFilter)) {
                continue;
            }
        }
        lv_obj_t* ui_BTN = lv_btn_create(ui_File_List_Panel);
        ui_BTN->user_data = file;
        DrawCell(ui_BTN, FileWidth, FileHeight, image, file->name);
        lv_group_add_obj(File_List_Group, ui_BTN);
        lv_obj_add_event_cb(ui_BTN, file_list_handler, LV_EVENT_ALL, NULL);
    }
    lv_label_set_text(ui_LAB_Real_Path, current_path);
}

static void RefreshFileWithFile(CategoryList filter)
{
}

static void ReturnUpper(void)
{
    lv_obj_clean(ui_File_List_Panel);
    char *file_extension = strrchr(current_path, '/');
    while(*file_extension != 0) {
        *file_extension = 0;
        file_extension++;
    }
    DestroyAllMediaList();
    ShowFileList(GetPreviousFileList());
}

static void CreateFilePanel(lv_obj_t* parent)
{
    // ui_File_List_Panel
    ui_File_List_Panel = lv_obj_create(parent);
    lv_obj_set_size(ui_File_List_Panel, 1010, 600);
    lv_obj_set_pos(ui_File_List_Panel, 110, 40);
    lv_obj_set_align(ui_File_List_Panel, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_File_List_Panel, LV_FLEX_FLOW_ROW_WRAP);
    //lv_obj_clear_flag(ui_File_List_Panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_File_List_Panel, lv_color_hex(0x0D6D96), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_File_List_Panel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_File_List_Panel, lv_color_hex(0x009DFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_File_List_Panel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_File_List_Panel, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_group_add_obj(File_List_Group, ui_File_List_Panel);

    strcat(current_path, media_dir);
}

static void LoadMedia(void)
{
    lv_disp_load_scr(ui_MediaScreen);
}

static void MediaWait(void)
{
    do {
        #ifdef HCCHIP_GCC
        MediaMonitorTask();
        #endif
        usleep(5000);
    } while (CurrentScreen == MediaScreen);
}

static void MediaClose(void)
{
    lv_obj_del(ui_MediaScreen);
    
    lv_group_set_default(default_group);
    lv_indev_set_group(keypad_indev, default_group);
    lv_group_del(Category_Group);
    lv_group_del(File_List_Group);

}

static void MediaInit(lv_obj_t* parent, void *param)
{
    (void)param;

    ui_MediaScreen = lv_obj_create(parent);
    lv_obj_set_size(ui_MediaScreen, 1280, 720);
    lv_obj_clear_flag(ui_MediaScreen, LV_OBJ_FLAG_SCROLLABLE);
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
    ui_LAB_Real_Path = lv_label_create(ui_MediaScreen);
    lv_obj_set_size(ui_LAB_Real_Path, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_pos(ui_LAB_Real_Path, 360, -289);
    lv_obj_set_align(ui_LAB_Real_Path, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_LAB_Real_Path, "");
    lv_obj_set_style_text_color(ui_LAB_Real_Path, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LAB_Real_Path, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_LAB_Real_Path, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LAB_Real_Path, &ui_font_MyFont34, LV_PART_MAIN | LV_STATE_DEFAULT);

    GroupInit();
    CreateCategoryPanel(ui_MediaScreen);
    CreateFilePanel(ui_MediaScreen);
    MediaFileInit();
    ShowFileList(GetFileList(media_dir));

    lv_group_set_default(Category_Group);
    
    keypad_indev = NULL;
    for (;;) {
        keypad_indev = lv_indev_get_next(keypad_indev);
        if (keypad_indev == NULL)
            break;
        if (keypad_indev->driver->type == LV_INDEV_TYPE_KEYPAD) {
            lv_indev_set_group(keypad_indev, Category_Group);
            break;
        }
    }
    lv_group_focus_obj(lv_obj_get_child(ui_Category_Panel, 0));
}

window MediaWindow = {
    .ScreenInit = MediaInit,
    .ScreenLoad = LoadMedia,
    .ScreenWait = MediaWait,
    .ScreenClose = MediaClose
};
