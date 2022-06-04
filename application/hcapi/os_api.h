/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-05-19 16:06:36
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-05-19 16:10:12
 * @FilePath: /gui/application/os_api.h
 * @Description: None
 * @other: None
 */
#ifndef __OS_API_H__
#define __OS_API_H__

#ifdef __cplusplus
extern "C" {
#endif

#define INVALID_ID  0xFFFFFFFF

uint32_t api_message_create(int msg_count, int msg_length);
int api_message_delete(uint32_t msg_id);
int api_message_send(uint32_t msg_id, void *msg, int length);
int api_message_receive(uint32_t msg_id, void *msg, int length);

int api_message_receive_tm(uint32_t msg_id, void *msg, int length, int ms);

#ifdef __cplusplus
}
#endif


#endif