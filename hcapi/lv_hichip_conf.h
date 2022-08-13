#ifndef _LV_HICHIP_CONF_H
#define _LV_HICHIP_CONF_H

/* hichip ge */
#define LV_USE_GPU_HICHIP 1

//enable dual framebuffer
#define LV_USE_DUAL_FRAMEBUFFER 1

//in pixel
#define LV_HC_SCREEN_HOR_RES (1280)
#define LV_HC_SCREEN_VER_RES (720)


//lvgl draw buffer size, in pixel
//lvgl sugest at least 10 * 
#define LV_HC_DRAW_BUF_SIZE (LV_HC_SCREEN_HOR_RES * LV_HC_SCREEN_VER_RES)

//only support 1 or 2
#define LV_HC_DRAW_BUF_COUNT (2)



#endif
