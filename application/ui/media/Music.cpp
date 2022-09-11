/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-13 20:21:23
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-09-11 15:26:09
 * @FilePath: /gui/application/ui/media/Music.cpp
 * @Description: None
 * @other: None
 */
#include <string>
#include "Music.h"
#include "MediaCom.h"
#include "MediaFile.h"
#include "music_lyric.h"

static MediaHandle* MusicHandler;
static lv_obj_t* MusicWindow;
static lv_obj_t* MusicName;
static lv_obj_t* MusicCover;
static lv_obj_t* LyricPanel;
static lv_group_t* MainGroup;//聚焦歌词使用
static lv_obj_t* PlayBar;

music_lyric* lyric;
int lyric_index;

extern "C" {
static void CreateMusicWindow(lv_obj_t* parent);
static void SetStyleForPlayBar(lv_obj_t* bar);
static void CreateLyricPanel(lv_obj_t* parent);
}

void creat_music_window(lv_obj_t* parent, char* file_name)
{
    CreateMusicWindow(parent);
    #ifdef HOST_GCC
    lv_obj_t* Player = lv_ffmpeg_player_create(MusicWindow);
    lv_ffmpeg_player_set_auto_restart(Player, true);
    lv_obj_set_x(Player, -300);
    lv_obj_set_y(Player, -50);
    lv_obj_set_width(Player, 256);
    lv_obj_set_height(Player, 256);
    //lv_obj_center(Player);
    MusicHandler = Player;
    #elif defined(HCCHIP_GCC)
    MusicHandler = media_open(MEDIA_TYPE_MUSIC, (void*)MediaMsgProc);
    #endif

    MediaComInit(MEDIA_MUSIC, MusicHandler);
    CreateMediaArray();
    LocateMediaIndex(file_name);
    PlayMedia(file_name);
    SetCurrentMusicTitle(file_name);
    SetCurrentMusicCover(&ui_img_music_cover2_png);
    
    PlayBar = CreatePlayBar(lv_scr_act());
    SetStyleForPlayBar(PlayBar);
}

void close_music_window(void)
{
    //step1 停止播放
    #ifdef HOST_GCC
    //lv_ffmpeg_player_set_cmd(MusicHandler, LV_FFMPEG_PLAYER_CMD_STOP);
    #elif defined(HCCHIP_GCC)
    media_stop(MusicHandler);
    media_close(MusicHandler);
    MusicHandler = NULL;
    #endif

    lyric->clean();
    delete lyric;
    lyric = nullptr;

    //step2 清理播放列表
    DestroyMediaArray();

    MediaComDeinit();

    lv_group_del(MainGroup);
    //step4 关闭窗口
    lv_obj_del_async(PlayBar);
    lv_obj_del_async(MusicWindow);
}

void SetCurrentMusicTitle(char* file_name)
{
    //step1 设置文件名
    lv_label_set_text(MusicName, file_name);
}

void SetCurrentMusicCover(const lv_img_dsc_t* cover)
{
    //step2 设置封面
    lv_img_set_src(MusicCover, cover);
}

