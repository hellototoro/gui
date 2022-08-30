#ifndef __GPIO_CTRL_H__
#define __GPIO_CTRL_H__

#include <hcuapi/pinpad.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef enum{
	GPIO_DIR_OUT,
	GPIO_DIR_IN,
}gpio_dir_e;

typedef struct{
	int number;
	int direction;
}gpio_info_t;


void *gpio_open(gpio_info_t *gpio_info);
int gpio_close(void *gpio_handle);
int gpio_read(void *gpio_handle);
int gpio_write(void *gpio_handle, bool high);

#ifdef __cplusplus
} /*extern "C"*/
#endif


#endif //end of __GPIO_CTRL_H__
