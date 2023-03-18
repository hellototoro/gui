/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-08-04 19:18:32
 * @FilePath: /gui/application/ConfigParam.cpp
 * @Description: None
 * @other: None
 */
#include "ConfigParam.h"

constexpr char ConfigFileName[] = {"/home/totoro/workspace/vasunrise/hclinux/SOURCE/projector_gui/application/setting/gui_setting.ini"};

void ReadConfigFile(boost::property_tree::ptree& config, std::string ConfigName)
{
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(ConfigFileName, pt);
    config = pt.get_child(ConfigName);
}

void WriteConfigFile_I(const char* ConfigName, int value)
{
    if (ConfigName == nullptr) return;
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(ConfigFileName, pt);
    pt.put<int>(ConfigName, value);
    boost::property_tree::ini_parser::write_ini(ConfigFileName, pt);
}

int ReadConfigFile_I(const char* ConfigName)
{
    if (ConfigName == nullptr) return 0;
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(ConfigFileName, pt);
    return pt.get<int>(ConfigName);
}

void WriteConfigFile_S(const char* ConfigName, const char* value)
{
    if (ConfigName == nullptr) return;
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(ConfigFileName, pt);
    pt.put<std::string>(ConfigName, value);
    boost::property_tree::ini_parser::write_ini(ConfigFileName, pt);
}