void LoadLyric(char* music_file_name)
{
    std::string music_name(music_file_name);
    std::string::size_type pos = music_name.rfind('.');
    std::string path(std::string(current_path) + '/' + music_name.substr(0,pos+1)+"lrc");
    lyric->clean();
    lyric->load(path.c_str());
    for (int i = 0; i < lyric->size(); i++) {
        lv_obj_t* lrc = lv_obj_get_child(LyricPanel, i);
        if (lrc == NULL) {
            lrc = lv_label_create(LyricPanel);
            lv_obj_set_width(lrc, 450);
            lv_obj_set_height(lrc, LV_SIZE_CONTENT);
            lv_obj_set_align(lrc, LV_ALIGN_CENTER);
            lv_obj_add_flag(lrc, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_set_style_text_color(lrc, lv_color_hex(0x89A29E), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(lrc, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(lrc, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(lrc, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(lrc, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_text_opa(lrc, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_group_add_obj(MainGroup, lrc);
        }
        lv_label_set_text(lrc, lyric->get_one_line(i).c_str());
    }
    uint16_t obj_number = lv_obj_get_child_cnt(LyricPanel);
    for (int i = lyric->size(); i < obj_number; i++) {
        lv_obj_del_async(lv_obj_get_child(LyricPanel, i));
    }
    lyric_index = 0;
    lv_group_focus_obj(lv_obj_get_child(LyricPanel, lyric_index));
}

void RefreshLyric(uint32_t played_time)
{
    static int last_index = 0;
    if (played_time >= lyric->get_start_time(lyric_index)) {
        for (; lyric_index < lyric->size()-1; ++lyric_index) {
            if ((lyric->get_start_time(lyric_index) <= played_time) && (played_time < lyric->get_start_time(lyric_index+1))) {
                break;
            }
        }
    }
    else {
        for (; lyric_index > 1; lyric_index--) {
            if ((lyric->get_start_time(lyric_index-1) < played_time) && (played_time <= lyric->get_start_time(lyric_index))) {
                break;
            }
        }
    }
    if (last_index != lyric_index) {
        last_index = lyric_index;
        lv_obj_t* child = lv_obj_get_child(LyricPanel, lyric_index);
        if (child != nullptr)
            lv_group_focus_obj(child);
    }
}

static void SetStyleForPlayBar(lv_obj_t* bar)
{
    lv_obj_set_style_bg_opa(bar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(bar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
}

static void CreateMusicWindow(lv_obj_t* parent)
{
    MusicWindow = lv_obj_create(parent);
    lv_obj_set_size(MusicWindow, 1280, 720);
    lv_obj_center(MusicWindow);
    lv_obj_set_style_radius(MusicWindow, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(MusicWindow, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(MusicWindow, lv_color_hex(0x2D729C), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(MusicWindow, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(MusicWindow, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    MusicName = lv_label_create(MusicWindow);
    lv_obj_set_width(MusicName, LV_SIZE_CONTENT);
    lv_obj_set_height(MusicName, LV_SIZE_CONTENT);
    lv_obj_set_x(MusicName, 0);
    lv_obj_set_y(MusicName, -300);
    lv_obj_set_align(MusicName, LV_ALIGN_CENTER);
    lv_label_set_text(MusicName, "");
    lv_obj_set_style_text_font(MusicName, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);

    MusicCover = lv_img_create(MusicWindow);
    lv_obj_set_width(MusicCover, LV_SIZE_CONTENT);
    lv_obj_set_height(MusicCover, LV_SIZE_CONTENT);
    lv_obj_set_x(MusicCover, -300);
    lv_obj_set_y(MusicCover, -50);
    lv_obj_set_align(MusicCover, LV_ALIGN_CENTER);
    lv_obj_add_flag(MusicCover, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(MusicCover, LV_OBJ_FLAG_SCROLLABLE);

    CreateLyricPanel(MusicWindow);
}

static void CreateLyricPanel(lv_obj_t* parent)
{
    lyric = new music_lyric;
    LyricPanel = lv_obj_create(parent);
    lv_obj_set_width(LyricPanel, 500);
    lv_obj_set_height(LyricPanel, 456);
    lv_obj_set_x(LyricPanel, 300);
    lv_obj_set_y(LyricPanel, -15);
    lv_obj_set_align(LyricPanel, LV_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(LyricPanel, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(LyricPanel, LV_DIR_VER);
    lv_obj_set_style_bg_opa(LyricPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(LyricPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_scroll_snap_y(LyricPanel, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_flex_flow(LyricPanel, LV_FLEX_FLOW_COLUMN);
    MainGroup = lv_group_create();
}

void MusicCoverSpinRun(void)
{
    SpinAnimation(MusicCover, LV_ANIM_PLAYTIME_INFINITE, 6000);
}

void MusicCoverSpinStop(void)
{
    lv_anim_del(MusicCover, nullptr);
}