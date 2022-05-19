/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-05-19 15:18:33
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-05-19 16:28:24
 * @FilePath: /gui/application/init.c
 * @Description: None
 * @other: None
 */
#include <stdio.h>
#include "init.h"
#include "key/key.h"
#include "lvgl/lvgl.h"
#include "lv_drivers/display/fbdev.h"
#include "application/ui/ui.h"

#define HOR_RES         1280
#define VER_RES         720
#define DISP_BUF_SIZE   (HOR_RES * 1024)//(128 * 1024)

/**
 * @brief: 
 * @retval: 
 */
void app_init(void)
{
    lv_disp_t *dsp = NULL;

    /*LVGL init*/
    lv_init();

    /*Linux frame buffer device init*/
    fbdev_init();
    
   /*A small buffer for LittlevGL to draw the screen's content*/
    static lv_color_t buf[DISP_BUF_SIZE];

    /*Initialize a descriptor for the buffer*/
    static lv_disp_draw_buf_t disp_buf;
    lv_disp_draw_buf_init(&disp_buf, buf, NULL, DISP_BUF_SIZE);

    /*Initialize and register a display driver*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf   = &disp_buf;
    disp_drv.flush_cb   = fbdev_flush;
    disp_drv.hor_res    = HOR_RES;
    disp_drv.ver_res    = VER_RES;
    dsp = lv_disp_drv_register(&disp_drv);
    if (!dsp) {
        printf("lv_disp_drv_register error!\n");
        return;
    }
    printf("init lv_disp_register: w-%d,h-%d\n",disp_drv.hor_res,disp_drv.ver_res);

    ui_init();
    key_init();
}
