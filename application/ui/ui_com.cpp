/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-21 12:32:11
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-08-22 19:48:41
 * @FilePath: /gui/application/ui/ui_com.cpp
 * @Description: None
 * @other: None
 */
#include <stack>
#include <list>
#include "ui_com.h"

/************全局变量*****************/
std::stack<lv_group_t*, std::list<lv_group_t*>> group_stack;
lv_indev_t* keypad_indev = nullptr;
lv_group_t* activity_group = nullptr;

void group_init(void)
{
    lv_group_t * group = lv_group_get_default();
    if (group) {
        //group = lv_group_create();
        lv_group_del(group);
        //lv_group_set_default(nullptr);
    }
}

lv_indev_t* get_keypad_indev(void)
{
    keypad_indev = NULL;
    for (;;) {
        keypad_indev = lv_indev_get_next(keypad_indev);
        if (keypad_indev == NULL)
            break;
        if (keypad_indev->driver->type == LV_INDEV_TYPE_KEYPAD)
            break;
    }
    return keypad_indev;
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
    lv_indev_set_group(keypad_indev, group);
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

lv_obj_t* CreateMsgBox(lv_obj_t* parent, const char* title, MsgBoxFunc_t func)
{
    auto event_cb = [] (lv_event_t* event) {
        lv_obj_t* target = lv_event_get_target(event);
        lv_obj_t* parent = lv_obj_get_parent(target);
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
                    MsgBoxFunc_t SelectedFunc = MsgBoxFunc_t(parent->user_data);
                    SelectedFunc();
                }
                delete_group(group);
                lv_obj_del_async(parent);
                //else if (index == 2) {//cancel
                //}
                break;
            case LV_KEY_ESC:
                delete_group(group);
                lv_obj_del_async(parent);
                break;
            default:
                break;
        }
    };

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

    lv_group_t* MainGroup = create_new_group();
    set_group_activity(MainGroup);
    lv_obj_t* btn_ok = lv_btn_create(MsgBoxPanel);
    lv_obj_set_size(btn_ok, 110, 60);
    lv_obj_set_pos(btn_ok, -100, 60);
    lv_obj_set_align(btn_ok, LV_ALIGN_CENTER);
    lv_obj_t * ok_lab = lv_label_create(btn_ok);
    lv_label_set_text(ok_lab, _("Ok"));
    lv_obj_set_style_text_font(ok_lab, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_center(ok_lab);
    lv_group_add_obj(MainGroup, btn_ok);
    lv_obj_add_event_cb(btn_ok, event_cb, LV_EVENT_KEY, nullptr);

    lv_obj_t* btn_cancel = lv_btn_create(MsgBoxPanel);
    lv_obj_set_size(btn_cancel, 110, 60);
    lv_obj_set_pos(btn_cancel, 100, 60);
    lv_obj_set_align(btn_cancel, LV_ALIGN_CENTER);
    lv_obj_t * cancel_lab = lv_label_create(btn_cancel);
    lv_label_set_text(cancel_lab, _("Cancel"));
    lv_obj_set_style_text_font(cancel_lab, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_center(cancel_lab);
    lv_group_add_obj(MainGroup, btn_cancel);
    lv_obj_add_event_cb(btn_cancel, event_cb, LV_EVENT_KEY, nullptr);
    lv_group_focus_obj(lv_obj_get_child(MsgBoxPanel, 2));

    return MsgBoxPanel;
}

void anim_callback_set_x(lv_anim_t * a, int32_t v)
{
    lv_obj_set_x((lv_obj_t *)a->user_data, v);
}

int32_t anim_callback_get_x(lv_anim_t * a)
{
    return lv_obj_get_x_aligned((lv_obj_t *)a->user_data);
}

void anim_callback_set_y(lv_anim_t * a, int32_t v)
{
    lv_obj_set_y((lv_obj_t *)a->user_data, v);
}

int32_t anim_callback_get_y(lv_anim_t * a)
{
    return lv_obj_get_y_aligned((lv_obj_t *)a->user_data);
}

int32_t anim_callback_get_opacity(lv_anim_t * a)
{
    return lv_obj_get_style_opa((lv_obj_t *)a->user_data, 0);
}

void anim_callback_set_opacity(lv_anim_t * a, int32_t v)
{
    lv_obj_set_style_opa((lv_obj_t *)a->user_data, v, 0);
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
