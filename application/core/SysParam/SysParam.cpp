/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-08-04 19:18:32
 * @FilePath: /lvgl_desktop/application/core/SysParam/SysParam.cpp
 * @Description: None
 * @other: None
 */
#include <sstream>
#include "SysParam.h"

SysParam::SysParam(/* args */) :
    file_path{"./SysParam.ini"},
    file{file_path},
    changed{false}
{
    file.read(ini);
}

SysParam::~SysParam()
{
    if (changed)
        file.write(ini);
}
