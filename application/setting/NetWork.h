/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-08-15 13:36:22
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-08-18 14:23:59
 * @FilePath: /gui/application/setting/NetWork.h
 * @Description: None
 * @other: None
 */
#ifndef __NETWORK_SETTING_H__
#define __NETWORK_SETTING_H__

#include "application/ui/ui_com.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void CreateNetWorkPanel(lv_obj_t* parent);
void CreateWiFiPanel(lv_obj_t* parent);

#ifdef __cplusplus
}
#endif

#endif
