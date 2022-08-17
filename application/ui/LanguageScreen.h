/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-05-23 13:51:29
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-08-17 21:27:19
 * @FilePath: /gui/application/ui/LanguageScreen.h
 * @Description: None
 * @other: None
 */
#ifndef __UI_LANGUAGE_SCREEN_H__
#define __UI_LANGUAGE_SCREEN_H__

#include "application/ui/ui_com.h"

#ifdef __cplusplus
extern "C" {
#endif

enum LanguageType {
    Espaol,     //西班牙语
    Deutsch,    //德语
    English,    //英语
    Italiano,   //意大利语
    France,     //法语
    Chinese,     //中文
    LanguageNumber
};

extern const char* Language[];
extern const char* DefaultLanguage;
extern int DefaultLanguageIndex;
extern void SaveCurrentLanguageType(int index);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
