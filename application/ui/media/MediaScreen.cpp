/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-05-23 13:51:24
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-08-31 12:41:07
 * @FilePath: /gui/application/ui/media/MediaScreen.cpp
 * @Description: None
 * @other: None
 */
#include <unistd.h>
#include <string.h>
#include <stack>
#include <list>
#include "MediaScreen.h"
#include "application/windows.h"
#include "MediaCom.h"
#include "Video.h"
#include "Music.h"
#include "Photo.h"
#include "Text.h"
#ifdef HCCHIP_GCC
#include "hcapi/media_player.h"
#endif
#include "application/ui/Volume.h"
#include "application/key_map.h"

#define FileListPanelWidth 1010
#define FileListPanelHeight 615
#define FileWidth 150
#define FileHeight 180

MediaFileCategoryList FileFilter;

static const uint16_t FileListPanelRowNumber = FileListPanelWidth/FileWidth;
static lv_obj_t* MediaRootScreen;
lv_obj_t* ui_Category_Panel;
lv_obj_t* ui_File_List_Panel;
lv_obj_t* ui_LAB_Real_Path;
static lv_group_t* CategoryGroup;
static lv_group_t* FileListGroup;
lv_obj_t* CurrentMediaWindow;
std::stack<lv_obj_t*, std::list<lv_obj_t*>> FileListPanelStack;

static void CreateFilePanel(lv_obj_t* parent);
static void ShowDisk(void);
static void ShowFileList(FileList *file_list);
static void DrawCell(lv_obj_t* ui_BTN, lv_coord_t w, lv_coord_t h, const void* pic, const char* str);
static void ReturnUpper(void);
static void RefreshFileWithFile(MediaFileCategoryList filter);
static void return_handler(lv_event_t* event);
static void FilterFile(MediaFileCategoryList file_type);
static void ExitMedia(ActiveScreen screen);

static void key_base_event_handler(lv_obj_t* target, lv_obj_t* parents)
{
    uint32_t value = lv_indev_get_key(lv_indev_get_act());
    lv_group_t* group = (lv_group_t*)lv_obj_get_group(target);
    switch (value)
    {
    case LV_KEY_UP:
        if (ui_Category_Panel == parents) {
            lv_group_focus_prev(group);
        }
        else if (ui_File_List_Panel == parents) {
            int index = lv_obj_get_index(target);
            if (index - FileListPanelRowNumber >= 0)
                lv_group_focus_obj(lv_obj_get_child(ui_File_List_Panel, index - FileListPanelRowNumber));
        }
        break;
    case LV_KEY_DOWN:
        if (ui_Category_Panel == parents) {
            lv_group_focus_next(group);
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
                set_group_activity(CategoryGroup);
            }
        }
    break;
    case LV_KEY_RIGHT:
        if (ui_Category_Panel == parents) {
            set_group_activity(FileListGroup);
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
        if(IsRootPath(current_path)) {
            ExitMedia(HomeScreen);
        }
        else if (!lv_obj_is_valid(CurrentMediaWindow)) {
            ReturnUpper();
        }
        break;
    case LV_KEY_VOLUME_UP:
    case LV_KEY_VOLUME_DOWN:
        SetVolume(value);
        break;

    default:
        break;
    }
}

static void focused_base_event_handler(lv_obj_t* target, lv_obj_t* parents)
{
     if (parents == ui_File_List_Panel) {
        if (lv_obj_get_child(parents, 0) != target) {
            lv_label_set_long_mode(target->spec_attr->children[1], LV_LABEL_LONG_SCROLL_CIRCULAR);
        }
    }
    else if (parents == ui_Category_Panel) {
        FileFilter = static_cast<MediaFileCategoryList>(lv_obj_get_index(target));
        FilterFile(FileFilter);
    }
}

