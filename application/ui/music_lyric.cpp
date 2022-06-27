/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-19 19:35:54
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-06-27 14:44:55
 * @FilePath: /gui/application/ui/music_lyric.cpp
 * @Description: None
 * @other: None
 */
#include <fstream>
#include <iostream>
#include "music_lyric.h"

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
        lic_list.push_back("暂无歌词");
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
    return start_time_list[index];
}

string& music_lyric::get_one_line(int index)
{
    return lic_list[index];
}
