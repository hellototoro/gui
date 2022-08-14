/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-21 12:32:11
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-08-15 00:44:56
 * @FilePath: /gui/application/ui/ui_com.cpp
 * @Description: None
 * @other: None
 */
#include <stack>
#include <list>
#include "ui_com.h"
#include "lv_i18n/src/lv_i18n.h"

/************全局变量*****************/
std::stack<lv_group_t*, std::list<lv_group_t*>> group_stack;
lv_indev_t* keypad_indev;

void group_init(void)
{
    lv_group_t * group = lv_group_get_default();
    if (!group) {
        lv_group_del(group);
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

lv_group_t* create_new_group(lv_group_t* old_group)
{
    lv_group_t* new_group;
    group_stack.push(old_group);
    new_group = lv_group_create();
    return new_group;
}

void set_group_activity(lv_group_t* group)
{
    lv_indev_set_group(keypad_indev, group);
}

bool group_stack_is_empty(void)
{
    return group_stack.empty();
}

lv_group_t* delete_group(lv_group_t* group)
{
    lv_group_t* last_group = group_stack.top();
    group_stack.pop();
    if (last_group != nullptr) {
        set_group_activity(last_group);
    }
    lv_group_del(group);
    return last_group;
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