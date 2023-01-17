#include <pthread.h>
#include <linux/input.h>
#include <sys/poll.h>
#include <fcntl.h>
#include "key.h"
#include "hcapi/com_api.h"
#include "application/key_map.h"
#include <errno.h>
#include <time.h>

static int fd_ir_key;
static int fd_adc_key;

/*Initialize your keypad*/
static void keypad_init(void)
{
    /*Your code comes here*/
    fd_ir_key = open("/dev/input/event0", O_RDONLY);
    fd_adc_key = open("/dev/input/event1", O_RDONLY);
}

static uint32_t keypad_get_key(int key_code, int key_pressing, int fd_key)
{
    /*Your code comes here*/
    struct pollfd pfd;
    struct input_event t;
    pfd.fd = fd_key;
    pfd.events = POLLIN | POLLRDNORM;

    while(poll(&pfd, 1, 0) > 0) {
        if (read(fd_key, &t, sizeof(t)) == sizeof(t) && (t.type == EV_KEY)) {
            key_code = t.code;
            key_pressing = t.value;
            break;
        }   
    }
    return key_pressing ? key_code: 0;
}

/*Will be called by the library to read the mouse*/
static inline void keypad_read(uint32_t act_key, lv_indev_data_t * data)
{
    /*Get whether the a key is pressed and save the pressed key*/
    if(act_key != 0) {
        data->state = LV_INDEV_STATE_PR;
        /*Translate the keys to LVGL control characters according to your key definitions*/
        switch(act_key) {
        case KEY_UP:
            act_key = LV_KEY_UP;
            break;
        case KEY_DOWN:
            act_key = LV_KEY_DOWN;
            break;
        case KEY_LEFT:
            act_key = LV_KEY_LEFT;
            break;
        case KEY_RIGHT:
            act_key = LV_KEY_RIGHT;
            break;
        case KEY_OK:
            act_key = LV_KEY_ENTER;
            break;
        case KEY_NEXT:
            act_key = LV_KEY_NEXT;
            break;
        case KEY_PREVIOUS:
            act_key = LV_KEY_PREV;
            break;
        case KEY_EXIT:
            act_key = LV_KEY_ESC;
            break;
        case KEY_VOLUMEUP:
            act_key = LV_KEY_VOLUME_UP;
            break;
        case KEY_VOLUMEDOWN:
            act_key = LV_KEY_VOLUME_DOWN;
            break;
        case KEY_MENU:
            act_key = LV_KEY_MENU;
            break;
        case KEY_RED:
            act_key = LV_KEY_SOURCE;
            break;
        default:
            act_key = USER_KEY_FLAG | act_key;
            break;
        }
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
    data->key = act_key;
}

static void ir_keypad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    ((void)indev_drv);
    static int key_code = 0;
    static int key_pressing = false;
    static time_t last_time = 0;
    static uint32_t last_key = 0;
    uint32_t act_key = keypad_get_key(fd_ir_key);
    time_t cur_time = time(NULL);
    if (act_key != 0) {
        time_t diff = difftime(cur_time, last_time)*10;
        if (diff < 20) {
            last_key = act_key;
            act_key = 0;
        } else {
            last_key = 0;
        }
    } else {
        act_key = last_key;
        last_key = 0;
        last_time = time(NULL);
    }
    keypad_read(act_key, data);
}

static void adc_keypad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    ((void)indev_drv);
    static int key_code = 0;
    static int key_pressing = false;
    static time_t last_time = 0;
    static uint32_t last_key = 0;
    uint32_t act_key = keypad_get_key(fd_adc_key);
    time_t cur_time = time(NULL);
    if (act_key != 0) {
        time_t diff = difftime(cur_time, last_time)*10;
        if (diff < 20) {
            last_key = act_key;
            act_key = 0;
        } else {
            last_key = 0;
        }
    } else {
        act_key = last_key;
        last_key = 0;
        last_time = time(NULL);
    }
    keypad_read(act_key, data);
}

void key_init(void)
{
    static lv_indev_drv_t ir_keypad_driver;
    static lv_indev_drv_t adc_keypad_driver;

    keypad_init();
    /* ir keypad */
    lv_indev_drv_init(&ir_keypad_driver);
    ir_keypad_driver.type = LV_INDEV_TYPE_KEYPAD;
    ir_keypad_driver.read_cb = ir_keypad_read;
    lv_indev_drv_register(&ir_keypad_driver);

    /* adc keypad */
    lv_indev_drv_init(&adc_keypad_driver);
    adc_keypad_driver.type = LV_INDEV_TYPE_KEYPAD;
    adc_keypad_driver.read_cb = adc_keypad_read;
    lv_indev_drv_register(&adc_keypad_driver);
}
