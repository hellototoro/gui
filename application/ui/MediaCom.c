/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-13 13:31:24
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-06-13 16:19:12
 * @FilePath: /gui/application/ui/MediaCom.c
 * @Description: None
 * @other: None
 */
#include <stdlib.h>
#include "MediaCom.h"
#ifdef HCCHIP_GCC
#include <sys/msg.h>
#include <ffplayer.h>
#include <hcuapi/codec_id.h>
#include "hcapi/com_api.h"
#include "hcapi/os_api.h"
#endif

char current_path[100];

#ifdef HCCHIP_GCC
static void MediaMsgProc(media_handle_t *media_hld, HCPlayerMsg *msg);
#endif
//VideoList* video_list;

VideoList* CreatVideoList(void)
{
    VideoList* video_list = (VideoList*) malloc(sizeof(VideoList));
    InitDList(video_list);
    return video_list;
}

bool VideoListIsEmpty(VideoList* video_list)
{
    return (video_list->len ==0 ) ? true : false;
}

void AddToVideoList(VideoList* video_list, char * video_name)
{
    DListAppend(video_list, video_name);
}

char * GetNextVideoName(VideoList* video_list)
{
    static DLNode *next = NULL;
    static DLNode *head = NULL;
    static VideoList* last_list = NULL;

    if (video_list == NULL) {//清除获取记录
        last_list = NULL;
        next = NULL;
        head = NULL;
        return NULL;
    }

    if ((next == head) && (head != NULL)) { //一个循环结束
        next = head->next;
    }

    if (last_list != video_list) {
        last_list = video_list;
        head = video_list->head;
        next = head->next;
    }
    else {
        next = next->next;
    }
    return (char*) next->data;
}

char * GetPreVideoName(VideoList* video_list)
{
    static DLNode *pre = NULL;
    static DLNode *head = NULL;
    static VideoList* last_list = NULL;

    if (video_list == NULL) {//清除获取记录
        last_list = NULL;
        pre = NULL;
        head = NULL;
        return NULL;
    }

    if ((pre == head) && (head != NULL)) { //一个循环结束
        pre = head->pre;
    }

    if (last_list != video_list) {
        last_list = video_list;
        head = video_list->head;
        pre = head->pre;
    }
    else {
        pre = pre->pre;
    }
    return (char*) pre->data;
}

#ifdef HCCHIP_GCC
int MediaMonitorInit(media_handle_t *media_hld)
{
    media_hld->msg_id = api_message_create(CTL_MSG_COUNT, sizeof(HCPlayerMsg));
    return API_SUCCESS;
}

int MediaMonitorDeinit(media_handle_t *media_hld)
{
    if (INVALID_ID == media_hld->msg_id)
        return API_SUCCESS;
    api_message_delete(media_hld->msg_id);
    media_hld->msg_id = INVALID_ID;
    return API_SUCCESS;
}

void MediaMonitorTask(media_handle_t *media_hld)
{
    if (!media_hld->exit) {
        HCPlayerMsg msg;
        if (msgrcv(media_hld->msg_id, (void *)&msg, sizeof(HCPlayerMsg) - sizeof(msg.type), 0, 0) != -1)
        {
            MediaMsgProc(media_hld, &msg);
        }
    }
}

static void MediaMsgProc(media_handle_t *media_hld, HCPlayerMsg *msg)
{
    if (!media_hld || !msg) return;
    //printf("%s(), msg->type:%d\n", __FUNCTION__, (int)(msg->type));
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