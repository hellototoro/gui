/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-13 20:21:23
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-06-22 14:03:48
 * @FilePath: /gui/application/ui/Music.c
 * @Description: None
 * @other: None
 */
#include "Music.h"
#include "MediaCom.h"
#include "MediaFile.h"
#include "ui_com.h"

#if 1

MediaHandle* MusicHandler;
lv_obj_t* MusicScreen;
lv_obj_t* MusicName;
lv_obj_t* MusicCover;
lv_obj_t* lyricPanel;
//lv_obj_t* FileListPanel;

lv_group_t* old_group;
lv_group_t* ListPanel_Group;

lv_img_dsc_t* CurrentMusicCover_IMG;
lv_img_dsc_t* CurrentPlayMode_IMG;
char* CurrentMusicName;
//bool ShowPlayList_Flag;

extern lv_indev_t* keypad_indev;
LV_FONT_DECLARE(ui_font_MyFont24);
LV_FONT_DECLARE(ui_font_MyFont30);
LV_FONT_DECLARE(ui_font_MyFont34);
LV_FONT_DECLARE(ui_font_MyFont38);
LV_IMG_DECLARE(ui_img_move_pause_png);    // assets\move_pause.png
LV_IMG_DECLARE(ui_img_move_next_png);    // assets\move_next.png
LV_IMG_DECLARE(ui_img_move_previous_png);    // assets\move_previous.png
LV_IMG_DECLARE(ui_img_move_play_png);    // assets\move_play.png
LV_IMG_DECLARE(ui_img_music_order_mode_png);    // assets\move_play.png
LV_IMG_DECLARE(ui_img_music_list_png);    // assets\move_play.png
LV_IMG_DECLARE(ui_img_music_cover_png);    // assets\move_play.png

static void key_event_handler(lv_event_t* event);
static lv_obj_t* CreateMusicScreen(lv_obj_t* parent);
static void SetStyleForPlayBar(lv_obj_t* bar);
static void CreatelyricPanel(lv_obj_t* parent, char** lrc_list, int number);

lv_obj_t* creat_music_window(lv_obj_t* foucsed_obj)
{
    CreateMusicScreen(lv_scr_act());
    #ifdef HOST_GCC
    lv_obj_t* Player = lv_ffmpeg_player_create(MusicScreen);
    lv_ffmpeg_player_set_auto_restart(Player, true);
    lv_obj_set_x(Player, 50);
    lv_obj_set_y(Player, 100);
    lv_obj_set_width(Player, LV_SIZE_CONTENT);
    lv_obj_set_height(Player, LV_SIZE_CONTENT);
    //lv_obj_center(Player);
    MusicHandler = Player;
    #elif defined(HCCHIP_GCC)
    MusicHandler = media_open(MEDIA_MUSIC);
    #endif

    MediaComInit(MEDIA_MUSIC, MusicHandler);
    CreateMediaArray(MEDIA_MUSIC);
    LocateMediaIndex(MEDIA_MUSIC, ((FileStr *)(foucsed_obj->user_data))->name);
    PlayMedia(MusicHandler, GetCurrentMediaName());
    SetCurrentMusicTitle(GetCurrentMediaName());
    SetCurrentMusicCover(&ui_img_music_cover_png);

    SetStyleForPlayBar(CreatePlayBar(MusicScreen));
    //CreatePlayListPanel(MusicScreen, GetMediaArray(), GetMediaArraySize(MEDIA_MUSIC));
    return MusicScreen;
}

void close_music_window(lv_obj_t* music_window)
{
    //step1 停止播放
    #ifdef HOST_GCC
    //lv_ffmpeg_player_set_cmd(MusicHandler, LV_FFMPEG_PLAYER_CMD_STOP);
    #elif defined(HCCHIP_GCC)
    media_stop(MusicHandler);
    MediaMonitorDeinit(MusicHandler);
    media_close(MusicHandler);
    MusicHandler = NULL;
    #endif

    //step2 清理播放列表
    DestroyMediaArray();

    //step3 恢复默认组
    MediaComDeinit();

    //step4 关闭窗口
    //lv_disp_load_scr(PreScreen);
    lv_obj_del_async(music_window);
}

void SetCurrentMusicTitle(char* file_name)
{
    //step1 设置文件名
    lv_label_set_text(MusicName, file_name);

    //step2 设置封面
    //lv_img_set_src(MusicCover, cover);

    //step3 设置歌词
}

