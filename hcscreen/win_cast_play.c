#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
//#include <lvgl/lvgl.h>
#include "lvgl/lvgl.h"
#include "../lvgl/src/font/lv_font.h"

#include "menu_mgr.h"
#include "com_api.h"
#include "cast_api.h"
#include "osd_com.h"

static volatile bool m_win_cast_play_open = false;

bool win_cast_play_wait_open(uint32_t timeout)
{
    uint32_t count;
    count = timeout/20;

    while(count--){
        if (m_win_cast_play_open)
            break;
        api_sleep_ms(20);
    }
    printf("%s(), m_win_cast_play_open(%d):%d\n", __func__, m_win_cast_play_open, count);
    return m_win_cast_play_open; 
}

static int win_cast_play_open(void *arg)
{
    printf("%s(), line: %d!\n", __func__, __LINE__);

	m_win_cast_play_open = true;
	return API_SUCCESS;
}

static int win_cast_play_close(void *arg)
{
    printf("%s(), line: %d!\n", __func__, __LINE__);
    //lv_obj_clean(lv_scr_act());
	m_win_cast_play_open = false;
    return API_SUCCESS;
}

static win_ctl_result_t win_cast_play_control(void *arg1, void *arg2)
{

	printf("%s()!\n", __FUNCTION__);

    (void)arg2;
    control_msg_t *ctl_msg = (control_msg_t*)arg1;
    win_ctl_result_t ret = WIN_CTL_SKIP;

    if(ctl_msg->msg_type == MSG_TYPE_CAST_AIRMIRROR_STOP ||
        ctl_msg->msg_type == MSG_TYPE_CAST_MIRACAST_STOP){

        ret = WIN_CTL_POPUP_CLOSE;
	}
	return ret;
}

win_des_t g_win_cast_play = 
{
    .open = win_cast_play_open,
    .close = win_cast_play_close,
    .control = win_cast_play_control,
};
