#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <hcuapi/dis.h>
#include <ffplayer.h>
#include <fcntl.h>

#include "hcapi/lvgl/lvgl.h"
#include "hcapi/lv_drivers/display/fbdev.h"


#include "com_api.h"
#include "cast_api.h"
#include "os_api.h"


static uint32_t m_control_msg_id = INVALID_ID;
static cast_play_state_t m_cast_play_state = CAST_STATE_IDLE;
static bool m_ffplay_init = false;

/*#define DISP_BUF_SIZE (128 * 1024)*/
#define DISP_BUF_SIZE (1280 * 720)

int api_system_init()
{
    return 0;
}

int api_video_init()
{
    return 0;
}

int api_audio_init()
{
    return 0;
}

void fbdev_flush(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_p);

/*A small buffer for LittlevGL to draw the screen's content*/
static lv_color_t m_lvgl_buf[DISP_BUF_SIZE];

static void my_monitor_cb(lv_disp_drv_t * disp_drv, uint32_t time, uint32_t px)
{
	  printf("%d px refreshed in %d ms\n", px, time);
}

#if 0
int api_lvgl_init(int width, int height)
{
    lv_disp_t *dsp = NULL;

    /*LittlevGL init*/
    lv_init();

    /*Linux frame buffer device init*/
    fbdev_init();

    /*Initialize a descriptor for the buffer*/
    static lv_disp_draw_buf_t disp_buf;
    printf("m_lvgl_buf: %p, m_lvgl_buf + DISP_BUF_SIZE: %p\n", m_lvgl_buf, m_lvgl_buf + DISP_BUF_SIZE);
#if 1
    lv_disp_draw_buf_init(&disp_buf, m_lvgl_buf, NULL, DISP_BUF_SIZE);
#else
    lv_disp_draw_buf_init(&disp_buf, fbdev_static_malloc_virt(DISP_BUF_SIZE) , fbdev_static_malloc_virt(DISP_BUF_SIZE), DISP_BUF_SIZE);
#endif

    /*Initialize and register a display driver*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf   = &disp_buf;
    disp_drv.flush_cb   = fbdev_flush;
    disp_drv.hor_res    = width;//1920;
    disp_drv.ver_res    = height;//1080;
    disp_drv.monitor_cb = my_monitor_cb;
    dsp = lv_disp_drv_register(&disp_drv);
    if (!dsp)
    {
        printf("lv_disp_drv_register error!\n");
        return -1;
    }
    printf("init lv_disp_register: w-%d,h-%d\n",disp_drv.hor_res,disp_drv.ver_res);

    return 0;
}
#else
void *lv_mem_adr;
int api_lvgl_init(int width, int height)
{
    lv_disp_t *dsp = NULL;

    /*Linux frame buffer device init*/
    fbdev_init();
    lv_mem_adr = fbdev_static_malloc_virt(LV_MEM_SIZE);
    printf("lv_mem_adr: %p\n", lv_mem_adr);


    /*LittlevGL init*/
    lv_init();

    /*Initialize a descriptor for the buffer*/
    static lv_disp_draw_buf_t disp_buf;
    printf("m_lvgl_buf: %p, m_lvgl_buf + DISP_BUF_SIZE: %p\n", m_lvgl_buf, m_lvgl_buf + DISP_BUF_SIZE);
#if 0
    lv_disp_draw_buf_init(&disp_buf, m_lvgl_buf, NULL, DISP_BUF_SIZE);
#else
    /*lv_disp_draw_buf_init(&disp_buf, fbdev_static_malloc_virt(DISP_BUF_SIZE) , fbdev_static_malloc_virt(DISP_BUF_SIZE), DISP_BUF_SIZE);*/
    lv_disp_draw_buf_init(&disp_buf, fbdev_static_malloc_virt(DISP_BUF_SIZE * 4), fbdev_static_malloc_virt(DISP_BUF_SIZE * 4), DISP_BUF_SIZE);
#endif

    /*Initialize and register a display driver*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf   = &disp_buf;
    disp_drv.flush_cb   = fbdev_flush;
    disp_drv.hor_res    = width;//1920;
    disp_drv.ver_res    = height;//1080;
    disp_drv.full_refresh = 0;
    disp_drv.direct_mode = 0;
    /*disp_drv.monitor_cb = my_monitor_cb;*/
    dsp = lv_disp_drv_register(&disp_drv);
    if (!dsp)
    {
        printf("lv_disp_drv_register error!\n");
        return -1;
    }
    printf("init lv_disp_register: w-%d,h-%d\n",disp_drv.hor_res,disp_drv.ver_res);

    return 0;
}
#endif

static void *m_logo_player = NULL;
int api_logo_show(const char *file)
{

    char *file_path = file;
    
    api_logo_off();

    if (!file)
        file_path = BACK_LOGO;

    HCPlayerInitArgs player_args;
	memset(&player_args, 0, sizeof(player_args));
    player_args.uri = file_path;
	player_args.msg_id = -1;
    m_logo_player = hcplayer_create(&player_args);
    if (!m_logo_player){
        printf("hcplayer_create() fail!\n");
        return -1;
    }
    //not block play, create a task to play.
    //hcplayer_play(m_logo_player);
    
    //block play
    hcplayer_play2(m_logo_player);

    //stop 
    //hcplayer_stop2(m_logo_player, false, false);
    printf("Show logo: %s ok!\n", file_path);
    return 0;
}

