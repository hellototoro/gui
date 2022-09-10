/*
gpio_ctrl.c, get/set the GPIO value
 */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <stdbool.h> //bool
#include <dirent.h>

#include "gpio_ctrl.h"
#include "com_api.h"

typedef struct{
	int number;
	int direction;
}gpio_handle_t;

static int file_exist(const char *path)
{
	DIR *dirp;
    if ((dirp = opendir(path)) == NULL){
        return 0;
    }else{
        closedir(dirp);
        return 1;
    }
}

void *gpio_open(gpio_info_t *gpio_info)
{
	int fd = -1;
	gpio_handle_t *gpio_handle = NULL;
	char str_buf[64];

	sprintf(str_buf, "/sys/class/gpio/gpio%d", gpio_info->number);
	//if /sys/class/gpio/gpio8 is exist, not need create again.
	if (!file_exist(str_buf)){

		fd = open("/sys/class/gpio/export", O_WRONLY);
		if (fd < 0){
			printf("%s(), line:%d. open export fail!, fd:%d\n", __func__, __LINE__, fd);
			return NULL;
		}
		sprintf(str_buf, "%d", gpio_info->number);
		write(fd, str_buf, strlen(str_buf));
		close(fd);
		api_sleep_ms(10);
	}

	// echo in > /sys/class/gpio/gpio8/direction
	sprintf(str_buf, "/sys/class/gpio/gpio%d/direction", gpio_info->number);
	fd = open(str_buf, O_WRONLY);
	if (fd < 0){
		printf("%s(), line:%d. open %s fail!\n", __func__, __LINE__, str_buf);
		return NULL;
	}
	if (GPIO_DIR_IN == gpio_info->direction)
		sprintf(str_buf, "in");
	else
		sprintf(str_buf, "out");

	write(fd, str_buf, strlen(str_buf));
	close(fd);


	gpio_handle = (gpio_handle_t*)malloc(sizeof(gpio_handle_t));
	memset(gpio_handle, 0, sizeof(gpio_handle_t));
	gpio_handle->number = gpio_info->number;
	gpio_handle->direction = gpio_info->direction;

	return (void*)gpio_handle;
}

int gpio_close(void *gpio_handle)
{
	gpio_handle_t *handle = (gpio_handle_t*)gpio_handle;
	char str_buf[64] = {0};

	if (!handle)
		return API_FAILURE;

	//"echo 8 > /sys/class/gpio/unexport"
	int fd = open("/sys/class/gpio/unexport", O_WRONLY);
	if (fd < 0){
		printf("%s(), line:%d. open unexport fail!\n", __func__, __LINE__);
		return API_FAILURE;	
	}
	sprintf(str_buf, "%d", handle->number);
	write(fd, str_buf, strlen(str_buf));
	close(fd);
	free(gpio_handle);

	return API_SUCCESS;	
}


int gpio_read(void *gpio_handle)
{
	char str_buf[64];
	int ret;
	gpio_handle_t *handle = (gpio_handle_t*)gpio_handle;
	int fd = -1;
	int value = -1;

	if (!handle)
		return API_FAILURE;

	char buffer[16]={0};


	//"cat /sys/class/gpio/gpio8/value"
	sprintf(str_buf, "/sys/class/gpio/gpio%d/value", handle->number);
	fd = open(str_buf, O_RDWR);

	if (fd < 0){
		printf("%s(), line:%d. open %s fail!\n", __func__, __LINE__, str_buf);
		return value;
	}

	ret = read(fd, buffer, sizeof(buffer));
	if (ret > 0){
		value = atoi(buffer);
	}
	close(fd);
	//printf("ret=%d, value:%d, buffer:%s\n", ret, value, buffer);
	
	return value;
}


int gpio_write(void *gpio_handle, bool high)
{
	char str_buf[64];
	gpio_handle_t *handle = (gpio_handle_t*)gpio_handle;
	int fd = -1;

	sprintf(str_buf, "/sys/class/gpio/gpio%d/value", handle->number);
	fd = open(str_buf, O_WRONLY);
	if (fd < 0){
		printf("%s(), line:%d. open %s fail!\n", __func__, __LINE__, str_buf);
		return API_FAILURE;
	}

	if (high)
		sprintf(str_buf, "1");
	else
		sprintf(str_buf, "0");

	write(fd, str_buf, strlen(str_buf));
	close(fd);
	return API_SUCCESS;
}
