/*
hotplug_mgr.c: to manage the hotplug device, such as usb wifi, usb disk etc
 */

#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <netdb.h>
#include <hccast/hccast_net.h>
#include <hcuapi/common.h>
#include <hcuapi/kumsgq.h>
#include <hcuapi/hdmi_tx.h>
#include "tv_sys.h"
#include "com_api.h"
#include "network_api.h"
#include "data_mgr.h"
#include "hotplug_mgr.h"

#define MX_EVNTS (10)
#define EPL_TOUT (1000)

#define DEV_HDMI  "/dev/hdmi"

static int m_hdmi_tx_plugin = 0;
static int m_hdmi_rx_plugin = 0;
static int m_usb_plugout = -1;
static int m_wifi_plugin = 0;

enum EPOLL_EVENT_TYPE
{
    EPOLL_EVENT_TYPE_KUMSG = 0,
    EPOLL_EVENT_TYPE_HOTPLUG_CONNECT,
    EPOLL_EVENT_TYPE_HOTPLUG_MSG,
};

struct epoll_event_data
{
    int fd;
    enum EPOLL_EVENT_TYPE type;
};

static struct epoll_event_data kumsg_data = { 0 };
static struct epoll_event_data hotplg_data = { 0 };
static struct epoll_event_data hotplg_msg_data = { 0 };

static int epoll_fd = -1;

static void process_hotplug_msg(char *msg)
{
    //plug-out: ACTION=wifi-remove INFO=v0BDApF179
    //plug-in: ACTION=wifi-add INFO=v0BDApF179

    control_msg_t ctl_msg = {0};
    const char *plug_msg;

    plug_msg = (const char*)msg;
    if(strstr(plug_msg, "ACTION=wifi"))
    {
        //usb wifi plugin/plugout message
        if (strstr(plug_msg, "ACTION=wifi-remove"))
        {
            m_wifi_plugin = 0;
            printf("Wi-Fi plug-out\n");
            network_wifi_module_set(0);
            ctl_msg.msg_type = MSG_TYPE_USB_WIFI_PLUGOUT;
            api_control_send_msg(&ctl_msg);
        }
        else if (strstr(plug_msg, "ACTION=wifi-add"))
        {
            m_wifi_plugin = 1;
            if (strstr(plug_msg, "INFO=v0BDApF179"))
            {
                printf("Wi-Fi probed RTL8188FTV\n");
                network_wifi_module_set(HCCAST_NET_WIFI_8188FTV);

            }
            else if (strstr(plug_msg, "INFO=v0BDApC811"))
            {
                printf("Wi-Fi probed RTL8811FTV\n");
                network_wifi_module_set(HCCAST_NET_WIFI_8811FTV);
            }

            ctl_msg.msg_type = MSG_TYPE_USB_WIFI_PLUGIN;
            api_control_send_msg(&ctl_msg);
        }
    }
    else
    {
        //usb disk plugin/plugout message
        uint8_t mount_name[32];
        //usb-disk is plug in (SD??)
        if (strstr(plug_msg, "ACTION=mount"))
        {
            sscanf(plug_msg, "ACTION=mount INFO=%s", mount_name);
            printf("U-disk is plug in: %s\n", mount_name);
            m_usb_plugout = 0;
            ctl_msg.msg_type = MSG_TYPE_USB_DISK_PLUGIN;
            //Enter upgrade window if there is upgraded file in USB-disk(hc_upgradexxxx.bin)
            //sys_upg_usb_check(5000);

        }    
        else if (strstr(plug_msg, "ACTION=umount"))
        {
            m_usb_plugout = 1;
            sscanf(plug_msg, "ACTION=mount INFO=%s", mount_name);
            printf("U-disk is plug out: %s\n", mount_name);
            ctl_msg.msg_type = MSG_TYPE_USB_DISK_PLUGOUT;
        }    
        api_control_send_msg(&ctl_msg);
    }
}

