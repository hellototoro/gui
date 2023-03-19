/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-21 12:32:11
 * @FilePath: /gui/application/ui/ui_com.cpp
 * @Description: None
 * @other: None
 */
#include <stack>
#include <list>
#include <vector>
#include <stdio.h>
#include <string.h>
#include "ui_com.h"
#include "Volume.h"
#include "Source.h"
#include "SettingScreen.h"

/************全局变量*****************/
std::stack<lv_group_t*, std::list<lv_group_t*>> group_stack;
std::vector<lv_indev_t*> indev;
lv_group_t* activity_group = nullptr;

/*************************** 组 ****************************/
void group_init(void)
{
    lv_group_t * group = lv_group_get_default();
    if (group) {
        lv_group_del(group);
    }
    get_keypad_indev();
}

void get_keypad_indev(void)
{
    lv_indev_t* lv_indev = NULL;
    for (;;) {
        lv_indev = lv_indev_get_next(lv_indev);
        if (lv_indev == NULL)
            break;
        indev.push_back(lv_indev);
    }
}

lv_group_t* create_new_group(void)
{
    lv_group_t* new_group = lv_group_create();
    group_stack.push(new_group);
    return new_group;
}

void set_group_activity(lv_group_t* group)
{
    activity_group = group;
    for(auto indev_iter : indev)
        lv_indev_set_group(indev_iter, group);
}

lv_group_t* get_activity_group(void)
{
    return activity_group;
}

lv_group_t* delete_group(lv_group_t* group)
{
    lv_group_t* last_group = nullptr;
    if(group_stack.top() == group) {
        if(activity_group == group)
            activity_group = nullptr;
        lv_group_remove_all_objs(group);
        lv_group_del(group);
        group_stack.pop();
        if(!group_stack.empty()) {
            last_group = group_stack.top();
            set_group_activity(last_group);
        }
    }
    else {
        last_group = group_stack.top();
        set_group_activity(last_group);
    }
    return last_group;
}

void delete_all_group(void)
{
    while (!group_stack.empty()) {
        lv_group_t* group = group_stack.top();
        lv_group_remove_all_objs(group);
        lv_group_del(group);
        group_stack.pop();
    }
    activity_group = nullptr;
}

/*************************** 字符刷新 ****************************/
lv_obj_tree_walk_res_t obj_tree_walk_cb(lv_obj_t* obj, void * user_data)
{
    if (lv_obj_check_type(obj, &lv_label_class) && obj->user_data != nullptr) {
        lv_label_set_text(obj, _(static_cast<const char*>(obj->user_data)));
    }
    return LV_OBJ_TREE_WALK_NEXT;
}

void refresh_all_lable_text(lv_obj_t* parent)
{
    lv_obj_tree_walk(parent, obj_tree_walk_cb, nullptr);
}

