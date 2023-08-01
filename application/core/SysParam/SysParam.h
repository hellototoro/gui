/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-08-04 19:18:37
 * @FilePath: /lvgl_desktop/application/core/SysParam/SysParam.h
 * @Description: None
 * @other: None
 */
#ifndef __SYS_PARAM_H__
#define __SYS_PARAM_H__

#include <string>
#include "mini/ini.h"

class SysParam
{
private:
    /* data */
    std::string file_path;
    mINI::INIFile file;
    mINI::INIStructure ini;
    bool changed;

public:
    SysParam(/* args */);
    ~SysParam();

    template <typename T>
    void write(const std::string& parent, const std::string& child, const T& value)
    {
        std::ostringstream oss;
        oss << value;
        ini[parent][child] = oss.str();
        changed = true;
    }

    template <typename T>
    T read(const std::string& parent, const std::string& child)
    {
        std::istringstream iss(ini[parent][child]);
        T value;
        iss >> value;
        return value;
    }
};

#endif