static void do_kumsg(KuMsgDH *msg)
{
    int ap_tv_sys;
    int ap_tv_sys_ret;
	int last_tv_sys;
    switch (msg->type) {
    case HDMI_TX_NOTIFY_CONNECT:
        m_hdmi_tx_plugin = 1;
        printf("%s(), line: %d. hdmi tx connect\n", __func__, __LINE__);
        break;
    case HDMI_TX_NOTIFY_DISCONNECT:
        m_hdmi_tx_plugin = 0;
        printf("%s(), line: %d. hdmi tx disconnect\n", __func__, __LINE__);
        break;
    case HDMI_TX_NOTIFY_EDIDREADY: {
        struct hdmi_edidinfo *edid = (struct hdmi_edidinfo *)&msg->params;
        printf("%s(), best_tvsys: %d\n", __func__, edid->best_tvsys);

		last_tv_sys = data_mgr_de_tv_sys_get();
        ap_tv_sys = data_mgr_app_tv_sys_get();
        ap_tv_sys_ret = tv_sys_app_auto_set(ap_tv_sys, 2000);
        if (ap_tv_sys_ret >= 0){
            if (APP_TV_SYS_AUTO == ap_tv_sys)
                data_mgr_app_tv_sys_set(APP_TV_SYS_AUTO);
            else
                data_mgr_app_tv_sys_set(ap_tv_sys_ret);
            data_mgr_save();
        }

		if(((last_tv_sys < TV_LINE_4096X2160_30) && (data_mgr_de_tv_sys_get() >= TV_LINE_4096X2160_30)) \
			|| ((last_tv_sys >= TV_LINE_4096X2160_30) && (data_mgr_de_tv_sys_get() < TV_LINE_4096X2160_30)))
		{
			control_msg_t ctl_msg = {0};
			ctl_msg.msg_type = MSG_TYPE_HDMI_TX_CHANGED;
			api_control_send_msg(&ctl_msg);
		}
		
        break;
    }
    default:
        break;
    }
}