static void defocused_base_event_handler(lv_obj_t* target, lv_obj_t* parents)
{
     if (parents == ui_File_List_Panel) {
        if (lv_obj_get_child(parents, 0) != target) {
            lv_label_set_long_mode(target->spec_attr->children[1], LV_LABEL_LONG_DOT);
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
                    FileListPanelStack.push(ui_File_List_Panel);
                    CreateFilePanel(MediaRootScreen);
                    set_group_activity(FileListGroup);
                    strcat(current_path, "/");
                    strcat(current_path, ((FileStr *)(target->user_data))->name);
                    ShowFileList(GetFileList(current_path));
                    lv_group_focus_obj(lv_obj_get_child(ui_File_List_Panel, 0));
                    break;
                case FILE_VIDEO:
                    CurrentMediaWindow = creat_video_window(((FileStr *)(target->user_data))->name);
                    break;
                        
                case FILE_MUSIC:
                    CurrentMediaWindow = creat_music_window(((FileStr *)(target->user_data))->name);
                    break;
                case FILE_PHOTO:
                    CurrentMediaWindow = creat_photo_window(((FileStr *)(target->user_data))->name);
                    break;
                case FILE_TEXT:
                    CurrentMediaWindow = creat_text_window(((FileStr *)(target->user_data))->name);
                    break;
                
                default:
                    break;
                }
            }
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

static void FilterFile(MediaFileCategoryList filter_type)
{
    static const lv_img_dsc_t* image_src[] = {
        NULL,
        &ui_img_movie_png,
        &ui_img_music_png,
        &ui_img_image_png,
        &ui_img_text_png,
        &ui_img_other_png };
    static MediaFileCategoryList last_filter_type = MediaFile_All;
    
    if (last_filter_type != filter_type) {
        FileStr* file = NULL;
        lv_obj_t* child = NULL;
        uint16_t dir_number = GetDirNumber(current_list);
        uint16_t non_dir_number = GetNonDirNumber(current_list);
        uint16_t media_number = GetMediaListSize(static_cast<MediaType>(filter_type));
        int first_file_index = dir_number + 1;//第一个非文件夹对象
        int end_index = first_file_index + ((filter_type == MediaFile_All) ? non_dir_number : media_number);
        GetNextFile(NULL);
        for (int i = first_file_index; i < end_index; i++) {
            do {
                file = GetNextFile(current_list->NonDirList);
            } while ((file != NULL) && (filter_type != MediaFile_All) && (file->type != (FileType)filter_type));
            if (file == NULL) break;
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
                lv_group_add_obj(FileListGroup, child);
                lv_obj_add_event_cb(child, file_list_handler, LV_EVENT_ALL, NULL);
            }
        }
        if (filter_type != MediaFile_All) {
            uint16_t obj_number = lv_obj_get_child_cnt(ui_File_List_Panel);
            for (int i = end_index; i < obj_number; i++) {
                lv_obj_del_async(lv_obj_get_child(ui_File_List_Panel, i));
            }
        }
        last_filter_type = filter_type;
    }
}

