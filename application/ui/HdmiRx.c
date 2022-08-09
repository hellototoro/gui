/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-07-21 20:34:15
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-07-21 20:54:52
 * @FilePath: /gui/application/ui/HdmiRx.c
 * @Description: None
 * @other: None
 */
#include "HdmiRx.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#ifdef HCCHIP_GCC
#ifdef __linux__
#include <termios.h>
#include <signal.h>
#else
#include <kernel/lib/console.h>
#endif
#include <hcuapi/common.h>
#include <hcuapi/kshm.h>
#include <hcuapi/vidmp.h>
#include <hcuapi/hdmi_rx.h>
#include "wav.h"
#endif

lv_obj_t* HdmiRxScreen;
lv_group_t* HdmiRxGroup;

#ifdef HCCHIP_GCC
static pthread_t rx_audio_read_thread_id = 0;
static pthread_t rx_video_read_thread_id = 0;
static int rx_fd = -1;
static bool stop_read = 0;
static int akshm_fd = -1;
static int vkshm_fd = -1;
static FILE *arecfile = NULL;
static FILE *vrecfile = NULL;
static unsigned int rotate_mode = ROTATE_TYPE_0;

static void *rx_audio_read_thread(void *args);
static void *rx_video_read_thread(void *args);
static int hdmi_rx_stop(int argc , char *argv[]);
static int hdmi_rx_start(int argc , char *argv[]);
#endif

void anim_callback_set_x(lv_anim_t * a, int32_t v)
{
    lv_obj_set_x((lv_obj_t *)a->user_data, v);
}
int32_t anim_callback_get_x(lv_anim_t * a)
{
    return lv_obj_get_x_aligned((lv_obj_t *)a->user_data);
}
void anim_callback_set_y(lv_anim_t * a, int32_t v)
{
    lv_obj_set_y((lv_obj_t *)a->user_data, v);
}
int32_t anim_callback_get_y(lv_anim_t * a)
{
    return lv_obj_get_y_aligned((lv_obj_t *)a->user_data);
}

void AutoMoving_Animation(lv_obj_t * TargetObject, int delay)
{
    lv_anim_t PropertyAnimation_0;
    lv_anim_init(&PropertyAnimation_0);
    lv_anim_set_time(&PropertyAnimation_0, 1000);
    lv_anim_set_user_data(&PropertyAnimation_0, TargetObject);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_0, anim_callback_set_x);
    lv_anim_set_values(&PropertyAnimation_0, 0, 500);
    lv_anim_set_path_cb(&PropertyAnimation_0, lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_0, delay + 0);
    lv_anim_set_early_apply(&PropertyAnimation_0, false);
    lv_anim_set_get_value_cb(&PropertyAnimation_0, &anim_callback_get_x);
    lv_anim_start(&PropertyAnimation_0);

    lv_anim_t PropertyAnimation_1;
    lv_anim_init(&PropertyAnimation_1);
    lv_anim_set_time(&PropertyAnimation_1, 1000);
    lv_anim_set_user_data(&PropertyAnimation_1, TargetObject);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_1, anim_callback_set_y);
    lv_anim_set_values(&PropertyAnimation_1, 0, 500);
    lv_anim_set_path_cb(&PropertyAnimation_1, lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_1, delay + 0);
    lv_anim_set_early_apply(&PropertyAnimation_1, false);
    lv_anim_set_get_value_cb(&PropertyAnimation_1, &anim_callback_get_y);
    lv_anim_start(&PropertyAnimation_1);
}

