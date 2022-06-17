/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-13 13:31:24
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-06-17 14:02:43
 * @FilePath: /gui/application/ui/MediaCom.c
 * @Description: None
 * @other: None
 */
#include <stdlib.h>
#include <string.h>
#include "MediaCom.h"
#ifdef HCCHIP_GCC
#include <sys/msg.h>
#include <ffplayer.h>
#include <hcuapi/codec_id.h>
#include "hcapi/com_api.h"
#include "hcapi/os_api.h"
#endif

char current_path[100];
lv_obj_t* total_time_obj;
lv_obj_t* progress_obj;
MediaList* media_list[MEDIA_MAX];
#ifdef HCCHIP_GCC
media_handle_t* current_media_hdl;
#endif
typedef char* file_name;
file_name* media_file_name_array;
int current_playing_index;

#ifdef HCCHIP_GCC
static void MediaMsgProc(media_handle_t *media_hld, HCPlayerMsg *msg);
#endif

MediaList* CreatMediaList(MediaType media_type)
{
    media_list[media_type] = (MediaList*) malloc(sizeof(MediaList));
    InitDList(media_list[media_type]);
    return media_list[media_type];
}

bool MediaListIsEmpty(MediaType media_type)
{
    return (media_list[media_type]->len ==0 ) ? true : false;
}

void AddToMediaList(MediaType media_type, char * media_name)
{
    if (media_list[media_type] == NULL)
        CreatMediaList(media_type);
    DListAppend(media_list[media_type], media_name);
}

MediaList* GetMediaList(MediaType media_type)
{
    return media_list[media_type];
}

uint16_t GetMediaListSize(MediaType media_type)
{
    if (media_list[media_type] != NULL)
        return media_list[media_type]->len;
    else
        return 0;
}

DLNode* GetNextMediaNode(MediaList* media_list, PlayListMode mode)
{
    static DLNode *next = NULL;
    static DLNode *head = NULL;
    static MediaList* last_list = NULL;

    if (media_list == NULL) {//清除获取记录
        last_list = NULL;
        next = NULL;
        head = NULL;
        return NULL;
    }

    if ((next == head) && (head != NULL)) { //一个循环结束
        if (mode == CyclePlay)
            next = head->next;
        else
            return NULL;
    }

    if (last_list != media_list) {
        last_list = media_list;
        head = media_list->head;
        next = head->next;
    }
    else {
        next = next->next;
    }
    return next;
}

DLNode * GetPreMediaNode(MediaList* media_list, PlayListMode mode)
{
    static DLNode *pre = NULL;
    static DLNode *head = NULL;
    static MediaList* last_list = NULL;

    if (media_list == NULL) {//清除获取记录
        last_list = NULL;
        pre = NULL;
        head = NULL;
        return NULL;
    }

    if ((pre == head) && (head != NULL)) { //一个循环结束
        if (mode == CyclePlay)
            pre = head->pre;
        else
            return NULL;
    }

    if (last_list != media_list) {
        last_list = media_list;
        head = media_list->head;
        pre = head->pre;
    }
    else {
        pre = pre->pre;
    }
    return pre;
}

void DestroyMediaList(MediaType media_type)
{
    if (media_list[media_type] != NULL) {
        DestroyDList(media_list[media_type]);
        media_list[media_type] = NULL;
    }
}

void DestroyAllMediaList(void)
{
    for (int i = 0; i < MEDIA_MAX; i++) {
        DestroyMediaList(i);
    }
}

void CreatMediaArray(MediaType media_type)
{
    int n = GetMediaListSize(media_type);
    if (n == 0) return;
    DestroyMediaArray();
    current_playing_index = 0;
    media_file_name_array = (file_name*) malloc(sizeof(file_name) * n);
    GetNextMediaNode(NULL, OrderPlay);
    for (int i = 0; i < n; i++) {
        media_file_name_array[i] = (char*) (GetNextMediaNode(GetMediaList(media_type), OrderPlay))->data;
    }
}

uint16_t GetMediaArraySize(MediaType media_type)
{
    return GetMediaListSize(media_type);
}