static void *hotplug_receive_event_func(void *arg)
{
    struct epoll_event events[MX_EVNTS];
    int n = -1;
    int i;
    struct sockaddr_in client;
    socklen_t sock_len = sizeof(client);
    int len;

    while (1)
    {
        n = epoll_wait(epoll_fd, events, MX_EVNTS, EPL_TOUT);
        if (n == -1)
        {
            if (EINTR == errno)
            {
                continue;
            }
            usleep(100 * 1000);
            continue;
        }
        else if (n == 0)
        {
            continue;
        }

        for (i = 0; i < n; i++)
        {
            struct epoll_event_data *d = (struct epoll_event_data *)events[i].data.ptr;
            int fd = (int)d->fd;
            enum EPOLL_EVENT_TYPE type = d->type;

            switch (type)
            {
                case EPOLL_EVENT_TYPE_KUMSG: 
                {
                    unsigned char msg[MAX_KUMSG_SIZE] = {0};
                    len = read(fd, (void*)msg, MAX_KUMSG_SIZE);
                    if(len > 0) {
                        do_kumsg((KuMsgDH *)msg);
                    }
                    break;
                }
                case EPOLL_EVENT_TYPE_HOTPLUG_CONNECT:
                {
                    printf("%s(), line: %d. get hotplug connect...\n", __func__, __LINE__);
                    struct epoll_event ev;
                    int new_sock = accept(fd, (struct sockaddr *)&client, &sock_len);
                    if (new_sock < 0)
                        break;

                    hotplg_msg_data.fd = new_sock;
                    hotplg_msg_data.type = EPOLL_EVENT_TYPE_HOTPLUG_MSG;
                    ev.events = EPOLLIN;
                    ev.data.ptr = (void *)&hotplg_msg_data;
                    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_sock, &ev);
                    break;
                }
                case EPOLL_EVENT_TYPE_HOTPLUG_MSG:
                {
                    printf("%s(), line: %d. get hotplug msg...\n", __func__, __LINE__);
                    char msg[128] = {0};
                    len = read(fd, (void*)msg, sizeof(msg) - 1);
                    if (len > 0)
                    {
                        printf("%s\n", msg);
                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                        if (strstr(msg, "ACTION="))
                        {
                            process_hotplug_msg(msg);
                        }
                    }
                    else
                    {
                        printf("read hotplug msg fail\n");
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }

    return NULL;
}


void hotplug_init(void)
{
    pthread_attr_t attr;
    pthread_t tid;
    struct sockaddr_un serv;
    struct epoll_event ev;
    int hotplug_fd = -1;
    int ret;

    int hdmi_tx_fd = -1;
    int kumsg_fd = -1;
    struct kumsg_event event = { 0 };

    // start hot-plug detect
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 0x2000);
    if (pthread_create(&tid, &attr, hotplug_receive_event_func, (void *)NULL))
    {
        printf("pthread_create receive_event_func fail\n");
        goto out;
    }

    hotplug_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (hotplug_fd < 0)
    {
        printf("socket error\n");
        goto out;
    }
    else
    {
        printf("socket success\n");
    }

    unlink("/tmp/hotplug.socket");
    bzero(&serv, sizeof(serv));
    serv.sun_family = AF_LOCAL;
    strcpy(serv.sun_path, "/tmp/hotplug.socket");
    ret = bind(hotplug_fd, (struct sockaddr *)&serv, sizeof(serv));
    if (ret < 0)
    {
        printf("bind error\n");
        goto out;
    }
    else
    {
        printf("bind success\n");
    }

    ret = listen(hotplug_fd, 1);
    if (ret < 0)
    {
        printf("listen error\n");
        goto out;
    }
    else
    {
        printf("listen success\n");
    }

    epoll_fd = epoll_create1(0);


    hotplg_data.fd = hotplug_fd;
    hotplg_data.type = EPOLL_EVENT_TYPE_HOTPLUG_CONNECT;
    ev.events = EPOLLIN;
    ev.data.ptr = (void *)&hotplg_data;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, hotplug_fd, &ev) != 0)
    {
        printf("EPOLL_CTL_ADD hotplug fail\n");
        goto out;
    }
    else
    {
        printf("EPOLL_CTL_ADD hotplug success\n");
    }

    hdmi_tx_fd = open(DEV_HDMI, O_RDWR);  
    if (-1 == hdmi_tx_fd){
        printf("open device %s error!\n", __func__);
        goto out;
    }
    kumsg_fd = ioctl(hdmi_tx_fd, KUMSGQ_FD_ACCESS, O_CLOEXEC);
    kumsg_data.fd = kumsg_fd;
    kumsg_data.type = EPOLL_EVENT_TYPE_KUMSG;
    ev.events = EPOLLIN;
    ev.data.ptr = (void *)&kumsg_data;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, kumsg_fd, &ev) != 0) {
        printf("EPOLL_CTL_ADD fail\n");
        goto out;
    }
    event.evtype = HDMI_TX_NOTIFY_CONNECT;
    event.arg = 0;
    ret = ioctl(hdmi_tx_fd, KUMSGQ_NOTIFIER_SETUP, &event);
    if (ret) {
        printf("KUMSGQ_NOTIFIER_SETUP 0x%08x fail\n", (int)event.evtype);
        goto out;
    }

    event.evtype = HDMI_TX_NOTIFY_DISCONNECT;
    event.arg = 0;
    ret = ioctl(hdmi_tx_fd, KUMSGQ_NOTIFIER_SETUP, &event);
    if (ret) {
        printf("KUMSGQ_NOTIFIER_SETUP 0x%08x fail\n", (int)event.evtype);
        goto out;
    }

    event.evtype = HDMI_TX_NOTIFY_EDIDREADY;
    event.arg = 0;
    ret = ioctl(hdmi_tx_fd, KUMSGQ_NOTIFIER_SETUP, &event);
    if (ret) {
        printf("KUMSGQ_NOTIFIER_SETUP 0x%08x fail\n", (int)event.evtype);
        goto out;
    }


out:
    return ;
}

int hotplug_hdmi_tx_get(void)
{
    return m_hdmi_tx_plugin;
}

int hotplug_hdmi_rx_get(void)
{
    return m_hdmi_rx_plugin;
}

int hotplug_usb_plugout(void)
{
    return m_usb_plugout;
}

int hotplug_wiif_get(void)
{
    return m_wifi_plugin;
}
