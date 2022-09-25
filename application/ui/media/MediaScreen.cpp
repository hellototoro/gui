/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-05-23 13:51:24
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

/*********************** 全局变量 *************************/
MediaFileCategoryList FileFilter;

/*********************** static变量 *************************/
static constexpr uint16_t FileListPanelRowNumber = FileListPanelWidth/FileWidth;
static lv_obj_t* MediaRootScreen;
static lv_obj_t* MediaSubScreen;
static lv_obj_t* CategoryPanel;
static lv_obj_t* FileListPanel;
static lv_obj_t* RealPath;
static lv_group_t* CategoryGroup;
static lv_group_t* FileListGroup;
static MediaFileCategoryList last_filter_type;
std::stack<lv_obj_t*, std::list<lv_obj_t*>> FileListPanelStack;

/*********************** static函数 *************************/
static void CreateCategoryPanel(lv_obj_t* parent);
static void CreateFilePanel(lv_obj_t* parent, const char *path);
static void ShowFileList(FileList *file_list);
static void DrawCell(lv_obj_t* ui_BTN, lv_coord_t w, lv_coord_t h, const void* pic, const char* str);
static void ReturnUpper(void);
static void FilterFile(MediaFileCategoryList file_type);
static void ExitMedia(ActiveScreen screen);

static void key_base_event_handler(lv_event_t* event)
{
    lv_obj_t* target = lv_event_get_current_target(event);
    uint32_t value = lv_indev_get_key(lv_indev_get_act());
    lv_group_t* group = (lv_group_t*)lv_obj_get_group(target);
    int index = lv_obj_get_index(target);
    switch (value)
    {
    case LV_KEY_UP:
        if (index > (FileListPanelRowNumber - 1))
            lv_group_focus_obj(lv_obj_get_child(FileListPanel, index - FileListPanelRowNumber));
        break;
    case LV_KEY_DOWN:
        if ((uint32_t)(index + FileListPanelRowNumber) < lv_obj_get_child_cnt(FileListPanel))
            lv_group_focus_obj(lv_obj_get_child(FileListPanel, index + FileListPanelRowNumber));
        else if ( ((uint32_t)index/FileListPanelRowNumber)*FileListPanelRowNumber + FileListPanelRowNumber < lv_obj_get_child_cnt(FileListPanel)) {
            lv_group_focus_obj(lv_obj_get_child(FileListPanel, lv_obj_get_child_cnt(FileListPanel) - 1));
        }
        break;
    case LV_KEY_LEFT:
        if(index == 0) {
            lv_event_send(target, LV_EVENT_DEFOCUSED, nullptr);
            set_group_activity(CategoryGroup);
        }
        else
            lv_group_focus_prev(group);
        break;
    case LV_KEY_RIGHT:
        if (lv_obj_get_child(FileListPanel, lv_obj_get_child_cnt(FileListPanel) - 1) != target)
            lv_group_focus_next(group);
        else
            lv_group_focus_obj(lv_obj_get_child(FileListPanel, 0));
        break;
    case LV_KEY_ESC:
        if(!IsRootPath(current_path)) ReturnUpper();
        else                          ExitMedia(HomeScreen);
        break;

    default:
        base_event_handler(event);
        break;
    }
}