int api_logo_off()
{
    if (m_logo_player)
        hcplayer_stop(m_logo_player);
    m_logo_player = NULL;
    return 0;
}

/**
 * @brief turn on/off the video frame output
 * 
 * @param on_off 
 * @return int 
 */
int api_dis_show_onoff(bool on_off)
{
    int fd = -1;
    struct dis_win_onoff winon = { 0 };

    fd = open("/dev/dis", O_WRONLY);
    if (fd < 0) {
        return -1;
    }

    winon.distype = DIS_TYPE_HD;
    winon.layer =  DIS_LAYER_MAIN;
    winon.on = on_off ? 1 : 0;
    
    ioctl(fd, DIS_SET_WIN_ONOFF, &winon);
    close(fd);

    return 0;
}

int api_control_send_msg(control_msg_t *control_msg)
{
    if (INVALID_ID == m_control_msg_id){
        m_control_msg_id = api_message_create(CTL_MSG_COUNT, sizeof(control_msg_t));
        if (INVALID_ID == m_control_msg_id){
            return -1;
        }
    }
    return api_message_send(m_control_msg_id, control_msg, sizeof(control_msg_t));
}

int api_control_receive_msg(control_msg_t *control_msg)
{
    if (INVALID_ID == m_control_msg_id){
        return -1;
    }
    return api_message_receive_tm(m_control_msg_id, control_msg, sizeof(control_msg_t), 5);
}

int api_control_send_key(uint32_t key)
{
    control_msg_t control_msg;
    control_msg.msg_type = MSG_TYPE_KEY;
    control_msg.msg_code = key;

    if (INVALID_ID == m_control_msg_id){
        m_control_msg_id = api_message_create(CTL_MSG_COUNT, sizeof(control_msg_t));
        if (INVALID_ID == m_control_msg_id){
            return -1;
        }
    }
    return api_message_send(m_control_msg_id, &control_msg, sizeof(control_msg_t));
}



cast_play_state_t api_cast_play_state_get(void)
{
    return m_cast_play_state;
}
void api_cast_play_state_set(cast_play_state_t state)
{
    m_cast_play_state = state;
}

/** check the string if it is IP address
 * @param
 * @return
 */
bool api_is_ip_addr(char *ip_buff)
{
    int ip1, ip2, ip3, ip4;
    char temp[64];
    if((sscanf(ip_buff,"%d.%d.%d.%d", &ip1, &ip2, &ip3, &ip4))!=4)
        return false;
    sprintf(temp,"%d.%d.%d.%d",ip1,ip2,ip3,ip4);
    if(strcmp(temp, ip_buff) != 0)
        return false;
    if(!((ip1 <= 255 && ip1 >= 0)&&(ip2 <= 255 && ip2 >= 0)&&(ip3 <= 255 && ip1 >= 0)))
        return false;
    else
        return true;
}


#define ETHE_MAC_PATH "/sys/class/net/eth0/address"
#define WIFI_MAC_PATH "/sys/class/net/wlan0/address"

#define MAC_ADDRESS_PATH   WIFI_MAC_PATH// ETHE_MAC_PATH
/**get the mac address, after modprobe cmmand, the ip address of wlan0
 * should be save tho "/sys/class/net/wlan0/address"
 * @param
 * @return
 */
int api_get_mac_addr(char *mac) 
{
    int ret;
    char buffer[32] = {0};
    FILE *fp = fopen(MAC_ADDRESS_PATH, "r");
    if (mac == NULL) return -1;
    if (fp == NULL) return -1;
    fread(buffer, 1, 32, fp);
    fclose(fp);
    buffer[strlen(buffer)-1] = '\0';
    printf(MAC_ADDRESS_PATH "=%s\n", buffer);
    ret = sscanf(buffer, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx",
            &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
    if(ret == 6)
        return 0;
    else
        return -1;

}


void app_ffplay_init()
{
    if (m_ffplay_init) return;

    hcplayer_init(5);
    m_ffplay_init = true;
}

void app_ffplay_deinit()
{
    hcplayer_deinit();
    m_ffplay_init = false;
}

/**
 * @brief linux system will send the exit signal, then release system resource here
 * 
 */
void app_exit()
{
    if (INVALID_ID != m_control_msg_id)
        api_message_delete(m_control_msg_id);

    app_ffplay_deinit();
#ifdef CAST_AIRPLAY_ENABLE    
    cast_airpaly_close();
#endif
#ifdef CAST_DLNA_ENABLE    
    cast_dlna_close();
#endif
#ifdef CAST_MIRACAST_ENABLE    
    cast_miracast_close();
#endif    
}

void api_sleep_ms(uint32_t ms)
{
    usleep(ms * 1000);
}

int api_shell_exe_result(char *cmd)
{
    int ret = API_FAILURE;
    char result_buf[32];
    int result_val = -1;
    FILE *fp = NULL;

    //step1: excute the shell command
    system(cmd);

    //step2: get the excuting result
    fp = popen("echo $?", "r");
    if (fgets(result_buf, sizeof(result_buf), fp) != NULL){
        if('\n' == result_buf[strlen(result_buf)-1]){
            result_buf[strlen(result_buf)-1] = '\0';
        }

        sscanf(result_buf,"%d", &result_val);
    }
    pclose(fp);
    if (0 == result_val){
        printf("Excute cmd: [%s] OK!\n", cmd);
        ret = API_SUCCESS;
    }else{
        printf("Excute cmd: [%s] fail, ret = %d!\n", cmd, result_val);
        ret = API_FAILURE;
    }

    return ret;

}
