/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-19 19:35:47
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-08-29 22:30:57
 * @FilePath: /gui/application/ui/media/music_lyric.h
 * @Description: None
 * @other: None
 */
#ifndef __MUSIC_LYRIC_H__
#define __MUSIC_LYRIC_H__

#include "lvgl/lvgl.h"

#include <vector>
#include <string>

class music_lyric
{
private:
    std::vector<std::string> lic_list;
    std::vector<int> start_time_list;
    
public:
    music_lyric(/* args */);
    ~music_lyric();

    void load(const char* file_name);
    void clean(void);
    int size(void);
    int get_start_time(int index);
    std::string& get_one_line(int index);
};

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