void CreateHdmiRxScreen(void)
{
    HdmiRxScreen = lv_obj_create(NULL);
    //lv_obj_set_size(ui_SettingScreen, 1280, 720);
    lv_obj_clear_flag(HdmiRxScreen, LV_OBJ_FLAG_SCROLLABLE);
    //lv_obj_set_style_radius(HdmiRxScreen, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(HdmiRxScreen, lv_color_hex(0x3500FE), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(HdmiRxScreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(HdmiRxScreen, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* lab = lv_label_create(HdmiRxScreen);
    lv_obj_set_width(lab, 250);
    lv_obj_set_height(lab, 60);
    lv_obj_set_x(lab, 0);
    lv_obj_set_y(lab, 0);
    lv_obj_set_align(lab, LV_ALIGN_CENTER);
    lv_label_set_text(lab, "No single");
    lv_obj_set_style_text_letter_space(lab, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(lab, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(lab, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lab, &ui_font_MyFont38, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(lab, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(lab, lv_color_hex(0x135BAB), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(lab, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_disp_load_scr(HdmiRxScreen);
}



#ifdef HCCHIP_GCC
static void *rx_audio_read_thread(void *args)
{
    int data_size = 0;
    uint8_t *data = NULL;

    if(arecfile)
    {
        struct wave_header header = { 0 };
        generate_wave_header(&header , 44100 , 16 , 2);
        fwrite(&header , sizeof(struct wave_header) , 1 , arecfile);
    }

    //printf("rx_audio_read_thread run\n");
    while(!stop_read && akshm_fd >= 0)
    {
        AvPktHd hdr = { 0 };
        while(read(akshm_fd , &hdr , sizeof(AvPktHd)) != sizeof(AvPktHd))
        {
            //printf("read audio hdr from kshm err\n");
            usleep(20 * 1000);
            if(stop_read)
            {
                goto end;
            }
        }
        printf("apkt size %d\n" , (int)hdr.size);

        if(data_size < hdr.size)
        {
            data_size = hdr.size;
            if(data)
            {
                data = realloc(data , data_size);
            }
            else
            {
                data = malloc(data_size);
            }
            if(!data)
            {
                printf("no memory\n");
                goto end;
            }
        }

        while(read(akshm_fd , data , hdr.size) != hdr.size)
        {
            //printf("read audio data from kshm err\n");
            usleep(20 * 1000);
            if(stop_read)
            {
                goto end;
            }
        }

        //printf("adata: 0x%x, 0x%x, 0x%x, 0x%x\n", data[0], data[1], data[2], data[3]);
        if(arecfile)
        {
            fwrite(data , hdr.size , 1 , arecfile);
        }
        usleep(1000);
    }

end:
    if(data)
        free(data);

    (void)args;

    return NULL;
}

static void *rx_video_read_thread(void *args)
{
    int data_size = 0;
    uint8_t *data = NULL;

    //printf("rx_video_read_thread run\n");
    while(!stop_read && vkshm_fd >= 0)
    {
        AvPktHd hdr = { 0 };
        while(read(vkshm_fd , &hdr , sizeof(AvPktHd)) != sizeof(AvPktHd))
        {
            //printf("read audio hdr from kshm err\n");
            usleep(20 * 1000);
            if(stop_read)
            {
                goto end;
            }
        }
        printf("vpkt size %d\n" , (int)hdr.size);

        if(data_size < hdr.size)
        {
            data_size = hdr.size;
            if(data)
            {
                data = realloc(data , data_size);
            }
            else
            {
                data = malloc(data_size);
            }
            if(!data)
            {
                printf("no memory\n");
                goto end;
            }
        }

        while(read(vkshm_fd , data , hdr.size) != hdr.size && !stop_read)
        {
            //printf("read audio data from kshm err\n");
            usleep(20 * 1000);
            if(stop_read)
            {
                goto end;
            }
        }

        //printf("vdata: 0x%x, 0x%x, 0x%x, 0x%x\n", data[0], data[1], data[2], data[3]);
        if(vrecfile)
        {
            fwrite(data , hdr.size , 1 , vrecfile);
        }
        usleep(1000);
    }

end:
    if(data)
        free(data);

    (void)args;

    return NULL;
}

static int hdmi_rx_stop(int argc , char *argv[])
{
    stop_read = 1;
    if(rx_audio_read_thread_id)
        pthread_join(rx_audio_read_thread_id , NULL);
    if(rx_video_read_thread_id)
        pthread_join(rx_video_read_thread_id , NULL);
    rx_audio_read_thread_id = rx_video_read_thread_id = 0;

    if(arecfile)
        fclose(arecfile);
    if(vrecfile)
        fclose(vrecfile);
    arecfile = vrecfile = NULL;

    if(akshm_fd >= 0)
        close(akshm_fd);
    if(vkshm_fd >= 0)
        close(vkshm_fd);
    akshm_fd = vkshm_fd = -1;

    if(rx_fd >= 0)
    {
        ioctl(rx_fd , HDMI_RX_STOP);
        ioctl(rx_fd , HDMI_RX_SET_VIDEO_ROTATE_MODE , 0);
        close(rx_fd);
    }
    rx_fd = -1;

    (void)argc;
    (void)argv;
    return 0;
}

static int hdmi_rx_start(int argc , char *argv[])
{
    enum HDMI_RX_VIDEO_DATA_PATH vpath = HDMI_RX_VIDEO_TO_OSD;
    enum HDMI_RX_AUDIO_DATA_PATH apath = HDMI_RX_AUDIO_TO_HDMI_TX;
    struct kshm_info rx_audio_read_hdl = { 0 };
    struct kshm_info rx_video_read_hdl = { 0 };

    pthread_attr_t attr;
    int opt;

    opterr = 0;
    optind = 0;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr , 0x1000);

    if(rx_fd >= 0)
    {
        hdmi_rx_stop(0, 0);
    }

    rx_fd = open("/dev/hdmi_rx" , O_WRONLY);
    if(rx_fd < 0)
    {
        goto err;
    }

    while((opt = getopt(argc , argv , "a:v:r:")) != EOF)
    {
        switch(opt)
        {
            case 'a':
                apath = atoi(optarg);
                break;
            case 'v':
                vpath = atoi(optarg);
                break;
            case 'r':
                rotate_mode = atoi(optarg) % 4;
                break;
            default:
                break;
        }
    }

    printf("apath %d, vpath %d\n" , apath , vpath);
    ioctl(rx_fd , HDMI_RX_SET_VIDEO_DATA_PATH , vpath);
    ioctl(rx_fd , HDMI_RX_SET_AUDIO_DATA_PATH , apath);

    if(apath == HDMI_RX_AUDIO_TO_KSHM)
    {
        arecfile = fopen("/media/hdd/astream" , "wb");
        if(!arecfile)
        {
            goto err;
        }

        akshm_fd = open("/dev/kshmdev" , O_RDONLY);
        if(akshm_fd < 0)
        {
            goto err;
        }

        ioctl(rx_fd , HDMI_RX_AUDIO_KSHM_ACCESS , &rx_audio_read_hdl);
        printf("get audio hdl, kshm desc 0x%x\n" , (int)rx_audio_read_hdl.desc);
        ioctl(akshm_fd , KSHM_HDL_SET , &rx_audio_read_hdl);

        stop_read = 0;
        if(pthread_create(&rx_audio_read_thread_id , &attr ,
                          rx_audio_read_thread , NULL))
        {
            printf("audio kshm recv thread create failed\n");
            goto err;
        }
    }

    if(vpath == HDMI_RX_VIDEO_TO_KSHM ||
       vpath == HDMI_RX_VIDEO_TO_DE_AND_KSHM ||
       vpath == HDMI_RX_VIDEO_TO_DE_ROTATE_AND_KSHM)
    {
        vrecfile = fopen("/media/hdd/vstream" , "wb");
        if(!vrecfile)
        {
            goto err;
        }

        vkshm_fd = open("/dev/kshmdev" , O_RDONLY);
        if(vkshm_fd < 0)
        {
            goto err;
        }

        ioctl(rx_fd , HDMI_RX_VIDEO_KSHM_ACCESS , &rx_video_read_hdl);
        printf("get video hdl, kshm desc 0x%x\n" , (int)rx_video_read_hdl.desc);
        ioctl(vkshm_fd , KSHM_HDL_SET , &rx_video_read_hdl);

        stop_read = 0;
        if(pthread_create(&rx_video_read_thread_id , &attr ,
                          rx_video_read_thread , NULL))
        {
            printf("video kshm recv thread create failed\n");
            goto err;
        }
    }

    if(vpath == HDMI_RX_VIDEO_TO_DE_ROTATE ||
       vpath == HDMI_RX_VIDEO_TO_DE_ROTATE_AND_KSHM)
    {
        printf("rotate_mode = 0x%x\n" , rotate_mode);
        ioctl(rx_fd , HDMI_RX_SET_VIDEO_ROTATE_MODE , rotate_mode);
    }

    ioctl(rx_fd , HDMI_RX_START);
    printf("hdmi_rx start ok```\n");

    return 0;

err:
    hdmi_rx_stop(0 , 0);
    return -1;
}
#endif
