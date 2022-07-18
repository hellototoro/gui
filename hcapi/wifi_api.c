/**
 * 
 */

#include <stdio.h> //printf()
#include <stdlib.h>
#include <string.h> //memcpy()
#include <unistd.h> //usleep()
//#include <stdbool.h> //bool
#include <stdint.h> //uint32_t_t
#include <pthread.h>
#include "com_api.h"
#include "wifi_api.h"


typedef struct{
    bool is_wifi_init;
    wifi_ap_info_t *wifi_info;
    int wifi_ap_count;
    bool is_scan_finished; 
    bool is_connected; 
    uint8_t cur_ap_select;
}wifi_wpa_status_t;

typedef struct{
    struct list_head node;

    char ifname[16]; //wlan0, wlan1, etc
    wifi_wpa_status_t wifi_status;
    wifi_ap_info_t *cur_wifi_info;

    void *wifi_ctrl;
    void *even_ctrl;

    pthread_mutex_t api_lock;
}wifi_wpa_handle_t;


static wifi_ap_info_t m_cur_wifi_info;


static LIST_HEAD (m_sys_ap_list_head);
static LIST_HEAD (m_wifi_handle_head);


#define WIFI_API_LOCK(lock) \
do{ \
    pthread_mutex_lock(lock); \
}while(0)
#define WIFI_API_UNLOCK(lock) \
do{ \
    pthread_mutex_unlock(lock); \
}while(0)

static char *m_wifi_module = NULL;

static void __on_scan_results_ready() {
    WifiAP* mdata;
    int data_cnt;
    wpa_wifi_infos_lock();
    wpa_wifi_infos_get(&mdata, &data_cnt);
    printf("bssid\tfreq(MHz)\tsignal_level_dBm\tflags\tssid\n");
    wpa_wifi_infos_dump2stdout(mdata, data_cnt);
    wpa_wifi_infos_unlock();
}

static int __on_wifi_connected(void *wifi_handle) 
{
    int ret = API_FAILURE;
    wifi_wpa_handle_t *handle = (wifi_wpa_handle_t*)(wifi_handle);
    ret = wpa_save_config(handle->wifi_ctrl);
    if (ret) {
    }
    return ret;
}


static void __wifi_wpa_event_status(enum WifiEventType event_type, const char* raw_msg, size_t raw_msg_len, void* userdata) 
{
    wifi_wpa_handle_t *handle = (wifi_wpa_handle_t *)(userdata);
    control_msg_t ctl_msg;    

    switch (event_type)
    {
    case WIFI_EVENT_SCAN_FINISHED:
        printf("[WIFI event]: wifi scan finished.\n");
        break;
    case WIFI_EVENT_SCAN_RESULTS_READY:
        handle->wifi_status.is_scan_finished = true;
        printf("[WIFI event]: wifi scan results ready.\n");
        if (1) __on_scan_results_ready();
        break;
    case WIFI_EVENT_CONNECTED:
        printf("[WIFI event]: wifi connected.\n");
        handle->wifi_status.is_connected = true;
        if (0) __on_wifi_connected(handle);
        break;
    case WIFI_EVENT_DISCONNECTED:
        handle->wifi_status.is_connected = false;
        printf("[WIFI event]: wifi disconnected.\n");
        break;
    case WIFI_EVENT_INVALID_PASSWD:
        handle->wifi_status.is_connected = false;
        ctl_msg.msg_type = MSG_TYPE_NETWORK_WIFI_PWD_WRONG;
        api_control_send_msg(&ctl_msg);

        printf("[WIFI event]: wifi passwd incorrect.\n");
        break;
    default:
        break;
    }
}

static int __wifi_api_init(void *wifi_handle)
{
    int ret = API_SUCCESS;
    wifi_wpa_handle_t *handle = (wifi_wpa_handle_t *)(wifi_handle);
    WIFI_API_LOCK(&handle->api_lock);
    do{
        if (true == handle->wifi_status.is_wifi_init){
            ret = API_SUCCESS;
            break;
        }
        memset(&handle->wifi_status, 0, sizeof(wifi_wpa_status_t));
        // init event function
        handle->even_ctrl = wpa_wifi_event_start(handle->ifname);
        if (NULL == handle->even_ctrl) {
            printf("wpa_wifi_event_start() fail!\n");
            ret = API_FAILURE;
            break;
        }
        ret = wpa_wifi_event_callback_add(&__wifi_wpa_event_status, (void*)wifi_handle, WPA_EVENT_ID_STATUS);
        if (ret) {
            printf("wpa_wifi_event_callback_add() fail!\n");
            ret = API_FAILURE;
            break;
        }

        // init control
        handle->wifi_ctrl = wpa_ctrl_init(handle->ifname);
        if (NULL == handle->wifi_ctrl) {
            printf("wpa_ctrl_init() fail!\n");
            ret = API_FAILURE;
            break;
        }

        handle->wifi_status.is_wifi_init = true;
    }while(0);

    WIFI_API_UNLOCK(&handle->api_lock);
    return ret;
}

