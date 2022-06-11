/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-05-23 13:51:24
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-06-11 19:25:04
 * @FilePath: /gui/application/ui/MediaScreen.c
 * @Description: None
 * @other: None
 */
#include <unistd.h>
#include <string.h>
#include "MediaScreen.h"
#include "application/windows.h"
#include "MediaFile.h"
#ifdef HCCHIP_GCC
#include "hcapi/media_player.h"
#endif

static const lv_coord_t FileListPanelStartPos_x = -402;
static const lv_coord_t FileListPanelStartPos_y = -182;
static const uint8_t FileListPanelOffset_x = 160;
static const uint8_t FileListPanelOffset_y = 182;
static lv_indev_t* keypad_indev;
static char current_path[100];
int lsat_focused_item = 0;
#ifdef HOST_GCC
lv_obj_t * player_hld;
uint32_t played_time_host;
#elif defined(HCCHIP_GCC)
static media_handle_t *m_cur_media_hld = NULL;
#endif

typedef enum {
    All,
    Vedio = FILE_VIDEO,
    Music,
    Photo,
    Text,
    CategoryNumber
} CategoryList;

typedef enum {
    Progress,
    Previous,
    Play,
    Next,
    PlayedTime,
    TotalTime,
    PlayBarNumber
} PlayBarList;

#ifdef HOST_GCC
lv_ffmpeg_player_cmd_t play_state;
#endif

static CategoryList FileFilter;

lv_obj_t* ui_MediaScreen;
lv_obj_t* ui_Category_Panel;
lv_obj_t* ui_File_List_Panel;
lv_group_t* Category_Group;
static lv_group_t * default_group;
lv_group_t* File_List_Group;
lv_group_t* Player_Group;
lv_obj_t* ui_LAB_Real_Path;
lv_timer_t* PlayBar_Timer;
lv_timer_t* PlayState_Timer;
lv_obj_t* ui_Play_Bar_Panel;

static void ShowDisk(void);
static void ShowFileList(FileList *file_list);
static void DrawCell(lv_obj_t* ui_BTN, lv_coord_t w, lv_coord_t h, lv_coord_t x, lv_coord_t y, const void* pic, const char* str);
static void ReturnUpper(void);
static void RefreshFileWithFile(CategoryList filter);
static void return_handler(lv_event_t* event);
static void PlayBar_Timer_cb(lv_timer_t * timer);
static void play_bar_event_handler(lv_event_t* event);
static void ShowPlayedState(lv_timer_t * timer);

