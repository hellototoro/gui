/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-05-23 13:51:24
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-09-01 16:09:45
 * @FilePath: /gui/application/ui/HomeScreen.c
 * @Description: None
 * @other: None
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "HomeScreen.h"
#include "application/windows.h"
#include "Volume.h"
#include "application/key_map.h"
#include "SettingScreen.h"
#include "HdmiRx.h"
#include "SyncScreen.h"
#include "application/ui/media/MediaScreen.h"
#ifdef HCCHIP_GCC
#include "hcapi/hotplug_mgr.h"
#endif

static lv_obj_t* HomeRootScreen;
static lv_obj_t* MainPanel;
static lv_obj_t* UdiskDetectPanel;
static lv_group_t* MainGroup;
static int LastFocusedObjIndex;
static bool LastUdiskStatus_PlugOut;

extern pthread_mutex_t lvgl_task_mutex;

enum HomeCategoryList {
    HOME_MOVIE,
    HOME_MUSIC,
    HOME_PICTURE,
    HOME_MIRRORING,
    HOME_UDISK,
    HOME_HDMI,
    HOME_SETTING,
    HOME_CategoryNumber
};

#ifdef HOST_GCC
int hotplug_usb_plugout(void)
{
    return 0;
}
#endif

static void CreateMainPanel(lv_obj_t* parent);
static void ExitHome(ActiveScreen screen);
static void CreateDetectUdiskTask(void);

