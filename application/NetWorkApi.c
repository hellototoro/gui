/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-08-19 15:46:16
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-09-11 21:51:09
 * @FilePath: /gui/application/NetWorkApi.c
 * @Description: None
 * @other: None
 */
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "NetWorkApi.h"

#define DEFAULT_AP_ADDR  "192.168.68.1"
#define DEFAULT_AP_PSW  "12345678"
#define DEFAULT_HOSTAPD_CONF  "/tmp/hostapd.conf"
#define HOST_AP_CHNNNEL 6

typedef struct wifi_ap_info{
	//struct list_head node;
	char 	ssid[32];     // AP's SSID
	//WIFI_ENCRYPT_MODE	encrypt_mode;   // Encrypt mode. frank:the old mode, not used in linux, must check again
	char 	key_index;                     // for WEP key index
	char	pwd[128];      // Password
	char 	pwd_count;        		    // the count of characters for password
	int 	quality;	//rssi strength
	short   enctypt_mode_support; //frank add.

	int network_id;// the ID is created from wpa command "add_network"
	char 	ap_index; //the index of the wifi ap list
}wifi_ap_info_t;

void *WiFi_handle = NULL;
wifi_ap_info_t *g_wifi_list;
int g_ap_count;

static int hc_dhcpc_start()
{
    FILE *fp = NULL;
    int ret = -1;

    char result_buf[256];
    char parse_buf1[128];
    char parse_buf2[128];
    char parse_buf3[128];

    snprintf(result_buf, sizeof(result_buf)-1, "udhcpc -b -i %s -q", "wlan0");

    fp = popen(result_buf, "r");
    if (!fp)
        return -1;

    printf("start hdcp: %s!\n", result_buf);

    while(fgets(result_buf, sizeof(result_buf), fp) != NULL){
        if('\n' == result_buf[strlen(result_buf)-1]){
            result_buf[strlen(result_buf)-1] = '\0';
        }

        if(sscanf(result_buf,"%s %s %s", parse_buf1, parse_buf2, parse_buf3) == 3){
        //If the hdcp success, it return string like that: "adding dns 192.168.51.1"
            // if (api_is_ip_addr(parse_buf3)){
            //     printf("DHCP success: %s!\n", parse_buf3);
            //     ret = 0;
            //     break;
            // }
        }
    }
    if (!fp)
        pclose(fp);   

    return ret;

}

static int hc_create_hostapd_conf(char *file_name)
{
    FILE *fp = NULL;
    char service_name[64] = {0};
    // cast_get_service_name(3, service_name, sizeof(service_name));

    fp = fopen(file_name, "w+");
    if (NULL == fp){
        printf("Create hostapd conf fail!\n");
        return -1;
    }
    fprintf(fp, "interface=%s\n", "wlan0");
    fprintf(fp, "driver=nl80211\n");
    fprintf(fp, "ctrl_interface_group=0\n");
    fprintf(fp, "hw_mode=g\n");
    fprintf(fp, "max_num_sta=2\n");
    fprintf(fp, "ssid=%s\n", service_name);
    fprintf(fp, "auth_algs=1\n");
    fprintf(fp, "channel=%d\n", HOST_AP_CHNNNEL);
    //fprintf(fp, "country_code=CN\n");
    //fprintf(fp, "ieee80211d=1\n");
    fprintf(fp, "wpa=2\n");
    fprintf(fp, "wpa_key_mgmt=WPA-PSK\n");
    fprintf(fp, "wpa_passphrase=%s\n", DEFAULT_AP_PSW);
    fprintf(fp, "rsn_pairwise=CCMP\n");
    fprintf(fp, "wpa_pairwise=CCMP\n");

    fflush(fp);
    int wfd = fileno(fp);
    if(wfd != -1)
        fsync(wfd); 

    fclose(fp);
    return 0;
}

