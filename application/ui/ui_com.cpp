/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-21 12:32:11
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-06-21 13:24:32
 * @FilePath: /gui/application/ui/ui_com.cpp
 * @Description: None
 * @other: None
 */
#include <stack>
#include <list>
#include "ui_com.h"

/************全局变量*****************/
std::stack<lv_group_t*, std::list<lv_group_t*>> group_stack;

lv_group_t* create_new_group(lv_group_t* old_group)
{
    lv_group_t* new_group;
    group_stack.push(old_group);
    new_group = lv_group_create();
    lv_group_remove_all_objs(new_group);
    return new_group;
}

void set_group_activity(lv_group_t* group, lv_indev_t* keypad_indev)
{
    lv_group_set_default(group);
    lv_indev_set_group(keypad_indev, group);
}

lv_group_t* delete_group(lv_group_t* group, lv_indev_t* keypad_indev)
{
    lv_group_t* last_group = group_stack.top();
    group_stack.pop();
    lv_group_set_default(last_group);
    lv_indev_set_group(keypad_indev, last_group);
    lv_group_remove_all_objs(group);
    lv_group_del(group);
    return last_group;
}