uint16_t LocateMediaIndex(MediaType media_type, char * file_name)
{
    int i;
    uint16_t n = GetMediaArraySize(media_type);
    for (i = 0; i < n; i++) {
        if (strcmp(media_file_name_array[i], file_name) == 0) {
            current_playing_index = i;
            break;
        }
    }
    return current_playing_index;
}

char* GetCurrentMediaName(void)
{
    return media_file_name_array[current_playing_index];
}

char* GetNextMediaName(MediaType media_type, PlayListMode mode)
{
    ++current_playing_index;
    switch (mode)
    {
    case CyclePlay:
        if (current_playing_index >= GetMediaArraySize(media_type))
            current_playing_index = 0;
        break;
    case OrderPlay:
        if (current_playing_index >= GetMediaArraySize(media_type))
            return NULL;
        break;
    case OnlyOnePlay:
        /* code */
        break;
    case RandPlay:
        /* code */
        break;
    
    default:
        break;
    }
    return media_file_name_array[current_playing_index];
}

char* GetPreMediaName(MediaType media_type, PlayListMode mode)
{
    --current_playing_index;
    switch (mode)
    {
    case CyclePlay:
        if (current_playing_index < 0)
            current_playing_index = GetMediaArraySize(media_type) - 1;
        break;
    case OrderPlay:
        if (current_playing_index < 0)
            return NULL;
        break;
    case OnlyOnePlay:
        /* code */
        break;
    case RandPlay:
        /* code */
        break;
    
    default:
        break;
    }
    return media_file_name_array[current_playing_index];
}

void DestroyMediaArray(void)
{
    if (media_file_name_array != NULL) {
        free(media_file_name_array);
        media_file_name_array = NULL;
    }
}

//ui message box
void SetMediaTotalTime(lv_obj_t * total_time_obj_)
{
    total_time_obj = total_time_obj_;
}

void SetMediaProgress(lv_obj_t * progress_obj_)
{
    progress_obj = progress_obj_;
}

static void SetTotalTimeAndProgress(uint32_t total_time)
{
    if (total_time == 0) total_time = 100;
    lv_label_set_text_fmt(total_time_obj, "%02"LV_PRIu32":%02"LV_PRIu32":%02"LV_PRIu32, (total_time) / 3600, ((total_time) % 3600) / 60, (total_time) % 60);
    lv_slider_set_range(progress_obj, 0, total_time);
}

#ifdef HCCHIP_GCC
int MediaMonitorInit(media_handle_t *media_hld)
{
    current_media_hdl = media_hld;
    media_hld->msg_id = api_message_create(CTL_MSG_COUNT, sizeof(HCPlayerMsg));
    return API_SUCCESS;
}

int MediaMonitorDeinit(media_handle_t *media_hld)
{
    if (INVALID_ID == media_hld->msg_id)
        return API_SUCCESS;
    api_message_delete(media_hld->msg_id);
    media_hld->msg_id = INVALID_ID;
    media_hld->exit = 1;
    media_hld->msg_id = INVALID_ID;
    current_media_hdl = NULL;
    return API_SUCCESS;
}

void MediaMonitorTask(void)
{
    if (current_media_hdl != NULL) {
        if (!current_media_hdl->exit) {
            HCPlayerMsg msg;
            if (msgrcv(current_media_hdl->msg_id, (void *)&msg, sizeof(HCPlayerMsg) - sizeof(msg.type), 0, 0) != -1)
            {
                MediaMsgProc(current_media_hdl, &msg);
            }
        }
    }
}