static void CreateCategoryPanel(lv_obj_t* parent)
{
    static const char* lab_text[] = {
        "media_category_p_all",
        "media_category_p_video",
        "media_category_p_music",
        "media_category_p_photo",
        "media_category_p_text" };
    ui_Category_Panel = lv_obj_create(parent);
    lv_obj_set_size(ui_Category_Panel, 252, 420);
    lv_obj_set_pos(ui_Category_Panel, -515, 0);
    lv_obj_set_align(ui_Category_Panel, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_Category_Panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(ui_Category_Panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_Category_Panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    CategoryGroup = create_new_group();
    set_group_activity(CategoryGroup);
    for (int i = 0; i < MediaFile_CategoryNumber; i++) {
        lv_obj_t* ui_BTN = lv_btn_create(ui_Category_Panel);
        lv_obj_set_size(ui_BTN, 265, 50);
        lv_obj_set_pos(ui_BTN, -30, -160 + i * 80);
        lv_obj_set_align(ui_BTN, LV_ALIGN_CENTER);
        lv_obj_clear_flag(ui_BTN, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_radius(ui_BTN, 25, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(ui_BTN, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_color(ui_BTN, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_opa(ui_BTN, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_radius(ui_BTN, 25, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_bg_color(ui_BTN, lv_color_hex(0xFF3700), LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_bg_opa(ui_BTN, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_shadow_color(ui_BTN, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_shadow_opa(ui_BTN, 0, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_group_add_obj(CategoryGroup, ui_BTN);
        lv_obj_add_event_cb(ui_BTN, category_list_handler, LV_EVENT_ALL, NULL);

        lv_obj_t* ui_LAB = lv_label_create(ui_BTN);
        lv_obj_set_size(ui_LAB, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_pos(ui_LAB, 10, 0);
        lv_obj_set_align(ui_LAB, LV_ALIGN_CENTER);
        ui_LAB->user_data = const_cast<char*>(lab_text[i]);
        lv_label_set_text(ui_LAB, _(lab_text[i]));
        lv_obj_set_style_text_color(ui_LAB, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(ui_LAB, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(ui_LAB, &ui_font_MyFont34, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_t* ui_IMG = lv_img_create(ui_BTN);
        lv_img_set_src(ui_IMG, &ui_img_arrow_right_png);
        lv_obj_set_size(ui_IMG, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_pos(ui_IMG, 104, 0);
        lv_obj_set_align(ui_IMG, LV_ALIGN_CENTER);
        lv_obj_set_style_img_recolor(ui_IMG, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_img_recolor_opa(ui_IMG, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    //FileFilter = MediaFile_All;
    lv_group_focus_obj(lv_obj_get_child(ui_Category_Panel, FileFilter));
}

static void DrawCell(lv_obj_t* ui_BTN, lv_coord_t w, lv_coord_t h, const void* pic, const char* str)
{
    lv_obj_set_size(ui_BTN, w, h);
    lv_obj_set_align(ui_BTN, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_BTN, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    lv_obj_set_style_radius(ui_BTN, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_BTN, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
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
    lv_label_set_long_mode(ui_LAB, LV_LABEL_LONG_DOT);//LV_LABEL_LONG_SCROLL_CIRCULAR
    lv_label_set_text(ui_LAB, str);
    lv_obj_set_style_text_align(ui_LAB, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LAB, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);
}

static void ShowFileList(FileList *file_list)
{
    lv_obj_t* ui_back = lv_btn_create(ui_File_List_Panel);
    ui_back->user_data = const_cast<char*>("media_file_p_return");
    DrawCell(ui_back, FileWidth, FileHeight, &ui_img_delivery_png, _("media_file_p_return"));
    lv_group_add_obj(FileListGroup, ui_back);
    lv_obj_add_event_cb(ui_back, return_handler, LV_EVENT_ALL, NULL);
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
            AddToMediaList(MEDIA_VIDEO, file->name);
            image = (lv_img_dsc_t* )&ui_img_movie_png;
            break;
        case FILE_MUSIC:
            AddToMediaList(MEDIA_MUSIC, file->name);
            image = (lv_img_dsc_t* )&ui_img_music_png;
            break;
        case FILE_PHOTO:
            AddToMediaList(MEDIA_PHOTO, file->name);
            image = (lv_img_dsc_t* )&ui_img_image_png;
            break;
        case FILE_TEXT:
            AddToMediaList(MEDIA_TEXT, file->name);
            image = (lv_img_dsc_t* )&ui_img_text_png;
            break;
        default:
            image = (lv_img_dsc_t* )&ui_img_other_png;
            break;
        }
        if (FileFilter != MediaFile_All) {//必须放在 AddToMediaList 函数后面，因为就算过滤掉此类型，也要将其加入媒体文件列表里面
            if ((file->type != FILE_DIR) && ((uint8_t)file->type != (uint8_t)FileFilter)) {
                continue;
            }
        }
        lv_obj_t* ui_BTN = lv_btn_create(ui_File_List_Panel);
        ui_BTN->user_data = file;
        DrawCell(ui_BTN, FileWidth, FileHeight, image, file->name);
        lv_group_add_obj(FileListGroup, ui_BTN);
        lv_obj_add_event_cb(ui_BTN, file_list_handler, LV_EVENT_ALL, NULL);
    }
    lv_label_set_text(ui_LAB_Real_Path, current_path);
}

static void ReturnUpper(void)
{
    FileListGroup = delete_group(FileListGroup);
    lv_obj_del_async(ui_File_List_Panel);
    ui_File_List_Panel = FileListPanelStack.top();
    FileListPanelStack.pop();
    char *file_extension = strrchr(current_path, '/');
    while(*file_extension != 0) {
        *file_extension = 0;
        file_extension++;
    }
    lv_label_set_text(ui_LAB_Real_Path, current_path);
    FileList* file_list = GetPreviousFileList();
    int FileCnt = GetFileNumber(file_list);
    DestroyAllMediaList();
    GetNextFileFromFileList(NULL);//清理前一次使用痕迹
    for (int i = 0; i < FileCnt; i++) {
        FileStr* file = GetNextFileFromFileList(file_list);
        switch (file->type)
        {
        case FILE_VIDEO:
        case FILE_MUSIC:
        case FILE_PHOTO:
        case FILE_TEXT:
            AddToMediaList(static_cast<MediaType>(file->type), file->name);

        default:
            break;
        }
    }
}

static void CreateFilePanel(lv_obj_t* parent)
{
    ui_File_List_Panel = lv_obj_create(parent);
    lv_obj_set_size(ui_File_List_Panel, 1010, 600);
    lv_obj_set_pos(ui_File_List_Panel, 110, 25);
    lv_obj_set_align(ui_File_List_Panel, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_File_List_Panel, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_bg_color(ui_File_List_Panel, lv_color_hex(0x0D6D96), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_File_List_Panel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_File_List_Panel, lv_color_hex(0x009DFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_File_List_Panel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_File_List_Panel, 6, LV_PART_MAIN | LV_STATE_DEFAULT);

    FileListGroup = create_new_group();
    lv_group_add_obj(FileListGroup, ui_File_List_Panel);
}

static void MediaInit(void)
{
    MediaRootScreen = lv_obj_create(nullptr);
    lv_obj_set_size(MediaRootScreen, 1280, 720);
    lv_obj_clear_flag(MediaRootScreen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(MediaRootScreen, lv_color_hex(0x0C9D89), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(MediaRootScreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(MediaRootScreen, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(MediaRootScreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* ui_LAB_Path = lv_label_create(MediaRootScreen);
    lv_obj_set_size(ui_LAB_Path, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_pos(ui_LAB_Path, -336, -300);
    lv_obj_set_align(ui_LAB_Path, LV_ALIGN_CENTER);
    ui_LAB_Path->user_data = const_cast<char*>("media_file_p_path");
    lv_label_set_text(ui_LAB_Path, _("media_file_p_path"));
    lv_obj_set_style_text_color(ui_LAB_Path, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LAB_Path, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LAB_Path, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LAB_Real_Path = lv_label_create(MediaRootScreen);
    lv_obj_set_size(ui_LAB_Real_Path, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_pos(ui_LAB_Real_Path, 360, -300);
    lv_obj_set_align(ui_LAB_Real_Path, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_LAB_Real_Path, "");
    lv_obj_set_style_text_color(ui_LAB_Real_Path, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LAB_Real_Path, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_LAB_Real_Path, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LAB_Real_Path, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);

    CreateFilePanel(MediaRootScreen);
    CreateCategoryPanel(MediaRootScreen);
    MediaFileInit();
    strcat(current_path, media_dir);
    ShowFileList(GetFileList(current_path));
}

static void LoadMedia(void)
{
    lv_scr_load_anim(MediaRootScreen, LV_SCR_LOAD_ANIM_NONE, 0, 0, true);
    //lv_scr_load_anim(MediaRootScreen, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0, true);
}

static void ExitMedia(ActiveScreen screen)
{
    DestroyAllMediaList();
    MediaFileDeInit();
    memset(current_path, 0, current_path_size );
    if (!FileListPanelStack.empty()) {
        FileListGroup = delete_group(FileListGroup);
        FileListPanelStack.pop();
    }
    delete_all_group();
    //lv_obj_del(MediaRootScreen);
    CurrentScreen = screen;
}

window MediaWindow = {
    .ScreenInit = MediaInit,
    .ScreenLoad = LoadMedia,
};
