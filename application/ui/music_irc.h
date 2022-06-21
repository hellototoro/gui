/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-19 19:35:47
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-06-21 12:24:58
 * @FilePath: /gui/application/ui/music_irc.h
 * @Description: None
 * @other: None
 */
#ifndef __MUSIC_IRC_H__
#define __MUSIC_IRC_H__

#include "lvgl/lvgl.h"
#ifdef HCCHIP_GCC
#include "hcapi/media_player.h"
#endif

#include <vector>
#include <string>

class music_irc
{
private:
    std::vector<std::string> irc_list;
    std::vector<int> start_time_list;
    
public:
    music_irc(/* args */);
    ~music_irc();

    void load_irc(char* file_name);
    std::string get_one_line_irc(int start_time);
};


#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
