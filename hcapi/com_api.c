#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <hcuapi/dis.h>
#include <ffplayer.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <hcuapi/fb.h>
#include <hcuapi/standby.h>

//#include "lvgl/lvgl.h"
//#include "lv_drivers/display/fbdev.h"
#include <hccast/hccast_wifi_mgr.h>
#include "gpio_ctrl.h"
#include "com_api.h"
#include "cast_api.h"
#include "os_api.h"


static uint32_t m_control_msg_id = INVALID_ID;
static cast_play_state_t m_cast_play_state = CAST_STATE_IDLE;
static bool m_ffplay_init = false;

void *g_gpio_reset_hld = NULL;


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

int api_lvgl_init(int width, int height)
{
    extern int hc_lvgl_init(void);
    return hc_lvgl_init();
}

static void *m_logo_player = NULL;
int api_logo_show(const char *file)
{
    char *file_path = file;
    
    if (m_logo_player){
        printf("%s(), line:%d. logo already show!\n", __func__, __LINE__);
        return -1;
    }

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
    printf("=============================Show logo: %s ok!=============================\n", file_path);

    /* if wifi is exist, probe it */
    system("/etc/wifiprobe.sh &");

    return 0;
}

int api_logo_off()
{
    if (m_logo_player)
    {
        //hcplayer_stop(m_logo_player);
       	hcplayer_stop2(m_logo_player,true,true);
    	m_logo_player = NULL;
		printf("=============================Close logo: ok!=============================\n");
    }	
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
 * should be saved to "/sys/class/net/wlan0/address"
 * @param
 * @return
 */
int api_get_mac_addr(char *mac) 
{
    int ret;
    char buffer[32] = {0};
    if (mac == NULL) 
        return -1;

    FILE *fp = fopen(MAC_ADDRESS_PATH, "r");
    if (fp == NULL) 
        return -1;

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

int api_get_wifi_freq_mode(void)
{
    return (int)hccast_wifi_mgr_freq_support_mode();
}

void app_ffplay_init(void)
{
    if (m_ffplay_init) return;

    hcplayer_init(1);
    m_ffplay_init = true;
}

void app_ffplay_deinit(void)
{
    hcplayer_deinit();
    m_ffplay_init = false;
}

/**
 * @brief linux system will send the exit signal, then release system resource here
 * 
 */
void app_exit(void)
{
    if (INVALID_ID != m_control_msg_id)
        api_message_delete(m_control_msg_id);

    app_ffplay_deinit();
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

static unsigned int crc_table[256];
static volatile bool m_crc_init = false;
static void api_crc_table_init(void)  
{  
    unsigned int c;  
    unsigned int i, j;  
    
    if (m_crc_init)
        return;

    for (i = 0; i < 256; i++) {  
        c = (unsigned int)i;  
        for (j = 0; j < 8; j++) {  
            if (c & 1)  
                c = 0xedb88320L ^ (c >> 1);  
            else  
                c = c >> 1;  
        }  
        crc_table[i] = c;  
    }  

    m_crc_init = true;
}  

unsigned int api_crc32(unsigned int crc,unsigned char *buffer, unsigned int size)  
{  
    unsigned int i;  

    api_crc_table_init();

    for (i = 0; i < size; i++) {  
        crc = crc_table[(crc ^ buffer[i]) & 0xff] ^ (crc >> 8);  
    }  
    return crc ;  
}  

int api_osd_show_onoff(bool on_off)
{
    // Open the file for reading and writing
    int fbfd = open("/dev/fb0", O_RDWR);
    uint32_t blank_mode;

    if(fbfd == -1) {
        printf("%s(), line: %d. Error: cannot open framebuffer device", __func__, __LINE__);
        return API_FAILURE;
    }

    if (on_off)
        blank_mode = FB_BLANK_UNBLANK;
    else
        blank_mode = FB_BLANK_NORMAL;

    if (ioctl(fbfd, FBIOBLANK, blank_mode) != 0) {
        printf("%s(), line: %d. Error: FBIOBLANK", __func__, __LINE__);
    }

    close(fbfd);
    return API_SUCCESS;
}

int api_gpio_init(void)
{
    gpio_info_t gpio_info;

    //reset gpio
    gpio_info.number = PINPAD_L08;
    gpio_info.direction = GPIO_DIR_IN;
    g_gpio_reset_hld = gpio_open(&gpio_info);

    if (NULL == g_gpio_reset_hld){
        return API_FAILURE;
    }

    return API_SUCCESS;
}

int api_gpio_deinit(void)
{
    if (g_gpio_reset_hld){
        gpio_close(g_gpio_reset_hld);
        g_gpio_reset_hld = NULL;
    }
    return API_SUCCESS;
}


void api_system_reboot(void)
{
    printf("%s(): reboot now!!\n", __func__);
    system("reboot");
    while(1);
}


/*
we can wake up by 3 ways: ir, gpio and sacadc key. 
 */
void api_system_standby(void)
{
    int fd = -1;

    fd = open("/dev/standby", O_RDWR);
    if (fd < 0) {
        printf("%s(), line:%d. open standby device error!\n", __func__, __LINE__);
        return;
    }

    printf("%s(), line:%d.\n", __func__, __LINE__);

    //step 1: off the display
    api_osd_show_onoff(false);
    api_logo_off();
    api_dis_show_onoff(false);
    //sleep for a while so that hardware display is really off.
    api_sleep_ms(100);
    

    //Step 2: off other devices


    //Step 3: config the standby wake up methods
        
    //config wake up ir scancode(so far, default is power key:28)   
    //check hclinux\SOURCE\linux-drivers\drivers\hcdrivers\rc\keymaps\rc-hcdemo.c
    //for scan code. 
    struct standby_ir_setting ir = { 0 };
    ir.num_of_scancode = 1;
    ir.scancode[0] = 28;
    ioctl(fd, STANDBY_SET_WAKEUP_BY_IR, (unsigned long)&ir);

    //config wake up GPIO
    struct standby_gpio_setting gpio = { 0 };
    gpio.pin = PINPAD_L08;
    gpio.polarity = 0;//low is active;
    ioctl(fd, STANDBY_SET_WAKEUP_BY_GPIO, (unsigned long)&gpio);

#if 0
    //config wake up adc key
    struct standby_saradc_setting adc = { 0 };
    adc.channel = 1;
    adc.min = 1000;
    adc.max = 1500;
    ioctl(fd, STANDBY_SET_WAKEUP_BY_SARADC, (unsigned long)&adc);
#endif    

#if 0
    //lower the volatage of ddr via the GPIO
    struct standby_pwroff_ddr_setting ddr = { 0 };
    ddr.pin = PINPAD_L09;
    ddr.polarity = 0;//low is active;
    ioctl(fd, STANDBY_SET_PWROFF_DDR, (unsigned long)&ddr);
#endif

    //Step 4: entering system standby
    ioctl(fd, STANDBY_ENTER, 0);
    close(fd);
    while(1);
}

static volatile int m_osd_off_time_cnt = 0;
#define ONE_COUNT_TIME  200
static void *osd_off_for_time(void *arg)
{
    uint32_t timeout = (uint32_t)arg;
    m_osd_off_time_cnt = timeout/ONE_COUNT_TIME;
    do{
        api_sleep_ms(ONE_COUNT_TIME);
    }while(m_osd_off_time_cnt --);

    m_osd_off_time_cnt = 0;
    api_osd_show_onoff(true);

    return NULL;
}

//Turn off OSD for a time, then turn on OSD.
//because sometimes enter dlna music play, the OSD is still show
//but the video screen is black(BUG #2848), so we turn off OSD for some time.
void api_osd_off_time(uint32_t timeout)
{

    //update the wait time
    if (m_osd_off_time_cnt){
        int timeout_cnt = timeout/ONE_COUNT_TIME;
        if (timeout_cnt > m_osd_off_time_cnt)
            m_osd_off_time_cnt = timeout_cnt;

        return;
    }

    api_osd_show_onoff(false);
    pthread_t thread_id = 0;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 0x1000);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED); //release task resource itself
    if (pthread_create(&thread_id, &attr, osd_off_for_time, (void*)timeout)) {
        return;
    }
}