static void file_list_handler(lv_event_t* event)
{
    lv_obj_t* target = lv_event_get_current_target(event);
    uint32_t value = lv_indev_get_key(lv_indev_get_act());
    if (LV_KEY_ENTER == value) {
        if (target->user_data != nullptr) {
            switch (((FileStr *)(target->user_data))->type)
            {
            case FILE_DIR:
                FileListPanelStack.push(FileListPanel);
                CreateFilePanel(MediaSubScreen, ((FileStr *)(target->user_data))->name);
                lv_group_focus_obj(lv_obj_get_child(FileListPanel, 0));
                break;
            case FILE_VIDEO:
                creat_video_window(MediaSubScreen, ((FileStr *)(target->user_data))->name);
                break;
            case FILE_MUSIC:
                creat_music_window(MediaSubScreen, ((FileStr *)(target->user_data))->name);
                break;
            case FILE_PHOTO:
                creat_photo_window(MediaSubScreen, ((FileStr *)(target->user_data))->name);
                break;
            case FILE_TEXT:
                creat_text_window(MediaSubScreen, ((FileStr *)(target->user_data))->name);
                break;
            
            default:
                break;
            }
        }
    }
    else {
        key_base_event_handler(event);
    }
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
    if (lv_obj_get_child_cnt(FileListPanel) == 0) {
        lv_obj_t* ReturnButton = lv_btn_create(FileListPanel);
        ReturnButton->user_data = const_cast<char*>("media_file_p_return");
        DrawCell(ReturnButton, FileWidth, FileHeight, &ui_img_delivery_png, _("media_file_p_return"));
        lv_group_add_obj(FileListGroup, ReturnButton);
        lv_obj_add_event_cb(ReturnButton, [] (lv_event_t* event) {
            uint32_t value = lv_indev_get_key(lv_indev_get_act());
            switch (value)
            {
            case LV_KEY_ENTER:
                if ( !IsRootPath(current_path)) ReturnUpper();
                break;
            default:
                key_base_event_handler(event);
                break;
            }
        }, LV_EVENT_KEY, nullptr);
    }
    if (file_list == nullptr) return;
    int FileCnt = GetFileNumber(file_list);
    DestroyAllMediaList();
    GetNextFileFromFileList(nullptr);//清理前一次使用痕迹
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
        lv_obj_t* btn = lv_btn_create(FileListPanel);
        btn->user_data = file;
        DrawCell(btn, FileWidth, FileHeight, image, file->name);
        lv_group_add_obj(FileListGroup, btn);
        lv_obj_add_event_cb(btn, file_list_handler, LV_EVENT_KEY, nullptr);
        lv_obj_add_event_cb(btn, [] (lv_event_t* event) {
            lv_obj_t* target = lv_event_get_current_target(event);
            lv_label_set_long_mode(target->spec_attr->children[1], LV_LABEL_LONG_SCROLL_CIRCULAR);
        }, LV_EVENT_FOCUSED, nullptr);
        lv_obj_add_event_cb(btn, [] (lv_event_t* event) {
            lv_obj_t* target = lv_event_get_current_target(event);
            lv_label_set_long_mode(target->spec_attr->children[1], LV_LABEL_LONG_DOT);
        }, LV_EVENT_DEFOCUSED, nullptr);
    }
    lv_label_set_text(RealPath, current_path);
}

