/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-19 19:35:54
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-09-11 05:23:18
 * @FilePath: /gui/application/ui/media/music_lyric.cpp
 * @Description: None
 * @other: None
 */
#include <fstream>
#include <iostream>
#include "music_lyric.h"
#include "lv_i18n/src/lv_i18n.h"

using namespace std;

music_lyric::music_lyric(/* args */)
{
}

music_lyric::~music_lyric()
{
}

void music_lyric::load(const char* file_name)
{
    ifstream* lic_file = new ifstream(file_name);
    string line;
    while ( getline (*lic_file, line) ) {
        if (*line.begin() == '[') {
            string::size_type pos = 1;
            if (!isdigit(line.at(pos)))
                continue;
            line = line.substr(pos);
            int minute = stoi(line, &pos);
            line = line.substr( line.find( ':', pos) + 1);
            int second = minute * 60 + stoi( line, &pos);
            string lic { line.substr(line.find(']', pos) + 1) };
            if (lic.size() == 0) continue;
            if (lic[lic.size()-1] == '\r') lic.erase(lic.size()-1);
            if (lic.size() == 0) continue;
            pos = lic.find('^');    //双语歌词
            if (pos != string::npos) {
                lic[pos] = '\n';
            }
            start_time_list.push_back(second);
            lic_list.push_back(lic);
        }
    }
    if (size() == 0) {
        start_time_list.push_back(0);
        lic_list.push_back(_("media_no_lyric"));
    }
    if(lic_file->is_open())
        lic_file->close();
    delete lic_file;
}

int music_lyric::size(void)
{
    return start_time_list.size();
}

void music_lyric::clean(void)
{
    start_time_list.clear();
    lic_list.clear();
}

int music_lyric::get_start_time(int index)
{
    if (!start_time_list.empty())
        return start_time_list[index];
    else 
        return 0;
}

string& music_lyric::get_one_line(int index)
{
    return lic_list[index];
}
