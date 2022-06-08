/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-05 14:32:14
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-06-07 13:39:24
 * @FilePath: /gui/application/ui/data_struct.h
 * @Description: None
 * @other: None
 */
#ifndef __GUI_QUEUE_H__
#define __GUI_QUEUE_H__

#include <stdbool.h>
#include "FileType.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int Status;
#define OK      1
#define ERROR   0

typedef void* ElemType;

typedef struct Node {
    ElemType data;
    struct Node *next;
}Node, *NodePtr;

typedef Node LNode;
typedef struct {
    NodePtr head;
    int len;
} LinkList;

typedef struct {
    NodePtr front,rear;
    int len;
} LinkQueue;

typedef Node StackNode;
typedef Node* LinkStack;

/* list */
Status InitList(LinkList *L);
Status ListAppend(LinkList *L, ElemType e);
LNode* ListHead(LinkList *L);
Status DestroyList(LinkList *L);

/* Queue */
Status InitQueue(LinkQueue *Q);
Status EnQueue(LinkQueue *Q, ElemType e);
Status DeQueue(LinkQueue *Q, ElemType *e);
bool QueueIsEmpty(LinkQueue Q);
void DestroyQueue(LinkQueue *Q);

/* stack */
Status InitStack(LinkStack *S);
Status Push(LinkStack *S, ElemType e);
Status Pop(LinkStack *S, ElemType *e);
bool StackIsEmpty(LinkStack S);
Status DestroyStack(LinkStack *S);


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