static void CreateFilePanel(lv_obj_t* parent, const char *path)
{
    FileListPanel = lv_obj_create(parent);
    lv_obj_set_size(FileListPanel, 1010, 600);
    lv_obj_set_pos(FileListPanel, 110, 25);
    lv_obj_set_align(FileListPanel, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(FileListPanel, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_bg_color(FileListPanel, lv_color_hex(0x0D6D96), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(FileListPanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(FileListPanel, lv_color_hex(0x009DFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(FileListPanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(FileListPanel, 6, LV_PART_MAIN | LV_STATE_DEFAULT);

    FileListGroup = create_new_group();
    set_group_activity(FileListGroup);
    strcat(current_path, "/");
    strcat(current_path, path);
    ShowFileList(GetFileList(current_path));
}

static void CreateCategoryPanel(lv_obj_t* parent)
{
    static const char* lab_text[] = {
        "media_category_p_all",
        "media_category_p_video",
        "media_category_p_music",
        "media_category_p_photo",
        "media_category_p_text" };
    CategoryPanel = lv_obj_create(parent);
    lv_obj_set_size(CategoryPanel, 252, 420);
    lv_obj_set_pos(CategoryPanel, -515, 0);
    lv_obj_set_align(CategoryPanel, LV_ALIGN_CENTER);
    lv_obj_clear_flag(CategoryPanel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(CategoryPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(CategoryPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    CategoryGroup = lv_group_create();//对于类别组做一般处理
    set_group_activity(CategoryGroup);
    for (int i = 0; i < MediaFile_CategoryNumber; i++) {
        lv_obj_t* btn = lv_btn_create(CategoryPanel);
        lv_obj_set_size(btn, 265, 50);
        lv_obj_set_pos(btn, -30, -160 + i * 80);
        lv_obj_set_align(btn, LV_ALIGN_CENTER);
        lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_radius(btn, 25, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_color(btn, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_opa(btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_radius(btn, 25, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_bg_color(btn, lv_color_hex(0xFF3700), LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_bg_opa(btn, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_shadow_color(btn, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_shadow_opa(btn, 0, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_group_add_obj(CategoryGroup, btn);
        lv_obj_add_event_cb(btn, [] (lv_event_t* event) {
            lv_obj_t* target = lv_event_get_current_target(event);
            lv_group_t* group = (lv_group_t*)lv_obj_get_group(target);
            uint32_t value = lv_indev_get_key(lv_indev_get_act());
            switch (value)
            {
            case LV_KEY_UP:
                lv_group_focus_prev(group);
                break;
            case LV_KEY_DOWN:
                lv_group_focus_next(group);
                break;
            case LV_KEY_RIGHT:
                set_group_activity(FileListGroup);
                lv_group_focus_obj(lv_obj_get_child(FileListPanel, 0));
                break;
            case LV_KEY_ESC:
                ExitMedia(HomeScreen);
                break;
            default:
                base_event_handler(event);
                break;
            }
        }, LV_EVENT_KEY, nullptr);
        lv_obj_add_event_cb(btn, [] (lv_event_t* event) {
            lv_obj_t* target = lv_event_get_current_target(event);
            FileFilter = static_cast<MediaFileCategoryList>(lv_obj_get_index(target));
            FilterFile(FileFilter);
        }, LV_EVENT_FOCUSED, nullptr);

        lv_obj_t* ui_LAB = lv_label_create(btn);
        lv_obj_set_size(ui_LAB, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_pos(ui_LAB, 10, 0);
        lv_obj_set_align(ui_LAB, LV_ALIGN_CENTER);
        ui_LAB->user_data = const_cast<char*>(lab_text[i]);
        lv_label_set_text(ui_LAB, _(lab_text[i]));
        lv_obj_set_style_text_color(ui_LAB, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(ui_LAB, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(ui_LAB, &ui_font_MyFont34, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_t* ui_IMG = lv_img_create(btn);
        lv_img_set_src(ui_IMG, &ui_img_arrow_right_png);
        lv_obj_set_size(ui_IMG, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_pos(ui_IMG, 104, 0);
        lv_obj_set_align(ui_IMG, LV_ALIGN_CENTER);
        lv_obj_set_style_img_recolor(ui_IMG, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_img_recolor_opa(ui_IMG, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    lv_group_focus_obj(lv_obj_get_child(CategoryPanel, FileFilter));
}

static void ReturnUpper(void)
{
    lv_obj_t* CurrentFileListPanel = FileListPanel;
    FileListPanel = FileListPanelStack.top();
    FileListPanelStack.pop();
    lv_obj_del_async(CurrentFileListPanel);
    FileListGroup = delete_group(FileListGroup);
    char *file_extension = strrchr(current_path, '/');
    while(*file_extension != 0) {
        *file_extension = 0;
        file_extension++;
    }
    lv_label_set_text(RealPath, current_path);
    FileList* file_list = GetPreviousFileList();
    int FileCnt = GetFileNumber(file_list);
    DestroyAllMediaList();
    GetNextFileFromFileList(nullptr);//清理前一次使用痕迹
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

static void FilterFile(MediaFileCategoryList filter_type)
{
    static const lv_img_dsc_t* image_src[] = {
        nullptr,
        &ui_img_movie_png,
        &ui_img_music_png,
        &ui_img_image_png,
        &ui_img_text_png,
        &ui_img_other_png };
    if (FileListPanel == nullptr) return;
    if (last_filter_type != filter_type) {
        FileStr* file = nullptr;
        lv_obj_t* child = nullptr;
        uint16_t dir_number = GetDirNumber(current_list);
        uint16_t non_dir_number = GetNonDirNumber(current_list);
        uint16_t media_number = GetMediaListSize(static_cast<MediaType>(filter_type));
        int first_file_index = dir_number + 1;//第一个非文件夹对象
        int end_index = first_file_index + ((filter_type == MediaFile_All) ? non_dir_number : media_number);
        GetNextFile(nullptr);
        for (int i = first_file_index; i < end_index; i++) {
            do {
                file = GetNextFile(current_list->NonDirList);
            } while ((file != nullptr) && (filter_type != MediaFile_All) && (file->type != (FileType)filter_type));
            if (file == nullptr) break;
            child = lv_obj_get_child(FileListPanel, i);
            if (child != nullptr) {
                child->user_data = file;
                lv_img_set_src(child->spec_attr->children[0], image_src[file->type]);
                lv_label_set_text(child->spec_attr->children[1], file->name);
            }
            else {
                child = lv_btn_create(FileListPanel);
                child->user_data = file;
                DrawCell(child, FileWidth, FileHeight, image_src[file->type], file->name);
                lv_group_add_obj(FileListGroup, child);
                lv_obj_add_event_cb(child, file_list_handler, LV_EVENT_KEY, nullptr);
                lv_obj_add_event_cb(child, [] (lv_event_t* event) {
                    lv_obj_t* target = lv_event_get_current_target(event);
                    lv_label_set_long_mode(target->spec_attr->children[1], LV_LABEL_LONG_SCROLL_CIRCULAR);
                }, LV_EVENT_FOCUSED, nullptr);
                lv_obj_add_event_cb(child, [] (lv_event_t* event) {
                    lv_obj_t* target = lv_event_get_current_target(event);
                    lv_label_set_long_mode(target->spec_attr->children[1], LV_LABEL_LONG_DOT);
                }, LV_EVENT_DEFOCUSED, nullptr);
            }
        }
        if (filter_type != MediaFile_All) {
            uint16_t obj_number = lv_obj_get_child_cnt(FileListPanel);
            for (int i = end_index; i < obj_number; i++) {
                lv_obj_del_async(lv_obj_get_child(FileListPanel, i));
            }
        }
        last_filter_type = filter_type;
    }
}

static void MediaInit(void)
{
    MediaRootScreen = lv_obj_create(nullptr);
    lv_obj_set_style_bg_opa(MediaRootScreen, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(MediaRootScreen, LV_OBJ_FLAG_SCROLLABLE);

    MediaSubScreen = lv_obj_create(MediaRootScreen);
    lv_obj_set_size(MediaSubScreen, 1280, 720);
    lv_obj_set_pos(MediaSubScreen, 0, 0);
    lv_obj_set_align(MediaSubScreen, LV_ALIGN_CENTER);
    lv_obj_clear_flag(MediaSubScreen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(MediaSubScreen, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(MediaSubScreen, lv_color_hex(0x0C9D89), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(MediaSubScreen, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(MediaSubScreen, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    #ifdef HCCHIP_GCC
    lv_msg_subsribe_obj(MSG_HOTPLUG, MediaSubScreen, NULL);
    #else
    lv_msg_subscribe_obj(MSG_HOTPLUG, MediaSubScreen, NULL);
    #endif
    lv_obj_add_event_cb(MediaSubScreen, [] (lv_event_t* event) {
        lv_msg_t* msg = lv_event_get_msg(event);
        const int* UdiskStatus = static_cast<const int*>(lv_msg_get_payload(msg));
        if (*UdiskStatus) {//拔出
            switch (GetPlayingMediaType())/***如果在播放，则先停止播放***/
            {
            case MEDIA_VIDEO:
                close_video_window();
                break;
            case MEDIA_MUSIC:
                close_music_window();
                break;
            case MEDIA_PHOTO:
                close_photo_window();
                break;
            
            default:
                break;
            }
            lv_obj_del_async(FileListPanel);
            while (!FileListPanelStack.empty()) {
                FileListPanel = FileListPanelStack.top();
                lv_obj_del_async(FileListPanel);
                FileListPanelStack.pop();
            }
            DestroyAllMediaList();
            MediaFileDeInit();
            memset(current_path, 0, current_path_size );
            delete_all_group();
            CreateFilePanel(MediaSubScreen, media_dir);
            CreateMsgBox(lv_scr_act(), _("media_udisk_removed"), 1, nullptr);
        }
        else {
            MediaFileInit();
            ShowFileList(GetFileList(current_path));
        }
    }, LV_EVENT_MSG_RECEIVED, NULL);

    lv_obj_t* ui_LAB_Path = lv_label_create(MediaSubScreen);
    lv_obj_set_size(ui_LAB_Path, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_pos(ui_LAB_Path, -336, -300);
    lv_obj_set_align(ui_LAB_Path, LV_ALIGN_CENTER);
    ui_LAB_Path->user_data = const_cast<char*>("media_file_p_path");
    lv_label_set_text(ui_LAB_Path, _("media_file_p_path"));
    lv_obj_set_style_text_color(ui_LAB_Path, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LAB_Path, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LAB_Path, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);

    RealPath = lv_label_create(MediaSubScreen);
    lv_obj_set_size(RealPath, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_pos(RealPath, 360, -300);
    lv_obj_set_align(RealPath, LV_ALIGN_LEFT_MID);
    lv_label_set_text(RealPath, "");
    lv_obj_set_style_text_color(RealPath, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(RealPath, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(RealPath, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(RealPath, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);

    MediaFileInit();
    CreateFilePanel(MediaSubScreen, media_dir);
    CreateCategoryPanel(MediaSubScreen);
    if (lv_obj_get_child_cnt(FileListPanel) > 0)
        lv_event_send(lv_obj_get_child(FileListPanel, 0), LV_EVENT_DEFOCUSED, nullptr);
    last_filter_type = FileFilter;
}

static void LoadMedia(void)
{
    lv_scr_load_anim(MediaRootScreen, LV_SCR_LOAD_ANIM_NONE, 0, 0, true);
    //lv_scr_load_anim(MediaRootScreen, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0, true);
}

static void ExitMedia(ActiveScreen screen)
{
    while (!FileListPanelStack.empty()) {
        FileListPanelStack.pop();
    }
    DestroyAllMediaList();
    MediaFileDeInit();
    memset(current_path, 0, current_path_size );
    delete_all_group();
    lv_group_del(CategoryGroup);//单独删除
    //lv_obj_del(MediaSubScreen);
    CurrentScreen = screen;
}

window MediaWindow = {
    .ScreenInit = MediaInit,
    .ScreenLoad = LoadMedia,
};