/*************************** 对话框 ****************************/
lv_obj_t* CreateMsgBox(lv_obj_t* parent, const char* title, int btn_num, MsgBoxFunc_t func)
{
    auto event_cb = [] (lv_event_t* event) {
        lv_obj_t* target = lv_event_get_current_target(event);
        lv_obj_t* _parent = lv_obj_get_parent(target);
        uint32_t value = lv_indev_get_key(lv_indev_get_act());
        lv_group_t* group = get_activity_group();
        int index = lv_obj_get_index(target);
        switch (value)
        {
            case LV_KEY_LEFT:
                lv_group_focus_prev(group);
                break;
            case LV_KEY_RIGHT:
                lv_group_focus_next(group);
                break;
            case LV_KEY_ENTER:
                if (index == 1) { //ok
                    MsgBoxFunc_t SelectedFunc = MsgBoxFunc_t(_parent->user_data);
                    if (SelectedFunc) SelectedFunc();
                }
                delete_group(group);
                lv_obj_del_async(_parent);
                break;
            case LV_KEY_ESC:
                delete_group(group);
                lv_obj_del_async(_parent);
                break;
            default:
                break;
        }
    };

    if (btn_num > 2) return nullptr;
    lv_obj_t* MsgBoxPanel = lv_obj_create(parent);
    lv_obj_set_size(MsgBoxPanel, 500, 300);
    lv_obj_center(MsgBoxPanel);
    MsgBoxPanel->user_data = reinterpret_cast<void*>(func);

    lv_obj_t* text = lv_label_create(MsgBoxPanel);
    lv_obj_set_size(text,LV_SIZE_CONTENT,LV_SIZE_CONTENT);
    lv_obj_set_pos(text, 0, -60);
    lv_obj_set_align(text, LV_ALIGN_CENTER);
    lv_label_set_text(text, _(title));
    lv_obj_set_style_text_font(text, &ui_font_MyFont38, LV_PART_MAIN | LV_STATE_DEFAULT);

    if(btn_num > 0) {
        lv_group_t* MainGroup = create_new_group();
        set_group_activity(MainGroup);
        lv_obj_t* btn_ok = lv_btn_create(MsgBoxPanel);
        lv_obj_set_size(btn_ok, 110, 60);
        lv_obj_set_y(btn_ok, 60);
        if (btn_num == 2)
            lv_obj_set_x(btn_ok, -100);
        else
            lv_obj_set_x(btn_ok, 0);
        lv_obj_set_style_border_color(btn_ok, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_border_opa(btn_ok, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_border_width(btn_ok, 3, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_shadow_color(btn_ok, lv_color_hex(0x2196F3), LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_shadow_opa(btn_ok, 180, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_shadow_width(btn_ok, 2, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_shadow_spread(btn_ok, 2, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_shadow_ofs_x(btn_ok, 0, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_shadow_ofs_y(btn_ok, 0, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_align(btn_ok, LV_ALIGN_CENTER);
        lv_obj_t * ok_lab = lv_label_create(btn_ok);
        lv_label_set_text(ok_lab, _("setting_p_ok"));
        lv_obj_set_style_text_font(ok_lab, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_center(ok_lab);
        lv_group_add_obj(MainGroup, btn_ok);
        lv_obj_add_event_cb(btn_ok, event_cb, LV_EVENT_KEY, nullptr);

        if (btn_num == 2) {
            lv_obj_t* btn_cancel = lv_btn_create(MsgBoxPanel);
            lv_obj_set_size(btn_cancel, 110, 60);
            lv_obj_set_pos(btn_cancel, 100, 60);
            lv_obj_set_style_border_color(btn_cancel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_border_opa(btn_cancel, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_border_width(btn_cancel, 3, LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_shadow_color(btn_cancel, lv_color_hex(0x2196F3), LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_shadow_opa(btn_cancel, 180, LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_shadow_width(btn_cancel, 2, LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_shadow_spread(btn_cancel, 2, LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_shadow_ofs_x(btn_cancel, 0, LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_shadow_ofs_y(btn_cancel, 0, LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_align(btn_cancel, LV_ALIGN_CENTER);
            lv_obj_t * cancel_lab = lv_label_create(btn_cancel);
            lv_label_set_text(cancel_lab, _("setting_p_cancel"));
            lv_obj_set_style_text_font(cancel_lab, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_center(cancel_lab);
            lv_group_add_obj(MainGroup, btn_cancel);
            lv_obj_add_event_cb(btn_cancel, event_cb, LV_EVENT_KEY, nullptr);
            lv_group_focus_obj(lv_obj_get_child(MsgBoxPanel, 2));
        }
    }
    return MsgBoxPanel;
}

lv_obj_t* CreateSpinBox(lv_obj_t* parent, const char* title, int time_s, MsgBoxFunc_t func)
{
    lv_obj_t* SpinBox = lv_obj_create(parent);
    lv_obj_set_size(SpinBox, 500, 300);
    lv_obj_set_align(SpinBox, LV_ALIGN_CENTER);
    lv_obj_clear_flag(SpinBox, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    SpinBox->user_data = reinterpret_cast<void*>(func);

    lv_obj_t* title_lab = lv_label_create(SpinBox);
    lv_obj_set_size(title_lab, LV_SIZE_CONTENT, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_pos(title_lab, 0, 70);
    lv_obj_set_align(title_lab, LV_ALIGN_CENTER);
    lv_label_set_text(title_lab, title);
    lv_obj_set_style_text_font(title_lab, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* spinner = lv_spinner_create(SpinBox, 1000, 90);
    lv_obj_set_size(spinner, 60, 60);
    lv_obj_set_pos(spinner, 0, -40);
    lv_obj_set_align(spinner, LV_ALIGN_CENTER);
    lv_obj_clear_flag(spinner, LV_OBJ_FLAG_CLICKABLE);      /// Flags

    lv_timer_create([] (lv_timer_t * timer) {
        lv_obj_t* obj = static_cast<lv_obj_t*>(timer->user_data);
        MsgBoxFunc_t SpinnerFunc = MsgBoxFunc_t(obj->user_data);
        if (SpinnerFunc) SpinnerFunc();
        lv_obj_del(obj);
        lv_timer_del(timer);
    }, time_s*1000, SpinBox);

    return SpinBox;
}

/*************************** 动画 ****************************/
void anim_callback_set_x(void * var, int32_t v)
{
    lv_obj_set_x(static_cast<lv_obj_t *>(var), v);
}

int32_t anim_callback_get_x(lv_anim_t * a)
{
    return lv_obj_get_x_aligned((lv_obj_t *)a->var);
}

void anim_callback_set_y(void * var, int32_t v)
{
    lv_obj_set_y(static_cast<lv_obj_t *>(var), v);
}

int32_t anim_callback_get_y(lv_anim_t * a)
{
    return lv_obj_get_y_aligned((lv_obj_t *)a->var);
}

int32_t anim_callback_get_opacity(lv_anim_t * a)
{
    return lv_obj_get_style_opa((lv_obj_t *)a->var, 0);
}

void anim_callback_set_opacity(void * var, int32_t v)
{
    lv_obj_set_style_opa(static_cast<lv_obj_t *>(var), v, 0);
}

void anim_callback_set_image_angle(void * var, int32_t v)
{
    lv_img_set_angle(static_cast<lv_obj_t *>(var), v);
}

int32_t anim_callback_get_image_angle(lv_anim_t * a)
{
    return lv_img_get_angle((lv_obj_t *)a->var);
}

void SpinAnimation(lv_obj_t * TargetObject, int delay, int time)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, TargetObject);
    lv_anim_set_values(&a, 0, 3600);
    lv_anim_set_time(&a, time);
    lv_anim_set_exec_cb(&a, anim_callback_set_image_angle);
    lv_anim_set_path_cb(&a, lv_anim_path_linear);
    lv_anim_set_delay(&a, delay + 0);
    lv_anim_set_playback_time(&a, 0);
    lv_anim_set_playback_delay(&a, 0);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_repeat_delay(&a, 0);
    lv_anim_set_early_apply(&a, false);
    lv_anim_set_get_value_cb(&a, &anim_callback_get_image_angle);
    lv_anim_start(&a);
}

static lv_obj_t* LoadingPanel;
lv_obj_t* CreateLoadingScreen(lv_obj_t* parent)
{
    if (lv_obj_is_valid(LoadingPanel))
        return LoadingPanel;
    LoadingPanel = lv_obj_create(parent);
    lv_obj_set_size(LoadingPanel, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_pos(LoadingPanel, 0, 0);
    lv_obj_set_align(LoadingPanel, LV_ALIGN_CENTER);
    lv_obj_clear_flag(LoadingPanel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(LoadingPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(LoadingPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(LoadingPanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(LoadingPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(LoadingPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* Spinner = lv_spinner_create(LoadingPanel, 1000, 90);
    lv_obj_set_size(Spinner, 80, 80);
    lv_obj_set_pos(Spinner, 0, 0);
    lv_obj_set_align(Spinner, LV_ALIGN_CENTER);
    lv_obj_clear_flag(Spinner, LV_OBJ_FLAG_CLICKABLE);
    return LoadingPanel;
}

void CloseLoadingScreen(void)
{
    if (lv_obj_is_valid(LoadingPanel))
        lv_obj_del(LoadingPanel);
}

/*************************** 事件 ****************************/
void base_event_handler(lv_event_t* event)
{
    //lv_obj_t* target = lv_event_get_current_target(event);
    //lv_obj_t* parent = lv_obj_get_parent(target);
    uint32_t value = lv_indev_get_key(lv_indev_get_act());
    //lv_group_t* group = (lv_group_t*)lv_obj_get_group(lv_event_get_current_target(event));
    switch (value)
    {
    case LV_KEY_VOLUME_UP:
    case LV_KEY_VOLUME_DOWN:
        SetVolume(value);
        break;
    case LV_KEY_MENU:
        CreateSettingScreen(lv_scr_act());
        break;
    case LV_KEY_SOURCE:
        CreateSourcePanel(lv_scr_act());
        break;
    default:
        break;
    }
}

void other_event_handler(lv_event_t* event)
{
    
}

/*************************** 系统消息 ****************************/
void ProcessSysMsg(void)
{
}

/*************************** 其他 ****************************/
bool HasUsbDevice(void)
{
    FILE * fp;
    char buffer[20];
    fp = popen("ls ~/tmp/lvgl_test_dir","r");
    if (nullptr != fp) {
        while (fgets(buffer, sizeof(buffer)/sizeof(char), fp) != NULL) {
            if (strncmp("usb-storage", buffer, strlen("usb-storage")) == 0) {
                pclose(fp);
                return true;
            }
        }
    }
    pclose(fp);
    return false;
}