static lv_obj_t* creat_video_window(void)
{
    lv_obj_t* vedio_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_opa(vedio_screen, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(vedio_screen, LV_OBJ_FLAG_SCROLLABLE);

    Player_Group = lv_group_create();
    lv_group_set_default(Player_Group);
    lv_indev_set_group(keypad_indev, Player_Group);
    //lv_group_add_obj(Player_Group, vedio_screen);

    lv_disp_load_scr(vedio_screen);
    return vedio_screen;
}

static void close_video_window(lv_obj_t* video_window)
{
    lv_group_set_default(File_List_Group);
    lv_indev_set_group(keypad_indev, File_List_Group);
    lv_disp_load_scr(ui_MediaScreen);
    lv_group_del(Player_Group);
    lv_obj_del(video_window);
}

static void CreateMoveBarPanel(lv_obj_t* parent)
{
    static const lv_coord_t img_area[][2] = {//{ x, y}
        { -100, 20},
        {    0, 20},
        {  100, 20}
    };
    static const lv_img_dsc_t* image_src[] = {
        & ui_img_move_previous_png,
        & ui_img_move_pause_png,
        & ui_img_move_next_png
    };

    // ui_Play_Bar_Panel
    ui_Play_Bar_Panel = lv_obj_create(parent);
    lv_obj_set_width(ui_Play_Bar_Panel, 1260);
    lv_obj_set_height(ui_Play_Bar_Panel, 140);
    lv_obj_set_x(ui_Play_Bar_Panel, 0);
    lv_obj_set_y(ui_Play_Bar_Panel, 300);
    lv_obj_set_align(ui_Play_Bar_Panel, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_Play_Bar_Panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_Play_Bar_Panel, lv_color_hex(0x303030), LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_obj_set_style_bg_opa(ui_Play_Bar_Panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Play_Bar_Panel, LV_OPA_TRANSP, 50);
    lv_obj_set_style_border_color(ui_Play_Bar_Panel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_Play_Bar_Panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_group_add_obj(Player_Group, ui_Play_Bar_Panel);

    // ui_Progress_Bar
    lv_obj_t* ui_Progress_Bar = lv_slider_create(ui_Play_Bar_Panel);
    lv_slider_set_value(ui_Progress_Bar, 0, LV_ANIM_OFF);
    if(lv_slider_get_mode(ui_Progress_Bar) == LV_SLIDER_MODE_RANGE) lv_slider_set_left_value(ui_Progress_Bar, 0, LV_ANIM_OFF);
    lv_obj_set_width(ui_Progress_Bar, 900);
    lv_obj_set_height(ui_Progress_Bar, 28);
    lv_obj_set_x(ui_Progress_Bar, 0);
    lv_obj_set_y(ui_Progress_Bar, -35);
    lv_obj_set_align(ui_Progress_Bar, LV_ALIGN_CENTER);
    //lv_group_add_obj(Player_Group, ui_Progress_Bar);

    lv_group_remove_all_objs(Player_Group);
    for (int i = 0; i < Next; i++) {
        // ui_Move_Pause
        lv_obj_t* ui_Move_Pause = lv_img_create(ui_Play_Bar_Panel);
        lv_img_set_src(ui_Move_Pause, image_src[i]);
        lv_obj_set_width(ui_Move_Pause, LV_SIZE_CONTENT);
        lv_obj_set_height(ui_Move_Pause, LV_SIZE_CONTENT);
        lv_obj_set_x(ui_Move_Pause, img_area[i][0]);
        lv_obj_set_y(ui_Move_Pause, img_area[i][1]);
        lv_obj_set_align(ui_Move_Pause, LV_ALIGN_CENTER);
        lv_obj_add_flag(ui_Move_Pause, LV_OBJ_FLAG_ADV_HITTEST);
        lv_obj_clear_flag(ui_Move_Pause, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_bg_color(ui_Move_Pause, lv_color_hex(0x08AED2), LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_bg_opa(ui_Move_Pause, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_group_add_obj(Player_Group, ui_Move_Pause);
        lv_obj_add_event_cb(ui_Move_Pause, play_bar_event_handler, LV_EVENT_ALL, NULL);
    }

    // ui_Lab_Played_Time
    lv_obj_t* ui_Lab_Played_Time = lv_label_create(ui_Play_Bar_Panel);
    lv_obj_set_width(ui_Lab_Played_Time, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_Lab_Played_Time, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_Lab_Played_Time, -550);
    lv_obj_set_y(ui_Lab_Played_Time, -35);
    lv_obj_set_align(ui_Lab_Played_Time, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Lab_Played_Time, "00:00:00");
    lv_obj_set_style_text_color(ui_Lab_Played_Time, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_Lab_Played_Time, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_Lab_Total_Time
    lv_obj_t* ui_Lab_Total_Time = lv_label_create(ui_Play_Bar_Panel);
    lv_obj_set_width(ui_Lab_Total_Time, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_Lab_Total_Time, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_Lab_Total_Time, 540);
    lv_obj_set_y(ui_Lab_Total_Time, -35);
    lv_obj_set_align(ui_Lab_Total_Time, LV_ALIGN_CENTER);
    //uint32_t* total_time = (uint32_t *) malloc(sizeof(uint32_t));
    #if 1//def HOST_GCC
    uint32_t total_time = 0*3600 + 1*60 + 12;
    #elif defined(HCCHIP_GCC)
    uint32_t total_time = media_get_playtime(m_cur_media_hld);
    #endif
    //ui_Lab_Total_Time->user_data = total_time;
    lv_obj_set_style_text_color(ui_Lab_Total_Time, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text_fmt(ui_Lab_Total_Time, "%02"LV_PRIu32":%02"LV_PRIu32":%02"LV_PRIu32, (total_time) / 3600, ((total_time) % 3600) / 60, (total_time) % 60);
    lv_obj_set_style_text_font(ui_Lab_Total_Time, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_slider_set_range(ui_Progress_Bar, 0, total_time);

    lv_group_focus_obj(lv_obj_get_child(ui_Play_Bar_Panel, Play));
    PlayBar_Timer = lv_timer_create(PlayBar_Timer_cb, 5*1000, NULL);
}

static void PlayVideo(char * file_name)
{
    char file_path[100] = {0};
    strcat(file_path, current_path);
    strcat(file_path, "/");
    strcat(file_path, file_name);
    #ifdef HOST_GCC
    player_hld = lv_ffmpeg_player_create(lv_scr_act());
    lv_ffmpeg_player_set_src(player_hld, file_path);
    lv_ffmpeg_player_set_auto_restart(player_hld, true);
    lv_ffmpeg_player_set_cmd(player_hld, LV_FFMPEG_PLAYER_CMD_START);
    lv_obj_center(player_hld);
    play_state = LV_FFMPEG_PLAYER_CMD_START;

    Player_Group = lv_group_create();
    lv_group_set_default(Player_Group);
    lv_indev_set_group(keypad_indev, Player_Group);
    lv_obj_t * player = player_hld;
    played_time_host = 0;
    #elif defined(HCCHIP_GCC)
    lv_obj_t* player = creat_video_window();
    m_cur_media_hld = media_open(MEDIA_TYPE_VIDEO);
    media_play(m_cur_media_hld, file_path);
    //lv_obj_set_style_bg_opa(player, LV_OPA_TRANSP, 50);
    #endif
    //lv_group_add_obj(Player_Group, player);
    CreateMoveBarPanel(player);
    
    PlayState_Timer = lv_timer_create(ShowPlayedState, 1000, NULL);
    lv_timer_set_repeat_count(PlayState_Timer, -1);
}

static void PlayBar_Timer_cb(lv_timer_t * timer)
{
    lv_obj_add_flag(ui_Play_Bar_Panel, LV_OBJ_FLAG_HIDDEN);// | LV_OBJ_FLAG_ADV_HITTEST
    lv_timer_reset(timer);
    lv_timer_pause(timer);
}

static void ShowPlayedState(lv_timer_t * timer)
{
    #ifdef HOST_GCC
    if (play_state == LV_FFMPEG_PLAYER_CMD_START)
        played_time_host++;
    #endif
    if (!lv_obj_has_flag(ui_Play_Bar_Panel, LV_OBJ_FLAG_HIDDEN)) {
        #ifdef HOST_GCC
        uint32_t played_time = played_time_host;
        #elif defined(HCCHIP_GCC)
        uint32_t played_time = media_get_playtime(m_cur_media_hld);
        #endif
        lv_label_set_text_fmt(lv_obj_get_child(ui_Play_Bar_Panel, PlayedTime), "%02"LV_PRIu32":%02"LV_PRIu32":%02"LV_PRIu32, played_time / 3600, (played_time % 3600) / 60, played_time % 60);
        lv_slider_set_value(lv_obj_get_child(ui_Play_Bar_Panel, Progress), played_time, LV_ANIM_ON);
    }
}

static void play_bar_event_handler(lv_event_t* event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t* target = lv_event_get_target(event);
    //lv_obj_t* parents = lv_obj_get_parent(target);
    if (LV_EVENT_KEY == code) {
        uint32_t value = lv_indev_get_key(lv_indev_get_act());
        switch (value)
        {
            case LV_KEY_ENTER:
                switch (lv_obj_get_index(target))
                {
                    case Play:
                    {
                        #ifdef HOST_GCC
                        lv_ffmpeg_player_cmd_t state = play_state;
                        #define __MEDIA_PLAY    LV_FFMPEG_PLAYER_CMD_START
                        #define __MEDIA_PAUSE   LV_FFMPEG_PLAYER_CMD_PAUSE
                        #elif defined(HCCHIP_GCC)
                        media_state_t state = m_cur_media_hld->state;
                        #define __MEDIA_PLAY    MEDIA_PLAY
                        #define __MEDIA_PAUSE   MEDIA_PAUSE
                        #endif
                        if (state == __MEDIA_PLAY) {
                            lv_img_set_src(target, &ui_img_move_play_png);
                            #ifdef HOST_GCC
                            play_state = __MEDIA_PAUSE;
                            lv_ffmpeg_player_set_cmd(player_hld, __MEDIA_PAUSE);
                            #elif defined(HCCHIP_GCC)
                            m_cur_media_hld->state = __MEDIA_PAUSE;
                            media_pause(m_cur_media_hld);
                            #endif
                        }
                        else if (state == __MEDIA_PAUSE) {
                            lv_img_set_src(target, &ui_img_move_pause_png);
                            #ifdef HOST_GCC
                            play_state = __MEDIA_PLAY;
                            lv_ffmpeg_player_set_cmd(player_hld, LV_FFMPEG_PLAYER_CMD_RESUME);
                            #elif defined(HCCHIP_GCC)
                            m_cur_media_hld->state = __MEDIA_PLAY;
                            media_resume(m_cur_media_hld);
                            #endif
                        }
                    }
                    break;
                    case Previous:
                        
                        break;
                    case Next:
                        /* code */
                        break;
                    
                    default:
                        break;
                }
                break;
        
            case LV_KEY_LEFT:
                lv_group_focus_prev(Player_Group);
                break;
            case LV_KEY_RIGHT:
                lv_group_focus_next(Player_Group);
                break;
            
            default:
                break;
        }
        if (!lv_obj_has_flag(ui_Play_Bar_Panel, LV_OBJ_FLAG_HIDDEN)) {
            lv_timer_reset(PlayBar_Timer);
        }
        else {
            lv_obj_clear_flag(ui_Play_Bar_Panel, LV_OBJ_FLAG_HIDDEN);
            lv_timer_resume(PlayBar_Timer);
        }

    }
}

static void key_base_event_handler(lv_obj_t* target, lv_obj_t* parents)
{
    uint32_t value = lv_indev_get_key(lv_indev_get_act());
    lv_group_t* group = (lv_group_t*)lv_obj_get_group(target);
    if (LV_KEY_UP == value) {
        if (ui_Category_Panel == parents) {
            lv_group_focus_prev(group);
        }
        else if (ui_File_List_Panel == parents) {
            int index = lv_obj_get_index(target);
            if (index - 6 >= 0)
                lv_group_focus_obj(lv_obj_get_child(ui_File_List_Panel, index - 6));
        }
    }
    else if (LV_KEY_DOWN == value) {
        if (ui_Category_Panel == parents) {
            lv_group_focus_next(group);
        }
        else if (ui_File_List_Panel == parents) {
            int index = lv_obj_get_index(target);
            if ((uint32_t)(index + 6) < lv_obj_get_child_cnt(ui_File_List_Panel))
                lv_group_focus_obj(lv_obj_get_child(ui_File_List_Panel, index + 6));
            else if ( ((uint32_t)index/6)*6 + 6 < lv_obj_get_child_cnt(ui_File_List_Panel)) {
                lv_group_focus_obj(lv_obj_get_child(ui_File_List_Panel, lv_obj_get_child_cnt(ui_File_List_Panel) - 1));
            }
        }
    }
    else if (LV_KEY_LEFT == value) {
        if (ui_File_List_Panel == parents) {
            if (lv_obj_get_child(ui_File_List_Panel, 0) != target) {
                lv_group_focus_prev(group);
            }
            else {
                lv_group_focus_obj(ui_File_List_Panel);
                lv_group_set_default(Category_Group);
                lv_indev_set_group(keypad_indev, Category_Group);
            }
        }
    }
    else if (LV_KEY_RIGHT == value) {
        if (ui_Category_Panel == parents) {
            lv_group_set_default(File_List_Group);
            lv_indev_set_group(keypad_indev, File_List_Group);
            lv_group_focus_obj(lv_obj_get_child(ui_File_List_Panel, 0));
        }
        else if (ui_File_List_Panel == parents) {
            if (lv_obj_get_child(ui_File_List_Panel, lv_obj_get_child_cnt(ui_File_List_Panel) - 1) != target)
                lv_group_focus_next(group);
            else
                lv_group_focus_obj(lv_obj_get_child(ui_File_List_Panel, 0));
        }
    }
}

static void focused_base_event_handler(lv_obj_t* target, lv_obj_t* parents)
{
     if (parents == ui_File_List_Panel) {
        static lv_obj_t* last_target = NULL;
        if (lv_obj_is_valid(last_target))//设置上一个对象，为点模式
            lv_label_set_long_mode(last_target->spec_attr->children[1], LV_LABEL_LONG_DOT);//LV_LABEL_LONG_SCROLL_CIRCULAR

        if (target->spec_attr->children[1] != NULL) {
            lv_label_set_long_mode(target->spec_attr->children[1], LV_LABEL_LONG_SCROLL_CIRCULAR);//LV_LABEL_LONG_SCROLL_CIRCULAR
            last_target = target;
        }
    }

}

static void focused_handler(lv_event_t* event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t* target = lv_event_get_target(event);
    lv_obj_t* parents = lv_obj_get_parent(target);
    if (LV_EVENT_FOCUSED == code) {
        if (parents == ui_Category_Panel) {
            FileFilter = lv_obj_get_index(target);
            switch (FileFilter)
            {
            case All:
                
                break;

            case Vedio:
                {
                    FileStr* file = NULL;
                    lv_obj_t* child = NULL;
                    int first_file_index = GetDirNumber(current_list) + 1;//第一个对象为返回上一层
                    for (int i = first_file_index; i < lv_obj_get_child_cnt(ui_File_List_Panel); i++) {
                        do {
                            file = GetNextFile(current_list->OtherList);
                        } while(file != NULL && file->type != FILE_VIDEO);
                        child = lv_obj_get_child(ui_File_List_Panel, i);
                        if (file != NULL) {
                            child->user_data = file;
                            lv_img_set_src(child->spec_attr->children[0], &ui_img_filetype_mp4_png);
                            lv_label_set_text(child->spec_attr->children[1], file->name);
                        }
                        else {
                            lv_obj_add_flag(child, LV_OBJ_FLAG_HIDDEN);
                        }
                    }
                }
                break;
        
            case Music:
                
                break;

            case Photo:
                
                break;

            case Text:
                
                break;

            default:
                break;
            }
        }
    }
    else if (LV_EVENT_KEY == code) {
        key_base_event_handler(target, parents);
    }
}

static void file_list_handler(lv_event_t* event)
{
    //static lv_obj_t* last_target = NULL;
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t* target = lv_event_get_target(event);
    lv_obj_t* parents = lv_obj_get_parent(target);
    if (LV_EVENT_KEY == code) {
        uint32_t value = lv_indev_get_key(lv_indev_get_act());
        //lv_group_t* group = (lv_group_t*)lv_obj_get_group(target);
        if (LV_KEY_ENTER == value) {
            if (target->user_data != NULL) {
                switch (((FileStr *)(target->user_data))->type)
                {
                case FILE_DIR:
                    lsat_focused_item = lv_obj_get_index(target);
                    lv_obj_clean(ui_File_List_Panel);
                    strcat(current_path, "/");
                    strcat(current_path, ((FileStr *)(target->user_data))->name);
                    ShowFileList(GetFileList(current_path));
                    lv_group_focus_obj(lv_obj_get_child(ui_File_List_Panel, 0));
                    break;
                case FILE_VIDEO:
                    PlayVideo( ((FileStr *)(target->user_data))->name);
                    break;
                        
                case FILE_MUSIC:

                    break;
                case FILE_IMAGE:

                    break;
                case FILE_TEXT:

                    break;
                
                default:
                    break;
                }
            }
            //last_target = NULL;
        }
        else {
            key_base_event_handler(target, parents);
        }
    }
    else if (LV_EVENT_FOCUSED == code) {
        focused_base_event_handler(target,parents);
    }
}

static void return_handler(lv_event_t* event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t* target = lv_event_get_target(event);
    lv_obj_t* parents = lv_obj_get_parent(target);
    if (LV_EVENT_KEY == code) {
        uint32_t value = lv_indev_get_key(lv_indev_get_act());
        if (LV_KEY_ENTER == value) {
            if ( !IsRootPath(current_path)) {
                ReturnUpper();
                //lv_group_focus_obj(lv_obj_get_child(ui_File_List_Panel, lsat_focused_item));
                lv_group_focus_obj(lv_obj_get_child(ui_File_List_Panel, 0));
            }
        }
        else {
            key_base_event_handler(target, parents);
        }
    }
    else if (LV_EVENT_FOCUSED == code) {
        focused_base_event_handler(target,parents);
    }
}

static void GroupInit(void)
{
    Category_Group = lv_group_create();
    File_List_Group = lv_group_create();
    default_group = lv_group_get_default();
    lv_group_remove_all_objs(default_group);
}

static void CreateCategoryPanel(lv_obj_t* parent)
{
    static const char* lab_text[] = {
        "全部分类",
        "视频文件",
        "音乐文件",
        "图片文件",
        "文本文件" 
    };
    // ui_Category_Panel
    ui_Category_Panel = lv_obj_create(parent);
    lv_obj_set_size(ui_Category_Panel, 252, 420);
    lv_obj_set_pos(ui_Category_Panel, -515, 0);
    lv_obj_set_align(ui_Category_Panel, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_Category_Panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_Category_Panel, lv_color_hex(0x36589D), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Category_Panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_Category_Panel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_Category_Panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    for (int i = 0; i < CategoryNumber; i++) {
        lv_obj_t* ui_BTN = lv_btn_create(ui_Category_Panel);
        lv_obj_set_size(ui_BTN, 265, 50);
        lv_obj_set_pos(ui_BTN, -30, -160 + i * 80);
        lv_obj_set_align(ui_BTN, LV_ALIGN_CENTER);
        //lv_obj_add_flag(ui_BTN_All, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
        //lv_obj_clear_flag(ui_BTN_All, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_radius(ui_BTN, 25, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(ui_BTN, lv_color_hex(0xFF3700), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(ui_BTN, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_color(ui_BTN, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_opa(ui_BTN, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_radius(ui_BTN, 25, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_bg_color(ui_BTN, lv_color_hex(0xFF3700), LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_bg_opa(ui_BTN, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_shadow_color(ui_BTN, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_shadow_opa(ui_BTN, 0, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_group_add_obj(Category_Group, ui_BTN);
        lv_obj_add_event_cb(ui_BTN, focused_handler, LV_EVENT_ALL, NULL);

        // ui_LAB
        lv_obj_t* ui_LAB = lv_label_create(ui_BTN);
        lv_obj_set_size(ui_LAB, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_pos(ui_LAB, 25, 0);
        lv_obj_set_align(ui_LAB, LV_ALIGN_CENTER);
        lv_label_set_text(ui_LAB, lab_text[i]);
        lv_obj_set_style_text_color(ui_LAB, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(ui_LAB, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(ui_LAB, &ui_font_MyFont34, LV_PART_MAIN | LV_STATE_DEFAULT);

        // ui_IMG
        lv_obj_t* ui_IMG = lv_img_create(ui_BTN);
        lv_img_set_src(ui_IMG, &ui_img_arrow_right_png);
        lv_obj_set_size(ui_IMG, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_pos(ui_IMG, 104, 0);
        lv_obj_set_align(ui_IMG, LV_ALIGN_CENTER);
        //lv_obj_add_flag(ui_IMG_All, LV_OBJ_FLAG_HIDDEN);
        //lv_obj_add_flag(ui_IMG, LV_OBJ_FLAG_ADV_HITTEST);
        //lv_obj_clear_flag(ui_IMG, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_img_recolor(ui_IMG, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_img_recolor_opa(ui_IMG, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    FileFilter = All;
}

static void DrawCell(lv_obj_t* ui_BTN, lv_coord_t w, lv_coord_t h, lv_coord_t x, lv_coord_t y, const void* pic, const char* str)
{
    lv_obj_set_size(ui_BTN, w, h);
    lv_obj_set_pos(ui_BTN, x, y);
    lv_obj_set_align(ui_BTN, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_BTN, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    lv_obj_set_style_radius(ui_BTN, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_BTN, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_obj_set_style_shadow_color(ui_BTN, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(ui_BTN, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_obj_set_style_bg_color(ui_BTN, lv_color_hex(0xE23920), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_BTN, 20, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_bg_color(ui_BTN, lv_color_hex(0xE23920), LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_bg_opa(ui_BTN, 255, LV_PART_MAIN | LV_STATE_FOCUSED); 

    //lv_obj_set_style_shadow_color(ui_BTN, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_FOCUSED);
    //lv_obj_set_style_shadow_opa(ui_BTN, 0, LV_PART_MAIN | LV_STATE_FOCUSED);

    /*lv_obj_clear_flag(ui_BTN, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_BTN, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_BTN, lv_color_hex(0xE23920), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_BTN, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_BTN, 20, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_bg_color(ui_BTN, lv_color_hex(0xE23920), LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_bg_opa(ui_BTN, 255, LV_PART_MAIN | LV_STATE_FOCUSED);*/

    lv_obj_t* ui_IMG = lv_img_create(ui_BTN);
    lv_img_set_src(ui_IMG, pic);
    lv_obj_set_size(ui_IMG, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_pos(ui_IMG, 0, -25);
    lv_obj_set_align(ui_IMG, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_IMG, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_IMG, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* ui_LAB = lv_label_create(ui_BTN);
    lv_obj_set_size(ui_LAB, w, 40);
    lv_obj_set_pos(ui_LAB, 0, 62);
    lv_obj_set_align(ui_LAB, LV_ALIGN_CENTER);
    lv_label_set_long_mode(ui_LAB, LV_LABEL_LONG_DOT);//LV_LABEL_LONG_SCROLL_CIRCULAR
    lv_label_set_text(ui_LAB, str);
    lv_obj_set_style_text_align(ui_LAB, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LAB, &ui_font_MyFont30, LV_PART_MAIN | LV_STATE_DEFAULT);
}

static void ShowFileList(FileList *file_list)
{
    //lv_group_remove_obj(ui_File_List_Panel);
    lv_obj_t* ui_back = lv_btn_create(ui_File_List_Panel);
    ui_back->user_data = NULL;
    DrawCell(ui_back, 150, 180, FileListPanelStartPos_x, FileListPanelStartPos_y, &ui_img_delivery_png, "返回上一级");
    lv_group_add_obj(File_List_Group, ui_back);
    lv_obj_add_event_cb(ui_back, return_handler, LV_EVENT_ALL, NULL);
    //current_list = GetFileList(path);
    int FileCnt = GetFileNumber(file_list);;
    GetNextFileFromFileList(NULL);//清理前一次使用痕迹
    for (int i = 0; i < FileCnt; i++) {
        lv_img_dsc_t* image;
        lv_coord_t x = FileListPanelStartPos_x + ((i + 1) % 6) * FileListPanelOffset_x;
        lv_coord_t y = FileListPanelStartPos_y + ((i + 1) / 6) * FileListPanelOffset_y;
        FileStr* file = GetNextFileFromFileList(file_list);
        if (FileFilter != All) {
            if ((file->type != FILE_DIR) && ((uint8_t)file->type != (uint8_t)FileFilter)) {
                continue;
            }
        }
        switch (file->type)
        {
        case FILE_DIR:
            if (IsRootPath(current_path))
                image = (lv_img_dsc_t* )&ui_img_drive_png;
            else
                image = (lv_img_dsc_t* )&ui_img_folder2_png;
            break;
        case FILE_VIDEO:
            image = (lv_img_dsc_t* )&ui_img_filetype_mp4_png;
            break;
        case FILE_MUSIC:
            image = (lv_img_dsc_t* )&ui_img_filetype_mp3_png;
            break;
        case FILE_IMAGE:
            image = (lv_img_dsc_t* )&ui_img_filetype_jpg_png;
            break;
        case FILE_TEXT:
            image = (lv_img_dsc_t* )&ui_img_filetype_text_png;
            break;
        default:
            image = (lv_img_dsc_t* )&ui_img_filetype_other_png;
            break;
        }
        lv_obj_t* ui_BTN = lv_btn_create(ui_File_List_Panel);
        ui_BTN->user_data = file;
        DrawCell(ui_BTN, 150, 180, x, y, image, file->name);
        lv_group_add_obj(File_List_Group, ui_BTN);
        lv_obj_add_event_cb(ui_BTN, file_list_handler, LV_EVENT_ALL, NULL);
    }
    lv_label_set_text(ui_LAB_Real_Path, current_path);
}

static void RefreshFileWithFile(CategoryList filter)
{
}

static void ReturnUpper(void)
{
    lv_obj_clean(ui_File_List_Panel);
    char *file_extension = strrchr(current_path, '/');
    while(*file_extension != 0) {
        *file_extension = 0;
        file_extension++;
    }
    ShowFileList(GetPreviousFileList());
}

static void CreateFilePanel(lv_obj_t* parent)
{
    // ui_File_List_Panel
    ui_File_List_Panel = lv_obj_create(parent);
    lv_obj_set_size(ui_File_List_Panel, 1010, 615);
    lv_obj_set_pos(ui_File_List_Panel, 110, 40);
    lv_obj_set_align(ui_File_List_Panel, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_File_List_Panel, LV_FLEX_FLOW_ROW_WRAP);
    //lv_obj_clear_flag(ui_File_List_Panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_File_List_Panel, lv_color_hex(0x0D6D96), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_File_List_Panel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_File_List_Panel, lv_color_hex(0x009DFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_File_List_Panel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_File_List_Panel, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_group_add_obj(File_List_Group, ui_File_List_Panel);

    strcat(current_path, media_dir);
}

static void LoadMedia(void)
{
    lv_disp_load_scr(ui_MediaScreen);
}

static void MediaWait(void)
{
    do {
        usleep(5000);
    } while (CurrentScreen == MediaScreen);
}

static void MediaClose(void)
{
    lv_obj_del(ui_MediaScreen);
    
    lv_group_set_default(default_group);
    lv_indev_set_group(keypad_indev, default_group);
    lv_group_del(Category_Group);
    lv_group_del(File_List_Group);

}

static void MediaInit(lv_obj_t* parent, void *param)
{
    (void)param;

    ui_MediaScreen = lv_obj_create(parent);
    lv_obj_set_size(ui_MediaScreen, 1280, 720);
    //设置背景
    lv_obj_set_style_bg_color(ui_MediaScreen, lv_color_hex(0x0C9D89), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_MediaScreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_MediaScreen, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_MediaScreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_LAB_Path
    lv_obj_t* ui_LAB_Path = lv_label_create(ui_MediaScreen);
    lv_obj_set_size(ui_LAB_Path, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_pos(ui_LAB_Path, -336, -289);
    lv_obj_set_align(ui_LAB_Path, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LAB_Path, "路径：");
    lv_obj_set_style_text_color(ui_LAB_Path, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LAB_Path, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LAB_Path, &ui_font_MyFont34, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_LAB_Real_Path
    ui_LAB_Real_Path = lv_label_create(ui_MediaScreen);
    lv_obj_set_size(ui_LAB_Real_Path, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_pos(ui_LAB_Real_Path, 360, -289);
    lv_obj_set_align(ui_LAB_Real_Path, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_LAB_Real_Path, "");
    lv_obj_set_style_text_color(ui_LAB_Real_Path, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LAB_Real_Path, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_LAB_Real_Path, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LAB_Real_Path, &ui_font_MyFont34, LV_PART_MAIN | LV_STATE_DEFAULT);

    GroupInit();
    CreateCategoryPanel(ui_MediaScreen);
    CreateFilePanel(ui_MediaScreen);
    MediaFileInit();
    ShowFileList(GetFileList(media_dir));

    lv_group_set_default(Category_Group);
    
    keypad_indev = NULL;
    for (;;) {
        keypad_indev = lv_indev_get_next(keypad_indev);
        if (keypad_indev == NULL)
            break;
        if (keypad_indev->driver->type == LV_INDEV_TYPE_KEYPAD) {
            lv_indev_set_group(keypad_indev, Category_Group);
            break;
        }
    }
    lv_group_focus_obj(lv_obj_get_child(ui_Category_Panel, 0));
}

window MediaWindow = {
    .ScreenInit = MediaInit,
    .ScreenLoad = LoadMedia,
    .ScreenWait = MediaWait,
    .ScreenClose = MediaClose
};

