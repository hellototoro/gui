/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-08-04 19:18:32
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-08-31 22:57:26
 * @FilePath: /gui/application/ConfigParam.cpp
 * @Description: None
 * @other: None
 */
#include "ConfigParam.h"

#ifdef HCCHIP_GCC
static constexpr char ConfigFileName[] = {"/mnt/data/gui_setting/gui_setting.ini"};
#else
static constexpr char ConfigFileName[] = {"/home/totoro/workspace/HiChip/hclinux/SOURCE/gui/application/setting/gui_setting.ini"};
#endif

void ReadConfigFile(boost::property_tree::ptree& config, std::string ConfigName)
{
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(ConfigFileName, pt);
    config = pt.get_child(ConfigName);
}

void WriteConfigFile_I(const char* ConfigName, int value)
{
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(ConfigFileName, pt);
    pt.put<int>(ConfigName, value);
    boost::property_tree::ini_parser::write_ini(ConfigFileName, pt);
}

void WriteConfigFile_S(const char* ConfigName, const char* value)
{
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(ConfigFileName, pt);
    pt.put<std::string>(ConfigName, value);
    boost::property_tree::ini_parser::write_ini(ConfigFileName, pt);
}
