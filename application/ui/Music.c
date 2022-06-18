/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-13 20:21:23
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-06-18 18:11:44
 * @FilePath: /gui/application/ui/Music.c
 * @Description: None
 * @other: None
 */
#include "Music.h"
#include "MediaCom.h"
#include "MediaFile.h"

#if 1

MediaHandle* MusicHandler;
lv_obj_t* MusicScreen;
lv_obj_t* MusicName;
lv_obj_t* MusicCover;
lv_obj_t* PlayListPanel;
lv_obj_t* FileListPanel;

lv_img_dsc_t* CurrentMusicCover_IMG;
lv_img_dsc_t* CurrentPlayMode_IMG;
char* CurrentMusicName;

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

static void event_handler(lv_event_t* event);
static lv_obj_t* CreateMusicScreen(lv_obj_t* parent);
static void SetStyleForPlayBar(lv_obj_t* bar);
static void CreatePlayListPanel(lv_obj_t* parent, file_name* name_list, int file_number);

lv_obj_t* creat_music_window(lv_obj_t* foucsed_obj)
{
    CreateMusicScreen(lv_scr_act());
    #ifdef HOST_GCC
    lv_obj_t* Player = lv_ffmpeg_player_create(MusicScreen);
    lv_ffmpeg_player_set_auto_restart(Player, true);
    lv_obj_center(Player);
    MusicHandler = Player;
    #elif defined(HCCHIP_GCC)
    MusicHandler = media_open(MEDIA_MUSIC);
    #endif
    lv_obj_add_event_cb(MusicScreen, event_handler, LV_EVENT_KEY, NULL);
    lv_obj_clear_flag(MusicScreen, LV_OBJ_FLAG_SCROLLABLE);

    MediaComInit(MEDIA_MUSIC, MusicHandler);
    CreateMediaArray(MEDIA_MUSIC);
    LocateMediaIndex(MEDIA_MUSIC, ((FileStr *)(foucsed_obj->user_data))->name);
    PlayMedia(MusicHandler, GetCurrentMediaName());
    SetCurrentMusicInfo(GetCurrentMediaName(), &ui_img_music_cover_png);

    SetStyleForPlayBar(CreatePlayBar(MusicScreen));
    CreatePlayListPanel(MusicScreen, GetMediaArray(), GetMediaArraySize(MEDIA_MUSIC));
    return MusicScreen;
}