static int hc_network_station_mode_init(void *wifi_handle)
{
    int ret = -1;

//just for test wifi WPA command
    wifi_ap_info_t *wifi_info = NULL;
    wifi_ap_info_t *wifi_list = NULL;
    int ap_count;
    int i;

    // ret = wifi_ap_scan(wifi_handle);
    if (ret == 0) 
        printf(" *** wifi_ap_scan() OK!");
    // ret = wifi_ap_list_get(wifi_handle, &wifi_list, &ap_count);
    if (ret == 0) 
        printf("*** wifi_ap_list_get() OK, ap_count = %d!\n", ap_count);
    //list the ap
    for (i = 0; i < ap_count; i ++){
        printf("ssid[%d]: %s, quality: %d%%\n", i, wifi_list[i].ssid, wifi_list[i].quality);
    }

    // wifi_info = wifi_choose_ap_from_scan(wifi_handle);
    if (NULL == wifi_info)
        return -1;

    printf("*** Connect to AP: %s ...!\n", wifi_info->ssid ? "wifi_info->ssid" : "NO AP");
    // ret = wifi_ap_connect(wifi_handle, wifi_info);
    if (ret == 0) 
        printf(" *** wifi_ap_connect() OK!");

    // ret = hc_dhcpc_start();
    if (ret == 0){
        //control_msg_t ctl_msg = {0};
        //ctl_msg.msg_type = MSG_TYPE_NETWORK_DHCP_ON;
        //api_control_send_msg(&ctl_msg);
    }    
    return 0;

}

static int hc_network_ap_mode_init()
{
    FILE *fp = NULL;
    int ret = -1;
    char result_buf[256];

    char *cmd1 = "ifconfig ""wlan0"" up";
    char *cmd2 = "ifconfig ""wlan0" " " DEFAULT_AP_ADDR;
    char *cmd3 = "hostapd "DEFAULT_HOSTAPD_CONF" -B";
    char *cmd4 = "dhcpcd";

    // ret = hc_create_hostapd_conf(DEFAULT_HOSTAPD_CONF);
    if (-1 == ret){
    }

    // ret = api_shell_exe_result(cmd1);
    if (-1 == ret)
        return ret;
    // ret = api_shell_exe_result(cmd2);
    if (-1 == ret)
        return ret;

    fp = popen(cmd3, "r");
    while(fgets(result_buf, sizeof(result_buf), fp) != NULL){
        if('\n' == result_buf[strlen(result_buf)-1]){
            result_buf[strlen(result_buf)-1] = '\0';
        }

        if (NULL != strstr(result_buf, "Failed")){
            printf("*** [%s] fail!\n", cmd3);
            pclose(fp);
            return -1;
        }
    }
    pclose(fp);

    // ret = api_shell_exe_result(cmd4);
    if (-1 == ret)
        return ret;

    return 0;
}

static int hc_network_p2p_mode_init(void)
{
    return 0;    
}

static void *hc_network_monitor_task(void *arg)
{
    static bool connect_draw = true;
    static bool disconnect_draw = true;
    int check_connect_cnt = 0;
    int check_connect_max = 20;
    // control_msg_t ctl_msg = {0};
    void *wifi_handle = NULL;
    
    while(1){

        if (NULL == wifi_handle){
            // wifi_handle = wifi_wpa_get_handle("wlan0");
            // api_sleep_ms(100);
            continue;
        }

        if (check_connect_cnt++ > check_connect_max){
            // if (true == wifi_ap_is_connected(wifi_handle)){
            //     //only update the OSD once time in connected status.
            //     if (connect_draw){
            //         ctl_msg.msg_type = MSG_TYPE_NETWORK_WIFI_CONNECTED;
            //         api_control_send_msg(&ctl_msg);
            //     }
            //     connect_draw = false;
            //     disconnect_draw = true;
            //     printf("...connect...\n");
            // }else{
            //     //only update the OSD once time in connected status.
            //     if (disconnect_draw){
            //         ctl_msg.msg_type = MSG_TYPE_NETWORK_WIFI_DISCONNECTED;
            //         api_control_send_msg(&ctl_msg);
            //     }
            //     disconnect_draw = false;
            //     connect_draw = true;
            //     printf("...disconnect...\n");
            // }
            check_connect_cnt = 0;
        }
        // api_sleep_ms(100);
    }

    return NULL;
}

