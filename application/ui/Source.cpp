/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-09-10 13:41:27
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-09-10 22:38:41
 * @FilePath: /gui/application/ui/Source.cpp
 * @Description: None
 * @other: None
 */
#include "Source.h"
#include "application/windows.h"

static lv_obj_t* SourcePanel;
static lv_obj_t* SourceTypeImg;
static lv_obj_t* SourceTypeLab;
static lv_group_t* MainGroup;
SourceType_t CureentSource;

static const lv_img_dsc_t* source_img_src[] = {
    &ui_img_usb2_big_png,
    &ui_img_hdmi_big_png
};

static void source_event_handler(lv_event_t* event)
{
    lv_obj_t* target = lv_event_get_current_target(event);
    uint32_t value = lv_indev_get_key(lv_indev_get_act());
    lv_group_t* group = (lv_group_t*)lv_obj_get_group(target);
    switch (value)
    {
    case LV_KEY_ENTER:
        switch (lv_obj_get_index(target))
        {
        case SourceUDisk:
            if (CurrentScreen != MediaScreen) {
                CurrentScreen = MediaScreen;
                lv_msg_send(MSG_EXIT_SCREEN, nullptr);
            }
            break;
         case SourceHDMI:
            //CreateHdmiRxScreen();
            break;
       
        default:
            break;
        }
        return;
    case LV_KEY_LEFT:
        lv_group_focus_prev(group);
        break;
    case LV_KEY_RIGHT:
        lv_group_focus_next(group);
        break;
    case LV_KEY_ESC:
        MainGroup = delete_group(MainGroup);
        lv_obj_del_async(SourcePanel);
        return;
    default:
        break;
    }
    int index = lv_obj_get_index(lv_group_get_focused(group));
    lv_img_set_src(SourceTypeImg, source_img_src[index]);
    if(index == 0)
        SourceTypeLab->user_data = (void*)"home_udisk";
    else if(index == 1)
        SourceTypeLab->user_data = (void*)"home_hdmi";
    lv_label_set_text(SourceTypeLab, _(static_cast<const char*>(SourceTypeLab->user_data)));
}

void CreateSourcePanel(lv_obj_t* parent)
{
    SourcePanel = lv_obj_create(parent);
    lv_obj_set_size(SourcePanel, 1280, 720);
    lv_obj_clear_flag(SourcePanel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(SourcePanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(SourcePanel, lv_color_hex(0x3500FE), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(SourcePanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(SourcePanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    MainGroup = create_new_group();
    set_group_activity(MainGroup);

    lv_obj_t* btn = lv_btn_create(SourcePanel);
    lv_obj_set_size(btn, 113, 107);
    lv_obj_set_pos(btn, -87, 160);
    lv_obj_set_align(btn, LV_ALIGN_CENTER);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(btn, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(btn, &ui_img_usb2_n_png, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(btn, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_border_opa(btn, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(btn, 5, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_group_add_obj(MainGroup, btn);
    lv_obj_add_event_cb(btn, source_event_handler, LV_EVENT_KEY, NULL);

    btn = lv_btn_create(SourcePanel);
    lv_obj_set_size(btn, 113, 107);
    lv_obj_set_pos(btn, 87, 160);
    lv_obj_set_align(btn, LV_ALIGN_CENTER);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(btn, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(btn, &ui_img_hdmi_n_png, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(btn, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_border_opa(btn, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(btn, 5, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_group_add_obj(MainGroup, btn);
    lv_obj_add_event_cb(btn, source_event_handler, LV_EVENT_KEY, NULL);

    SourceTypeImg = lv_img_create(SourcePanel);
    lv_img_set_src(SourceTypeImg, &ui_img_usb2_big_png);
    lv_obj_set_size(SourceTypeImg, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_pos(SourceTypeImg, 0, -80);
    lv_obj_set_align(SourceTypeImg, LV_ALIGN_CENTER);
    lv_obj_add_flag(SourceTypeImg, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(SourceTypeImg, LV_OBJ_FLAG_SCROLLABLE);

    SourceTypeLab = lv_label_create(SourceTypeImg);
    lv_obj_set_size(SourceTypeLab, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_pos(SourceTypeLab, 0, 84);
    lv_obj_set_align(SourceTypeLab, LV_ALIGN_CENTER);
    SourceTypeLab->user_data = (void*)"home_udisk";
    lv_label_set_text(SourceTypeLab, _(static_cast<const char*>(SourceTypeLab->user_data)));
    lv_obj_set_style_text_color(SourceTypeLab, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(SourceTypeLab, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(SourceTypeLab, &ui_font_MyFont38, LV_PART_MAIN | LV_STATE_DEFAULT);
}