void close_music_window(lv_obj_t* music_window)
{
    //step1 停止播放
    #ifdef HOST_GCC
    lv_ffmpeg_player_set_cmd(MusicHandler, LV_FFMPEG_PLAYER_CMD_STOP);
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

static void event_handler(lv_event_t* event)
{
    (void)event;
    close_music_window(MusicScreen);
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
    lv_obj_clear_flag(MusicScreen, LV_OBJ_FLAG_SCROLLABLE);

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
    //lv_img_set_src(MusicCover, CurrentMusicCover_IMG);
    lv_obj_set_width(MusicCover, LV_SIZE_CONTENT);
    lv_obj_set_height(MusicCover, LV_SIZE_CONTENT);
    lv_obj_set_x(MusicCover, -300);
    lv_obj_set_y(MusicCover, -50);
    lv_obj_set_align(MusicCover, LV_ALIGN_CENTER);
    lv_obj_add_flag(MusicCover, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(MusicCover, LV_OBJ_FLAG_SCROLLABLE);

    // Musiclyric
    lv_obj_t * Musiclyric = lv_textarea_create(MusicScreen);
    lv_obj_set_width(Musiclyric, 453);
    lv_obj_set_height(Musiclyric, 387);
    lv_obj_set_x(Musiclyric, 359);
    lv_obj_set_y(Musiclyric, -50);
    lv_obj_set_align(Musiclyric, LV_ALIGN_CENTER);
    if("" == "") lv_textarea_set_accepted_chars(Musiclyric, NULL);
    else lv_textarea_set_accepted_chars(Musiclyric, "");
    lv_textarea_set_text(Musiclyric, "");
    lv_textarea_set_placeholder_text(Musiclyric, "Placeholder...");
    lv_obj_add_flag(Musiclyric, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_set_style_text_font(Musiclyric, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(Musiclyric, lv_color_hex(0xCB3F1E), LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(Musiclyric, 255, LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(Musiclyric, lv_color_hex(0x1D8C1B), LV_PART_CURSOR | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(Musiclyric, 255, LV_PART_CURSOR | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(Musiclyric, 0, LV_PART_CURSOR | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(Musiclyric, 0, LV_PART_CURSOR | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(Musiclyric, LV_TEXT_ALIGN_AUTO, LV_PART_CURSOR | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(Musiclyric, lv_color_hex(0xA92727), LV_PART_TEXTAREA_PLACEHOLDER | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(Musiclyric, 255, LV_PART_TEXTAREA_PLACEHOLDER | LV_STATE_DEFAULT);

    return MusicScreen;
}

static void CreatePlayListPanel(lv_obj_t* parent, file_name* name_list, int file_number)
{
    // PlayListPanel
    PlayListPanel = lv_obj_create(parent);
    lv_obj_set_width(PlayListPanel, 1280);
    lv_obj_set_height(PlayListPanel, 682);
    lv_obj_set_x(PlayListPanel, 0);
    lv_obj_set_y(PlayListPanel, 100);
    lv_obj_set_align(PlayListPanel, LV_ALIGN_CENTER);
    lv_obj_add_flag(PlayListPanel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(PlayListPanel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(PlayListPanel, 40, LV_PART_MAIN | LV_STATE_DEFAULT);


    // PlayListInfo
    lv_obj_t* PlayListInfo = lv_obj_create(PlayListPanel);
    lv_obj_set_width(PlayListInfo, 647);
    lv_obj_set_height(PlayListInfo, 64);
    lv_obj_set_x(PlayListInfo, -260);
    lv_obj_set_y(PlayListInfo, -305);
    lv_obj_set_align(PlayListInfo, LV_ALIGN_CENTER);
    lv_obj_clear_flag(PlayListInfo, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(PlayListInfo, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(PlayListInfo, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(PlayListInfo, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(PlayListInfo, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // PlayListMode_IMG

    lv_obj_t* PlayListMode_IMG = lv_img_create(PlayListInfo);
    lv_img_set_src(PlayListMode_IMG, CurrentPlayMode_IMG);
    lv_obj_set_width(PlayListMode_IMG, LV_SIZE_CONTENT);
    lv_obj_set_height(PlayListMode_IMG, LV_SIZE_CONTENT);
    lv_obj_set_x(PlayListMode_IMG, 0);
    lv_obj_set_y(PlayListMode_IMG, 0);
    lv_obj_set_align(PlayListMode_IMG, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(PlayListMode_IMG, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(PlayListMode_IMG, LV_OBJ_FLAG_SCROLLABLE);

    // PlayListMode_LAB
    lv_obj_t* PlayListMode_LAB = lv_label_create(PlayListInfo);
    lv_obj_set_width(PlayListMode_LAB, LV_SIZE_CONTENT);
    lv_obj_set_height(PlayListMode_LAB, LV_SIZE_CONTENT);
    lv_obj_set_x(PlayListMode_LAB, 79);
    lv_obj_set_y(PlayListMode_LAB, 0);
    lv_obj_set_align(PlayListMode_LAB, LV_ALIGN_LEFT_MID);
    lv_label_set_text(PlayListMode_LAB, "顺序播放（276首）");
    lv_obj_set_style_text_font(PlayListMode_LAB, &ui_font_MyFont34, LV_PART_MAIN | LV_STATE_DEFAULT);

    // FileListPanel
    FileListPanel = lv_obj_create(PlayListPanel);
    lv_obj_set_width(FileListPanel, 1280);
    lv_obj_set_height(FileListPanel, 615);
    lv_obj_set_x(FileListPanel, 0);
    lv_obj_set_y(FileListPanel, 40);
    lv_obj_set_align(FileListPanel, LV_ALIGN_CENTER);
    lv_obj_set_style_bg_color(FileListPanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(FileListPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(FileListPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(FileListPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    for(int i = 0; i < file_number; i++) {
        // file_panel
        lv_obj_t* file_panel = lv_obj_create(FileListPanel);
        lv_obj_set_width(file_panel, 1140);
        lv_obj_set_height(file_panel, 75);
        lv_obj_set_x(file_panel, 0);
        lv_obj_set_y(file_panel, -275 + i * 90);
        lv_obj_set_align(file_panel, LV_ALIGN_CENTER);
        lv_obj_clear_flag(file_panel, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_radius(file_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(file_panel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_opa(file_panel, 90, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_side(file_panel, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);

        // file_name
        lv_obj_t* file_name = lv_label_create(file_panel);
        lv_obj_set_width(file_name, LV_SIZE_CONTENT);
        lv_obj_set_height(file_name, LV_SIZE_CONTENT);
        lv_obj_set_x(file_name, -12);
        lv_obj_set_y(file_name, -20);
        lv_label_set_text(file_name, name_list[i]);
        lv_obj_set_style_text_font(file_name, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);

        // file_info
        lv_obj_t* file_info = lv_label_create(file_panel);
        lv_obj_set_width(file_info, LV_SIZE_CONTENT);
        lv_obj_set_height(file_info, LV_SIZE_CONTENT);
        lv_obj_set_x(file_info, -10);
        lv_obj_set_y(file_info, 20);
        lv_obj_set_align(file_info, LV_ALIGN_BOTTOM_LEFT);
        lv_label_set_text(file_info, "");
        lv_obj_set_style_text_color(file_info, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(file_info, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(file_info, &ui_font_MyFont24, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
}

void SetCurrentMusicInfo(char* file_name, const lv_img_dsc_t* cover)
{
    //step1 设置文件名
    lv_label_set_text(MusicName, file_name);

    //step2 设置封面
    lv_img_set_src(MusicCover, cover);

    //step3 设置歌词
}
#endif
