/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-07-21 20:34:15
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-09-19 00:11:42
 * @FilePath: /gui/application/ui/HdmiRx.c
 * @Description: None
 * @other: None
 */
#include "HdmiRx.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <malloc.h>
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/epoll.h>
#include <errno.h>
#include <sys/msg.h>
#include <termios.h>
#include <poll.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <unistd.h>

#define MSG_TASK_STACK_DEPTH (0x2000)
#define MX_EVNTS (10)
#define EPL_TOUT (1000)

#define HDMI_SWITCH_HDMI_STATUS_PLUGOUT		0
#define HDMI_SWITCH_HDMI_STATUS_PLUGIN		1
#define HDMI_SWITCH_BOOTLOGO_LAYER			DIS_PIC_LAYER_CURRENT
#define HDMI_SWITCH_HDMI_RX_LAYER           DIS_PIC_LAYER_MAIN

#define HDMI_SWITCH_DEFAULT_DISP_RESOLUTION TVSYS_1080P_60
#define HDMI_SWITCH_GET_EDID_TRY_TIME		500

lv_obj_t* HdmiRxScreen;
lv_group_t* HdmiRxGroup;

void AutoMoving_Animation(lv_obj_t * TargetObject, int delay)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_time(&a, 1000);
    lv_anim_set_var(&a, TargetObject);
    lv_anim_set_exec_cb(&a, anim_callback_set_x);
    lv_anim_set_values(&a, 0, 500);
    lv_anim_set_path_cb(&a, lv_anim_path_linear);
    lv_anim_set_delay(&a, delay + 0);
    lv_anim_set_early_apply(&a, false);
    lv_anim_set_get_value_cb(&a, &anim_callback_get_x);
    lv_anim_start(&a);

    lv_anim_init(&a);
    lv_anim_set_time(&a, 1000);
    lv_anim_set_var(&a, TargetObject);
    lv_anim_set_exec_cb(&a, anim_callback_set_y);
    lv_anim_set_values(&a, 0, 500);
    lv_anim_set_path_cb(&a, lv_anim_path_linear);
    lv_anim_set_delay(&a, delay + 0);
    lv_anim_set_early_apply(&a, false);
    lv_anim_set_get_value_cb(&a, &anim_callback_get_y);
    lv_anim_start(&a);
}

void CreateHdmiRxScreen(void)
{
    HdmiRxScreen = lv_obj_create(NULL);
    lv_obj_set_style_bg_opa(HdmiRxScreen, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(HdmiRxScreen, base_event_handler, LV_EVENT_KEY, NULL);

    lv_obj_clear_flag(HdmiRxScreen, LV_OBJ_FLAG_SCROLLABLE);
    //lv_obj_set_style_radius(HdmiRxScreen, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(HdmiRxScreen, lv_color_hex(0x3500FE), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(HdmiRxScreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(HdmiRxScreen, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* lab = lv_label_create(HdmiRxScreen);
    lv_obj_set_size(lab, 250, 60);
    lv_obj_set_pos(lab, 0, 0);
    lv_obj_set_align(lab, LV_ALIGN_CENTER);
    lv_label_set_text(lab, _("hdmi_no_single"));
    lv_obj_set_style_text_letter_space(lab, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(lab, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(lab, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lab, &ui_font_MyFont38, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(lab, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(lab, lv_color_hex(0x135BAB), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(lab, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_disp_load_scr(HdmiRxScreen);
}