static void hc_network_monitor_start()
{
    pthread_t thread_id = 0;
    pthread_attr_t attr;    
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 0x2000);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED); //release task resource itself
    if(pthread_create(&thread_id, &attr, hc_network_monitor_task, NULL)) {
        return;
    }
}

int NetWorkInit(void)
{
    // control_msg_t ctl_msg;

    if (-1 == wifi_driver_module_init()){
        return -1;
    }
    // ctl_msg.msg_type = MSG_TYPE_NETWORK_MAC_OK;
    // ctl_msg.msg_code = 0;
    // api_control_send_msg(&ctl_msg);

    WiFi_handle = wifi_wpa_open("wlan0");
    if (WiFi_handle) 
        printf("*** wifi_wpa_open() OK!\n");
    else
        return -1;

    // ctl_msg.msg_type = MSG_TYPE_NETWORK_INIT_OK;
    // api_control_send_msg(&ctl_msg);
    return 0;

    /*int sys_ap_list_count = 0;
    void *wifi_handle = NULL;
    int ret;
    control_msg_t ctl_msg;

    if (-1 == wifi_driver_module_init()){
        return -1;
    }
    ctl_msg.msg_type = MSG_TYPE_NETWORK_MAC_OK;
    ctl_msg.msg_code = 0;
    api_control_send_msg(&ctl_msg);

    //if there is WIFI ap information in system, that means the WIFI AP is
    // configured, initialize WIFI to station mode to connect AP.
    sys_ap_list_count = wifi_sys_ap_init_list();
    if (sys_ap_list_count > 0)
    {
        wifi_handle = wifi_wpa_open("wlan0");
        if (wifi_handle) 
            printf("*** wifi_wpa_open() OK!\n");
        else
            return -1;
        //hc_network_station_mode_init(wifi_handle);
    }
    else {
        hc_network_ap_mode_init();
    }

    ctl_msg.msg_type = MSG_TYPE_NETWORK_INIT_OK;
    api_control_send_msg(&ctl_msg);
    return -1;*/
}

void NetWorkDeInit(void)
{
    wifi_wpa_close(WiFi_handle);
}

void WiFi_Connect(wifi_ap_info_t *wifi_info)
{
    int ret = -1;
    // ret = wifi_ap_connect(WiFi_handle, wifi_info);
    if (ret == 0) 
        printf(" *** wifi_ap_connect() OK!");

    // ret = hc_dhcpc_start();
    if (ret == 0){
        // control_msg_t ctl_msg = {0};
        // ctl_msg.msg_type = MSG_TYPE_NETWORK_DHCP_ON;
        // api_control_send_msg(&ctl_msg);
    }
}

void WiFi_GetConnectedInfo(wifi_ap_info_t* info)
{
    
}

int WiFi_GetAPCount(void)
{
    return g_ap_count;
}

wifi_ap_info_t* WiFi_GetAPList(void)
{
    return g_wifi_list;
}



static void* WiFi_ScanTask(void* arg)
{
    NetWorkTaskCallBack WiFi_ScanCallBack = arg;
    // if (wifi_ap_scan(WiFi_handle) == 0) 
    //     printf(" *** wifi_ap_scan() OK!");
    // if (wifi_ap_list_get(WiFi_handle, &g_wifi_list, &g_ap_count) == 0) 
    //     printf("*** wifi_ap_list_get() OK, ap_count = %d!\n", g_ap_count);
    if(WiFi_ScanCallBack)
        WiFi_ScanCallBack();
    pthread_exit(NULL);
}

void WiFi_Scan(NetWorkTaskCallBack cb)
{
    int res;
    pthread_t thread_id = 0;
    pthread_attr_t attr;
    res = pthread_attr_init(&attr);
    if (res != 0) {
        perror("Attribute creation failed");
        exit(EXIT_FAILURE);
    }
    res = pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
    if (res != 0) {
        perror("Setting detached attribute failed");
        exit(EXIT_FAILURE);
    }
    res = pthread_create(&thread_id, &attr, WiFi_ScanTask, cb);
    if (res != 0) {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    }
}

bool WiFi_IsConnected(void)
{
    bool ret = false;
    return ret;
}