static int __wifi_api_deinit(void *wifi_handle)
{
    // deinit
    int ret = API_SUCCESS;
    wifi_wpa_handle_t *handle = (wifi_wpa_handle_t *)(wifi_handle);

    WIFI_API_LOCK(&handle->api_lock);
    do{
        if (handle->wifi_ctrl){
            ret = wpa_ctrl_deinit(handle->wifi_ctrl);
            if (ret) {
                printf("wpa_ctrl_deinit() fail!\n");
                ret = API_FAILURE;
                break;
            }
        }

        if (handle->even_ctrl){
            ret = wpa_wifi_event_stop(handle->even_ctrl);
            if (ret) {
                printf("wpa_wifi_event_stop() fail!\n");
                ret = API_FAILURE;
                break;
            }
        }

        wpa_wifi_event_callback_clear();
        handle->wifi_status.is_wifi_init = false;
        handle->wifi_status.is_scan_finished = false;
        handle->wifi_status.is_connected = false;
    }while(0);
    WIFI_API_UNLOCK(&handle->api_lock);
    return ret;
}


int wifi_driver_module_init()
{
    int ret = API_FAILURE;
    FILE *fp = NULL;
    char *module = WIFI_MODULE_NAME;

    char result_buf[128];
    if (m_wifi_module)
        module = m_wifi_module;

#if 0
    fp = popen("lsmod", "r");
    while(fgets(result_buf, sizeof(result_buf), fp) != NULL){
        if('\n' == result_buf[strlen(result_buf)-1]){
            result_buf[strlen(result_buf)-1] = '\0';
        }

        if (strstr(result_buf, module)){
            printf("WIFI module already load!\n");
            pclose(fp);
            return API_SUCCESS; 
        }
    }
    pclose(fp);
#endif
    sprintf(result_buf, "modprobe %s", module);
    system(result_buf);

    api_sleep_ms(100);

    fp = popen("lsmod", "r");
    while(fgets(result_buf, sizeof(result_buf), fp) != NULL){
        if('\n' == result_buf[strlen(result_buf)-1]){
            result_buf[strlen(result_buf)-1] = '\0';
        }

        if (NULL != strstr(result_buf, module)){
            printf("WIFI module load OK!\n");
            pclose(fp);
            return API_SUCCESS; 
        }
    }
    pclose(fp);


    return ret;
}


/**
 * start scan ap list
 * 
 */
int wifi_ap_scan(void *wifi_handle)
{
    int ret = API_FAILURE;
    wifi_wpa_handle_t *handle = (wifi_wpa_handle_t *)(wifi_handle);

    WIFI_API_LOCK(&handle->api_lock);
    handle->wifi_status.is_scan_finished = false;
    ret = wpa_wifi_scan(handle->wifi_ctrl);
    WIFI_API_UNLOCK(&handle->api_lock);
    return ret;
}


/**
 * Get ap list after wifi_ap_scan()
 * wifi_info: output parameter, the wifi list information for user to access
 * ap_cnt: output parameter, the count of wifi AP list
 * return: -1, scan not finised; -2, error happen
 */
