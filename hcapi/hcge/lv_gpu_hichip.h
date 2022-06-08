/**
 * @file lv_gpu_hichip.h
 *
 */

#ifndef LV_GPU_HICHIP_H
#define LV_GPU_HICHIP_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/src/misc/lv_color.h"
#include "lvgl/src/hal/lv_hal_disp.h"
#include "lvgl/src/draw/sw/lv_draw_sw.h"
#include <hcge/ge_api.h>

#if LV_USE_GPU_HICHIP

/*********************
 *      DEFINES
 *********************/

#define LV_DMA2D_ARGB8888 0
#define LV_DMA2D_RGB888 1
#define LV_DMA2D_RGB565 2
#define LV_DMA2D_ARGB1555 3
#define LV_DMA2D_ARGB4444 4

/**********************
 *      TYPEDEFS
 **********************/
typedef lv_draw_sw_ctx_t lv_draw_hichip_ctx_t;

struct _lv_disp_drv_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Turn on the peripheral and set output color mode, this only needs to be done once
 */
void lv_draw_hichip_init(void);

void lv_draw_hichip_ctx_init(struct _lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx);

void lv_draw_hichip_ctx_deinit(struct _lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx);

void lv_draw_hichip_blend(lv_draw_ctx_t * draw_ctx, const lv_draw_sw_blend_dsc_t * dsc);

void lv_draw_hichip_buffer_copy(lv_draw_ctx_t * draw_ctx,
                                     void * dest_buf, lv_coord_t dest_stride, const lv_area_t * dest_area,
                                     void * src_buf, lv_coord_t src_stride, const lv_area_t * src_area);

void lv_gpu_hichip_wait_cb(lv_draw_ctx_t * draw_ctx);


void lv_draw_hichip_lock(void);
void lv_draw_hichip_unlock(void);

extern hcge_context *hcge_ctx;

/**********************
 *      MACROS
 **********************/

#endif  /*LV_USE_GPU_HICHIP*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GPU_HICHIP_H*/