static void MediaMsgProc(media_handle_t *media_hld, HCPlayerMsg *msg)
{
    if (!media_hld || !msg) return;
    switch (msg->type)
    {
    case HCPLAYER_MSG_STATE_EOS:
        printf (">> app get eos, normal play end!\n");
        //api_control_send_key(V_KEY_NEXT);
        break;
    case HCPLAYER_MSG_STATE_TRICK_EOS:
        printf (">> app get trick eos, fast play to end\n");
        //api_control_send_key(V_KEY_NEXT);
        break;
    case HCPLAYER_MSG_STATE_TRICK_BOS:
        printf (">> app get trick bos, fast back play to begining!\n");
        //api_control_send_key(V_KEY_PLAY);
        break;
    case HCPLAYER_MSG_OPEN_FILE_FAILED:
        printf (">> open file fail\n");
        break;
    case HCPLAYER_MSG_ERR_UNDEFINED:
        printf (">> error unknow\n");
        break;
    case HCPLAYER_MSG_UNSUPPORT_FORMAT:
        printf (">> unsupport format\n");
        break;
    case HCPLAYER_MSG_BUFFERING:
        printf(">> buffering %d\n", msg->val);
        break;
    case HCPLAYER_MSG_STATE_PLAYING:
        printf(">> player playing\n");
        break;
    case HCPLAYER_MSG_STATE_PAUSED:
        printf(">> player paused\n");
        break;
    case HCPLAYER_MSG_STATE_READY:
        printf(">> player ready\n");
        SetTotalTimeAndProgress(media_get_totaltime(media_hld));
        break;
    case HCPLAYER_MSG_READ_TIMEOUT:
        printf(">> player read timeout\n");
        break;
    case HCPLAYER_MSG_UNSUPPORT_ALL_AUDIO:
        printf(">> no audio track/or no supported audio track\n");
        break;
    case HCPLAYER_MSG_UNSUPPORT_ALL_VIDEO:
        printf(">> no video track/or no supported video track\n");
        break;
    case HCPLAYER_MSG_UNSUPPORT_VIDEO_TYPE:
        {
            HCPlayerVideoInfo video_info;
            char *video_type = "unknow";
            if (!hcplayer_get_nth_video_stream_info (media_hld->player, msg->val, &video_info)) {
                /* only a simple sample, app developers use a static struct to mapping them. */
                if (video_info.codec_id == HC_AVCODEC_ID_HEVC) {
                    video_type = "h265";
                } 
            }
            printf("unsupport video type %s\n", video_type);
        }
        break;
    case HCPLAYER_MSG_UNSUPPORT_AUDIO_TYPE:
        {
            HCPlayerAudioInfo audio_info;
            char *audio_type = "unknow";
            if (!hcplayer_get_nth_audio_stream_info (media_hld->player, msg->val, &audio_info)) {
                /* only a simple sample, app developers use a static struct to mapping them. */
                if (audio_info.codec_id < 0x11000) {
                    audio_type = "pcm";
                } else if (audio_info.codec_id < 0x12000) {
                    audio_type = "adpcm";
                } else if (audio_info.codec_id == HC_AVCODEC_ID_DTS) {
                    audio_type = "dts";
                } else if (audio_info.codec_id == HC_AVCODEC_ID_EAC3) {
                    audio_type = "eac3";
                } else if (audio_info.codec_id == HC_AVCODEC_ID_APE) {
                    audio_type = "ape";
                } 
            }
            printf("unsupport audio type %s\n", audio_type);
        }
        break;
    case HCPLAYER_MSG_AUDIO_DECODE_ERR:
        {
            printf("audio dec err, audio idx %d\n", msg->val);
            /* check if it is the last audio track, if not, then change to next one. */
            if (media_hld->player) {
                int total_audio_num = -1;
                total_audio_num = hcplayer_get_audio_streams_count(media_hld->player);
                if (msg->val >= 0 && total_audio_num > (msg->val + 1)) {
                    HCPlayerAudioInfo audio_info;
                    if (!hcplayer_get_cur_audio_stream_info(media_hld->player, &audio_info)) {
                        if (audio_info.index == msg->val) {
                            int idx = audio_info.index + 1;
                            while (hcplayer_change_audio_track(media_hld->player, idx)) {
                                idx++;
                                if (idx >= total_audio_num) {
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
        break;
    case HCPLAYER_MSG_VIDEO_DECODE_ERR:
        {
            printf("video dec err, video idx %d\n", msg->val);
            /* check if it is the last video track, if not, then change to next one. */
            if (media_hld->player) {
                int total_video_num = -1;
                total_video_num = hcplayer_get_video_streams_count(media_hld->player);
                if (msg->val >= 0 && total_video_num > (msg->val + 1)) {
                    HCPlayerVideoInfo video_info;
                    if (!hcplayer_get_cur_video_stream_info(media_hld->player, &video_info)) {
                        if (video_info.index == msg->val) {
                            int idx = video_info.index + 1;
                            while (hcplayer_change_video_track(media_hld->player, idx)) {
                                idx++;
                                if (idx >= total_video_num) {
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
        break;
    default:
        break;
    }
}
#endif