int wifi_ap_list_get(void *wifi_handle, wifi_ap_info_t **wifi_info, int *ap_list_cnt)
{
    WifiAP *ap_list;
    int ap_cnt;
    wifi_ap_info_t *wifi_info_tmp;
    wifi_wpa_handle_t *handle = (wifi_wpa_handle_t *)(wifi_handle); 
    int i;

    WIFI_API_LOCK(&handle->api_lock);

#if 1
    //wait for the wifi scan ready
    int wait_ready_cnt = 0;
    int wait_ready_max = 50;//wait 5000ms

    while(1){
        if (handle->wifi_status.is_scan_finished)
            break;
        
        if (wait_ready_cnt++ > wait_ready_max){
            WIFI_API_UNLOCK(&handle->api_lock);
            printf("WIFI scan not ready!\n");
            return -1;
        }
        api_sleep_ms(100);
    }
#endif

    wpa_wifi_infos_get(&ap_list, &ap_cnt);
    if (ap_cnt <= 0){
        WIFI_API_UNLOCK(&handle->api_lock);
        printf("wpa_wifi_infos_get fail!\n");
        return -2;
    }

    if (handle->wifi_status.wifi_info)
        free(handle->wifi_status.wifi_info);
    handle->wifi_status.wifi_info = (wifi_ap_info_t *)malloc(sizeof(wifi_ap_info_t)*ap_cnt);
    handle->wifi_status.wifi_ap_count = ap_cnt;

    if (NULL == handle->wifi_status.wifi_info) {
        WIFI_API_UNLOCK(&handle->api_lock);
        printf("handle->wifi_status.wifi_info is NULL!\n");
        return -2;
    }

    wifi_info_tmp = handle->wifi_status.wifi_info;
    for (i = 0; i < ap_cnt; i ++){
        if (wpa_wifi_flag_wep(ap_list)) {
            wifi_info_tmp->enctypt_mode_support |= ENCRYPT_SIMPLE_MODE_WEP;
            //frank, need modify????
            wifi_info_tmp->encrypt_mode = WIFI_ENCRYPT_MODE_SHARED_WEP;
        }
        if (wpa_wifi_flag_wpa(ap_list)){
            wifi_info_tmp->enctypt_mode_support |= ENCRYPT_SIMPLE_MODE_WPA;
            //frank, need modify????
            wifi_info_tmp->encrypt_mode = WIFI_ENCRYPT_MODE_WPAPSK_AES;
        }
        if (wpa_wifi_flag_wpa2(ap_list)) {
            wifi_info_tmp->enctypt_mode_support |= ENCRYPT_SIMPLE_MODE_WPA2;
            //frank, need modify????
            wifi_info_tmp->encrypt_mode = WIFI_ENCRYPT_MODE_WPA2PSK_AES;
        }
        if (wpa_wifi_flag_wpa3(ap_list)) {
            wifi_info_tmp->enctypt_mode_support |= ENCRYPT_SIMPLE_MODE_WPA3;
            //frank: need to be done???
        }

        strncpy(wifi_info_tmp->ssid, ap_list->ssid, WIFI_MAX_SSID_LEN);
        wifi_info_tmp->quality = (ap_list->signal_level_dBm/2) + 100;
        wifi_info_tmp->ap_index = i;

        ap_list ++;
        wifi_info_tmp ++;
    }

    *wifi_info = handle->wifi_status.wifi_info;
    *ap_list_cnt = handle->wifi_status.wifi_ap_count;

    WIFI_API_UNLOCK(&handle->api_lock);
    return API_SUCCESS;
}


/**
 * connent the wifi AP
 * wifi_info: this parameter should be form scan, storage system etc
 *            only change these parameters is OK
 *            wifi_info->ssid
 *            wifi_info->pwd
 *            wifi_info->pwd_count
 * 
 */
int wifi_ap_connect(void *wifi_handle, wifi_ap_info_t *wifi_info)
{
    int network_id = 0;
    bool is_wep_key = false;
    int ret = API_FAILURE;
    wifi_wpa_handle_t *handle = (wifi_wpa_handle_t *)(wifi_handle); 

    WIFI_API_LOCK(&handle->api_lock);
    handle->wifi_status.is_connected = false;

    do{
        /*
        ret = wpa_wifi_remove(handle->wifi_ctrl, network_id);
        if (ret) {
        }
        */

        ret = wpa_wifi_add(handle->wifi_ctrl, &network_id);
        if (ret) {
            break;
        }

        wifi_info->network_id = network_id;

        ret = wpa_wifi_set_ssid(handle->wifi_ctrl, network_id, wifi_info->ssid);
        if (ret) {
            break;
        }

        if ((wifi_info->encrypt_mode == WIFI_ENCRYPT_MODE_OPEN_WEP) ||
            (wifi_info->encrypt_mode == WIFI_ENCRYPT_MODE_SHARED_WEP)){
            is_wep_key = true;

        }
        else if ( (wifi_info->encrypt_mode == WIFI_ENCRYPT_MODE_WPAPSK_TKIP) ||
                (wifi_info->encrypt_mode == WIFI_ENCRYPT_MODE_WPAPSK_AES) ||
                (wifi_info->encrypt_mode == WIFI_ENCRYPT_MODE_WPA2PSK_TKIP) ||
                (wifi_info->encrypt_mode == WIFI_ENCRYPT_MODE_WPA2PSK_AES)){
            is_wep_key = false;
        }

        if (is_wep_key) {
            ret = wpa_wifi_set_wep_key(handle->wifi_ctrl, network_id, wifi_info->pwd);
        } else {
            ret = wpa_wifi_set_psk(handle->wifi_ctrl, network_id, wifi_info->pwd);
        }
        if (ret) {
            break;
        }

        ret = wpa_wifi_set_scan_ssid(handle->wifi_ctrl, network_id, 1);
        if (ret) {
            break;
        }

        ret = wpa_wifi_enable(handle->wifi_ctrl, network_id);
        if (ret) {
            break;
        }

        ret = wpa_wifi_autoreconnect(handle->wifi_ctrl, 1);
        if (ret) {
            break;
        }

        ret = wpa_wifi_select(handle->wifi_ctrl, network_id);
        if (ret) {
            break;
        }
        handle->wifi_status.cur_ap_select = wifi_info->ap_index;

        ret = API_SUCCESS;
    }while(0);

    printf("%s(), connent AP: %s OK!\n", __FUNCTION__, wifi_info->ssid);

#if 1
    //wait for connect is DONE!
    int wait_ready_cnt = 0;
    int wait_ready_max = 50;//wait 5000ms
    //wait for the wifi connect ready
    while(1){
        if (handle->wifi_status.is_connected)
            break;
        
        if (wait_ready_cnt++ > wait_ready_max){
            printf("WIFI connect timeout!\n");
            ret = API_FAILURE;
            break;            
        }
        api_sleep_ms(100);
    }
#endif
    wifi_connected_ap_save(handle, wifi_info);
    memcpy(&m_cur_wifi_info, wifi_info, sizeof(wifi_ap_info_t));
    handle->cur_wifi_info = &m_cur_wifi_info;


    WIFI_API_UNLOCK(&handle->api_lock);
	return ret;
}

