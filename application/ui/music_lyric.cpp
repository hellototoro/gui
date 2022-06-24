/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-19 19:35:54
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-06-24 15:07:24
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

bool music_lyric::load(const char* file_name)
{
    bool res = false;
    ifstream* lic_file = new ifstream(file_name);
    if (lic_file == nullptr) {
        cout << "file open failed" << endl;
        return res;
    }
    
    string line;
    while ( getline (*lic_file, line) ) {
        string::size_type pos = 0;
        if (line.at(pos) == '[') {
            if (!isdigit(line.at(pos + 1)))
                continue;
            int minute = stoi(line.substr(pos + 1), &pos);
            int second = minute * 60 + stoi(line.substr(pos + 2), &pos);
            start_time_list.push_back(second);
            string lic { line.substr(line.find(']', pos) + 1) };
            pos = lic.find('^');    //双语歌词
            if (pos != string::npos) {
                lic[pos] = '\n';
            }
            lic_list.push_back(lic);
        }
    }
    lic_file->close();
    delete lic_file;
    if(size() > 0)
        res = true;
    return res;
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
