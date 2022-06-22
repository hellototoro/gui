/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-19 19:35:54
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-06-22 13:50:32
 * @FilePath: /gui/application/ui/music_lyric.cpp
 * @Description: None
 * @other: None
 */
#include <fstream>
#include <iostream>
#include "music_lyric.h"

using namespace std;

music_irc::music_irc(/* args */)
{
}

music_irc::~music_irc()
{
}

void music_irc::load_irc(char* file_name)
{
    ifstream* irc_file = new ifstream(file_name);
    if (irc_file == nullptr) {
        cout << "file open failed" << endl;
    }
    
    string line;
    while ( getline (*irc_file, line) ) {
        string::size_type pos = 0;
        if (line.at(pos) == '[') {
            if (!isdigit(line.at(pos + 1)))
                continue;
            int minute = stoi(line.substr(pos + 1), &pos);
            int second = minute * 60 + stoi(line.substr(pos + 2), &pos);
            start_time_list.push_back(second);
            irc_list.push_back(line.substr(line.find(']', pos) + 1));
        }
    }
    irc_file->close();
    delete irc_file;
}

string music_irc::get_one_line_irc(int start_time)
{
    return string("123");
}