/**
 * send the message of conneted status(fail/success)
 */
int wifi_ap_disconnect(void *wifi_handle, wifi_ap_info_t *wifi_info)
{
    wifi_wpa_handle_t *handle = (wifi_wpa_handle_t *)(wifi_handle);

    WIFI_API_LOCK(&handle->api_lock);
    wpa_wifi_disable(handle->wifi_ctrl, wifi_info->network_id);
    wpa_wifi_remove(handle->wifi_ctrl, wifi_info->network_id);
    handle->wifi_status.is_connected = false;
    handle->wifi_status.is_scan_finished = false;
    WIFI_API_UNLOCK(&handle->api_lock);
	return API_SUCCESS;
}

bool wifi_ap_is_connected(void *wifi_handle)
{
    if (!wifi_handle)
        return false;

    wifi_wpa_handle_t *handle = (wifi_wpa_handle_t *)(wifi_handle);
    return handle->wifi_status.is_connected;
}

/**
 * 
 */
int wifi_disconnect_current_ap(void *wifi_handle)
{
    wifi_wpa_handle_t *handle = (wifi_wpa_handle_t *)(wifi_handle);
    return wifi_ap_disconnect(handle, handle->cur_wifi_info);
}

wifi_ap_info_t *wifi_get_current_ap(void *wifi_handle)
{
    if (!wifi_handle)
        return NULL;

    wifi_wpa_handle_t *handle = (wifi_wpa_handle_t *)(wifi_handle);
    if (handle->wifi_status.is_connected)
        return handle->cur_wifi_info;

    return NULL;
}


/**
 * [wifi_connected_ap_save description] save the connected AP information to flash
 * @return [description]
 */
int wifi_connected_ap_save(void *wifi_handle, wifi_ap_info_t *wifi_info)
{
    wifi_ap_info_t *wifi_list = NULL;
    wifi_wpa_handle_t *handle = (wifi_wpa_handle_t *)(wifi_handle);

    //step 1: if the adding wifi AP is in the system AP list, delete first
    wifi_ap_info_t *wifi_list_tmp;
    list_for_each_entry_safe (wifi_list, wifi_list_tmp, &m_sys_ap_list_head, node) {
        if (0 == strcmp(wifi_info->ssid, wifi_list->ssid)){
            list_del(&(wifi_list->node));
            break;
        }
    }

    //step2: add the adding wifi AP to the head list link
    //add wifi_info before m_sys_ap_list_head. so when load wifi ap list
    //from flash, the last adding wifi AP will have the top priority to connect.
    list_add_tail(&wifi_info->node, &m_sys_ap_list_head);

    //step 3: save wifi information to flash. to be done.
    list_for_each_entry (wifi_list, &m_sys_ap_list_head, node) {
        //save wifi_list to flash //to be done
    }

    return API_SUCCESS;
}


/**
 * [wifi_choose_ap_from_scan description]get ap list from storage, compare with the scan result AP list.
 *     choose one AP from the scan result, return NULL if no match ap.
 * @return [description]
 */