static void event_handler(lv_event_t* event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t* target = lv_event_get_target(event);
    lv_obj_t* parent = lv_obj_get_parent(target);
    if (LV_EVENT_KEY == code) {
        uint32_t value = lv_indev_get_key(lv_indev_get_act());
        lv_group_t* group = (lv_group_t*)lv_obj_get_group(target);
        switch (value)
        {
        case LV_KEY_LEFT:
            lv_group_focus_prev(group);
            break;
        case LV_KEY_RIGHT:
            lv_group_focus_next(group);
            break;
        case LV_KEY_UP:
            switch (lv_obj_get_index(target))
            {
            case HOME_UDISK:
            case HOME_HDMI:
                lv_group_focus_obj(lv_obj_get_child(parent, HOME_PICTURE));
                break;
            case HOME_PICTURE:
                lv_group_focus_obj(lv_obj_get_child(parent, HOME_MOVIE));
                break;
            case HOME_SETTING:
                lv_group_focus_obj(lv_obj_get_child(parent, HOME_MIRRORING));
                break;
            case HOME_MIRRORING:
                lv_group_focus_obj(lv_obj_get_child(parent, HOME_MUSIC));
                break;
            default:
                break;
            }
            break;
        case LV_KEY_DOWN:
            switch (lv_obj_get_index(target))
            {
            case HOME_MOVIE:
                lv_group_focus_obj(lv_obj_get_child(parent, HOME_PICTURE));
                break;
            case HOME_PICTURE:
                lv_group_focus_obj(lv_obj_get_child(parent, HOME_UDISK));
                break;
            case HOME_MUSIC:
                lv_group_focus_obj(lv_obj_get_child(parent, HOME_MIRRORING));
                break;
            case HOME_MIRRORING:
                lv_group_focus_obj(lv_obj_get_child(parent, HOME_SETTING));
                break;
            default:
                break;
            }
            break;
        case LV_KEY_ENTER:
            switch (lv_obj_get_index(target))
            {
            case HOME_MOVIE:
                FileFilter = MediaFile_Vedio;
                ExitHome(MediaScreen);
                break;
            case HOME_MUSIC:
                FileFilter = MediaFile_Music;
                ExitHome(MediaScreen);
                break;
            case HOME_PICTURE:
                FileFilter = MediaFile_Photo;
                ExitHome(MediaScreen);
                break;
            case HOME_UDISK:
                FileFilter = MediaFile_All;
                ExitHome(MediaScreen);
                break;
            case HOME_SETTING:
                CreateSettingScreen(HomeRootScreen);
                break;
            case HOME_MIRRORING:
                CreateSyncScreen(HomeRootScreen);
                break;
            default:
                break;
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
}

static void CreateMainPanel(lv_obj_t* parent)
{
    static const lv_coord_t img_area[HOME_CategoryNumber][4] = {//{ x, y, w, h}
        { -220, -180, 401, 167},
        {  220, -180, 401, 167},
        { -220,   20, 401, 167},
        {  220,   20, 401, 167},
        { -292,  220, 254, 164},
        {    0,  220, 254, 164},
        {  292,  220, 254, 164} };
    static const lv_coord_t lab_area[HOME_CategoryNumber][2] = {//{ x, y}
        { -90,  0},
        { -90,  0},
        { -90,  0},
        { -90,  0},
        {   0, 50},
        {   0, 50},
        {   0, 50} };
    static const char* str[HOME_CategoryNumber] = {
        "home_movie", 
        "home_music", 
        "home_picture", 
        "home_mirroring", 
        "home_udisk", 
        "home_hdmi", 
        "home_setting" };
    static const lv_img_dsc_t* image_src[] = {
        & ui_img_home_new_movie_png,
        & ui_img_home_new_music_png,
        & ui_img_home_new_photo_png,
        & ui_img_home_new_sync_screen_png,
        & ui_img_home_new_udisk_png,
        & ui_img_home_new_hdmi_in_png,
        & ui_img_home_new_setting_png };

    MainPanel = lv_obj_create(parent);
    lv_obj_set_size(MainPanel, 1280, 720);
    lv_obj_set_pos(MainPanel, 0, 0);
    lv_obj_set_align(MainPanel, LV_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(MainPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(MainPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    UdiskDetectPanel = lv_obj_create(parent);
    lv_obj_set_width(UdiskDetectPanel, 200);
    lv_obj_set_height(UdiskDetectPanel, 80);
    lv_obj_set_x(UdiskDetectPanel, 520);
    lv_obj_set_y(UdiskDetectPanel, -310);
    lv_obj_set_align(UdiskDetectPanel, LV_ALIGN_CENTER);
    lv_obj_clear_flag(UdiskDetectPanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(UdiskDetectPanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(UdiskDetectPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(UdiskDetectPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(UdiskDetectPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    LastUdiskStatus_PlugOut = hotplug_usb_plugout();

    lv_obj_t* img = lv_img_create(UdiskDetectPanel);
    lv_img_set_src(img, &ui_img_usb_png);
    lv_obj_set_width(img, LV_SIZE_CONTENT);   /// 50
    lv_obj_set_height(img, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_x(img, 0);
    lv_obj_set_y(img, 0);
    lv_obj_set_align(img, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(img, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(img, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_img_set_angle(img, 900);

    lv_obj_t* lab = lv_label_create(UdiskDetectPanel);
    lv_obj_set_width(lab, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(lab, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(lab, 0);
    lv_obj_set_y(lab, 0);
    lv_obj_set_align(lab, LV_ALIGN_LEFT_MID);
    lab->user_data = (void*)"no_device";
    lv_label_set_text(lab, _(lab->user_data));
    if (LastUdiskStatus_PlugOut) 
        lv_obj_add_flag(img, LV_OBJ_FLAG_HIDDEN);     /// Flags
    else
        lv_obj_add_flag(lab, LV_OBJ_FLAG_HIDDEN);     /// Flags
    lv_obj_set_style_text_color(lab, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(lab, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lab, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);

    MainGroup = create_new_group();
    set_group_activity(MainGroup);
    for (int i = 0; i < HOME_CategoryNumber; i++) {
        lv_obj_t* lv_obj = lv_obj_create(MainPanel);
        lv_obj_set_size(lv_obj, img_area[i][2], img_area[i][3]);
        lv_obj_set_pos(lv_obj, img_area[i][0], img_area[i][1]);
        lv_obj_set_align(lv_obj, LV_ALIGN_CENTER);
        lv_obj_clear_flag(lv_obj, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_radius(lv_obj, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(lv_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_opa(lv_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_opa(lv_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_img_src(lv_obj, image_src[i], LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(lv_obj, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_border_opa(lv_obj, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_border_width(lv_obj, 5, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_group_add_obj(MainGroup, lv_obj);
        lv_obj_add_event_cb(lv_obj, event_handler, LV_EVENT_KEY, NULL);

        lv_obj_t* lv_lab = lv_label_create(lv_obj);
        lv_obj_set_size(lv_lab, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_pos(lv_lab, lab_area[i][0], lab_area[i][1]);
        lv_obj_set_align(lv_lab, LV_ALIGN_CENTER);
        lv_obj_clear_flag(lv_lab, LV_OBJ_FLAG_SCROLLABLE);
        lv_lab->user_data = (void*)str[i];
        lv_label_set_text(lv_lab, _(str[i]));
        lv_obj_set_style_text_color(lv_lab, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(lv_lab, &ui_font_MyFont38, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    lv_group_focus_obj(lv_obj_get_child(MainPanel, LastFocusedObjIndex));
}

static void HomeInit(void)
{
    HomeRootScreen = lv_obj_create(NULL);
    lv_obj_set_size(HomeRootScreen, 1280, 720);
    lv_obj_set_style_bg_color(HomeRootScreen, lv_color_hex(0x3200FE), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(HomeRootScreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    CreateMainPanel(HomeRootScreen);
    CreateDetectUdiskTask();
}

static void* DetectHotPlugTask(void* arg)
{
    (void)arg;
    while (CurrentScreen == HomeScreen) {
        if (LastUdiskStatus_PlugOut != hotplug_usb_plugout()) {
            LastUdiskStatus_PlugOut = hotplug_usb_plugout();
            pthread_mutex_lock(&lvgl_task_mutex);
            lv_obj_t* img = lv_obj_get_child(UdiskDetectPanel, 0);
            lv_obj_t* lab = lv_obj_get_child(UdiskDetectPanel, 1);
            if (LastUdiskStatus_PlugOut) {
                lv_obj_add_flag(img, LV_OBJ_FLAG_HIDDEN);     /// Flags
                lv_obj_clear_flag(lab, LV_OBJ_FLAG_HIDDEN);     /// Flags
            }
            else {
                lv_obj_add_flag(lab, LV_OBJ_FLAG_HIDDEN);     /// Flags
                lv_obj_clear_flag(img, LV_OBJ_FLAG_HIDDEN);     /// Flags
            }
            pthread_mutex_unlock(&lvgl_task_mutex);
        }
        usleep(1000 * 500);
    }
    pthread_exit(NULL);
}

void CreateDetectUdiskTask(void)
{
    int res;
    pthread_t thread_id = 0;
    pthread_attr_t attr;
    res = pthread_attr_init(&attr);
    if (res != 0) {
        perror("Attribute creation failed");
        exit(EXIT_FAILURE);
    }
    res = pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
    if (res != 0) {
        perror("Setting detached attribute failed");
        exit(EXIT_FAILURE);
    }
    res = pthread_create(&thread_id, &attr, DetectHotPlugTask, NULL);
    if (res != 0) {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    }
}

static void LoadHome(void)
{
    lv_scr_load_anim(HomeRootScreen, LV_SCR_LOAD_ANIM_NONE, 0, 0, true);
    //lv_scr_load_anim(HomeRootScreen, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0, true);
}

static void ExitHome(ActiveScreen screen)
{
    CurrentScreen = screen;
    LastFocusedObjIndex = lv_obj_get_index(lv_group_get_focused(MainGroup));
    delete_all_group();
    //lv_obj_del_async(HomeRootScreen);
}

window HomeWindow = {
    .ScreenInit = HomeInit,
    .ScreenLoad = LoadHome,
};
