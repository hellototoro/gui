/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-07-21 20:34:50
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-08-11 21:43:15
 * @FilePath: /gui/application/ui/HdmiRx.h
 * @Description: None
 * @other: None
 */
#ifndef __UI_HDMI_RX_H__
#define __UI_HDMI_RX_H__

#include "application/ui/resource/ui.h"

#ifdef __cplusplus
extern "C" {
#endif

int hdmi_init(void);
int hdmi_hotplug_enable(void);
int hdmi_rx_start(void);

extern void CreateHdmiRxScreen(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
