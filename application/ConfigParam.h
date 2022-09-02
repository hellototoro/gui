/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-08-04 19:18:37
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-09-02 01:38:22
 * @FilePath: /gui/application/ConfigParam.h
 * @Description: None
 * @other: None
 */
#ifndef __CONFIG_PARAM_H__
#define __CONFIG_PARAM_H__

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

extern const char ConfigFileName[];

extern void ReadConfigFile(boost::property_tree::ptree& config, std::string ConfigName);
#ifdef __cplusplus
extern "C" {
#endif
extern void WriteConfigFile_I(const char* ConfigName, int value);
extern void WriteConfigFile_S(const char* ConfigName, const char* value);
#ifdef __cplusplus
}
#endif

#endif