wifi_ap_info_t *wifi_choose_ap_from_scan(void *wifi_handle)
{
    int i;
    wifi_wpa_handle_t *handle = (wifi_wpa_handle_t *)(wifi_handle);

    wifi_ap_info_t *wifi_info = NULL;
    list_for_each_entry (wifi_info, &m_sys_ap_list_head, node) {
        printf("%s(). sys AP:ssid:%s, pwd:%s \n", __FUNCTION__, wifi_info->ssid, wifi_info->pwd);
        for (i = 0; i < handle->wifi_status.wifi_ap_count; i ++){
            printf("%s(). [%d]:ssid:%s \n", __FUNCTION__, i, handle->wifi_status.wifi_info[i].ssid);
            if (0 == strcmp(wifi_info->ssid, handle->wifi_status.wifi_info[i].ssid)){
                printf("%s(), find system wifi (%s) from scan!\n", __FUNCTION__, wifi_info->ssid);
                return wifi_info;
            }
        }
    }

    printf("%s(), Can not find system wifi from scan!\n", __FUNCTION__);
    return NULL;
}


/**
 * [wifi_sys_ap_init_list description]Get AP list form system(flash or file), create a list link for AP list
 * @return [description] the AP list count from system (flash or file)
 */
int wifi_sys_ap_init_list()
{
    int i;
    int ap_list_cnt = 0;
    wifi_ap_info_t *ap_list = NULL;
    //step 1
    //read wifi infor from storage(flash or file)


#if 1
    //just temp test
    static wifi_ap_info_t tmp_ap_info[] = {
        {
            .ssid = "TP-LINK_WPA3",
            .pwd = "12345678",
            .pwd_count = 8,
            .enctypt_mode_support = WIFI_ENCRYPT_MODE_WPA2PSK_AES,
        },
        {
            .ssid = "TEST_11",
            .pwd = "dlna1234",
            .pwd_count = 8,
            .enctypt_mode_support = WIFI_ENCRYPT_MODE_WPA2PSK_AES,
        },
    };
    ap_list = tmp_ap_info;
    ap_list_cnt = sizeof(tmp_ap_info)/sizeof(wifi_ap_info_t);
#endif

    //step2 
    //init lint link
    for (i = 0; i < ap_list_cnt; i ++){
        //insert wifi_info before m_sys_ap_list_head
        list_add_tail(&ap_list[i].node, &m_sys_ap_list_head);
        printf("%s(). sys AP[%d]:ssid:%s, pwd:%s \n", __FUNCTION__, 
            i, ap_list[i].ssid, ap_list[i].pwd);
    }
    return ap_list_cnt;
}

/**
 * [wifi_sys_ap_count_get description]Get the count of AP list from system(flash or file)
 * @return [description]
 */
int wifi_sys_ap_count_get()
{
    int count = 0;

    wifi_ap_info_t *wifi_info = NULL;
    list_for_each_entry (wifi_info, &m_sys_ap_list_head, node) {
        count ++;
    }
    return count;
}


int wifi_p2p_mode_set()
{
    return API_SUCCESS;
}

int wifi_api_set_module(char *wifi_module)
{
    m_wifi_module = wifi_module;
    return API_SUCCESS;
}


void *wifi_wpa_open(char *ifname)
{
    int ret = API_FAILURE;
    wifi_wpa_handle_t *wifi_handle = NULL;
    wifi_handle = (wifi_wpa_handle_t *)malloc(sizeof(wifi_wpa_handle_t));

    //wifi_handle->api_lock = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_init(&wifi_handle->api_lock,NULL);

    strcpy(wifi_handle->ifname, ifname);
    ret = __wifi_api_init(wifi_handle);
    if (ret == API_FAILURE){
        printf("wifi_api_init fail!\n");
        return NULL;
    }


    //insert wifi_handle before m_wifi_handle_head
    list_add_tail(&wifi_handle->node, &m_wifi_handle_head);

    return wifi_handle;
}


int wifi_wpa_close(void *wifi_handle)
{
    wifi_wpa_handle_t *handle = (wifi_wpa_handle_t *)(wifi_handle);

    list_del_init(&handle->node);
    __wifi_api_deinit(handle);
    pthread_mutex_destroy(&handle->api_lock);

    if (handle)
        free(handle);

    return API_SUCCESS;
}

void *wifi_wpa_get_handle(char *ifname)
{
    wifi_wpa_handle_t *wifi_handle = NULL;
    list_for_each_entry (wifi_handle, &m_wifi_handle_head, node) {
        if (0 == strcmp(wifi_handle->ifname, ifname))
            return (void*)wifi_handle;
    }
    return NULL;
}