void SetCurrentMusicCover(const lv_img_dsc_t* cover)
{
    //step1 设置文件名
    //lv_label_set_text(MusicName, file_name);

    //step2 设置封面
    lv_img_set_src(MusicCover, cover);

    //step3 设置歌词
}


/*void ShowPlayList(void)
{
    ShowPlayList_Flag = true;
    ShowUpAnimation(PlayListPanel, 300);
    set_group_activity(ListPanel_Group);
    lv_group_focus_obj(lv_obj_get_child(FileListPanel, 0));
}*/

static void key_event_handler(lv_event_t* event)
{
    //lv_obj_t* target = lv_event_get_target(event);
    //lv_obj_t* parents = lv_obj_get_parent(target);
    //lv_group_t* group = (lv_group_t*)lv_obj_get_group(target);
    (void)event;
    uint32_t value = lv_indev_get_key(lv_indev_get_act());
    switch (value)
    {
        /*case LV_KEY_UP:
            lv_group_focus_prev(ListPanel_Group);
            break;
        case LV_KEY_DOWN:
            lv_group_focus_next(ListPanel_Group);
            break;*/
        case LV_KEY_ESC:
            /*if (ShowPlayList_Flag) {
                //ShowDownAnimation(PlayListPanel, 300);
                //ShowPlayList_Flag = false;
                set_group_activity(old_group);
            }
            else */{
                close_music_window(MusicScreen);
            }
        break;

        default:
        break;
    }
}

static void SetStyleForPlayBar(lv_obj_t* bar)
{
    lv_obj_set_style_bg_color(bar, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(bar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(bar, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(bar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
}

static lv_obj_t* CreateMusicScreen(lv_obj_t* parent)
{
    // MusicScreen
    MusicScreen = lv_obj_create(parent);
    lv_obj_set_size(MusicScreen, 1280, 720);
    lv_obj_set_style_radius(MusicScreen, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(MusicScreen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(MusicScreen, lv_color_hex(0x2D729C), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(MusicScreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(MusicScreen, key_event_handler, LV_EVENT_KEY, NULL);

    // MusicName
    MusicName = lv_label_create(MusicScreen);
    lv_obj_set_width(MusicName, LV_SIZE_CONTENT);
    lv_obj_set_height(MusicName, LV_SIZE_CONTENT);
    lv_obj_set_x(MusicName, 0);
    lv_obj_set_y(MusicName, -300);
    lv_obj_set_align(MusicName, LV_ALIGN_CENTER);
    lv_label_set_text(MusicName, "");
    lv_obj_set_style_text_font(MusicName, &ui_font_MyFont38, LV_PART_MAIN | LV_STATE_DEFAULT);

    // MusicCover
    MusicCover = lv_img_create(MusicScreen);
    //lv_img_set_src(MusicCover, &ui_img_music_cover_png);
    lv_obj_set_width(MusicCover, LV_SIZE_CONTENT);
    lv_obj_set_height(MusicCover, LV_SIZE_CONTENT);
    lv_obj_set_x(MusicCover, -300);
    lv_obj_set_y(MusicCover, -50);
    lv_obj_set_align(MusicCover, LV_ALIGN_CENTER);
    lv_obj_add_flag(MusicCover, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(MusicCover, LV_OBJ_FLAG_SCROLLABLE);

    return MusicScreen;
}

static void CreatelyricPanel(lv_obj_t* parent, char** lrc_list, int number)
{
    // lyricPanel
    lyricPanel = lv_obj_create(parent);
    lv_obj_set_width(lyricPanel, 500);
    lv_obj_set_height(lyricPanel, 456);
    lv_obj_set_x(lyricPanel, 320);
    lv_obj_set_y(lyricPanel, -15);
    lv_obj_set_align(lyricPanel, LV_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(lyricPanel, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(lyricPanel, LV_DIR_VER);
    lv_obj_set_style_bg_color(lyricPanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(lyricPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(lyricPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(lyricPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    for (int i = 0; i < number; i++) {
        // lrc
        lv_obj_t*  lrc = lv_label_create(lyricPanel);
        lv_obj_set_width(lrc, 500);
        lv_obj_set_height(lrc, LV_SIZE_CONTENT);
        lv_obj_set_x(lrc, 0);
        lv_obj_set_y(lrc, 0);
        lv_obj_set_align(lrc, LV_ALIGN_CENTER);
        lv_label_set_text(lrc, lrc_list[i]);
        lv_obj_set_style_text_color(lrc, lv_color_hex(0x89A29E), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(lrc, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_align(lrc, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(lrc, &ui_font_MyFont24, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(lrc, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_text_opa(lrc, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
    }
}

#endif
