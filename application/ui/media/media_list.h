/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-05 14:32:14
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-09-11 19:14:46
 * @FilePath: /gui/application/ui/media/media_list.h
 * @Description: None
 * @other: None
 */
#ifndef __GUI_QUEUE_H__
#define __GUI_QUEUE_H__

#include <stdbool.h>

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
    int len;
    NodePtr head;
} LinkList;

typedef struct DNode {
    ElemType data;
    struct DNode *pre, *next;
}DNode, *DNodePtr;

typedef DNode DLNode;
typedef struct {
    int len;
    DNodePtr head;
} LinkDList;

typedef struct {
    int len;
    NodePtr front,rear;
} LinkQueue;

typedef Node StackNode;
typedef Node* LinkStack;

/* list */
Status InitList(LinkList *L);
Status ListAppend(LinkList *L, ElemType e);
LNode* ListHead(LinkList *L);
Status DestroyList(LinkList *L);

/* double list */
Status InitDList(LinkDList *L);
Status DListAppend(LinkDList *L, ElemType e);
DLNode* DListHead(LinkDList *L);
Status DestroyDList(LinkDList *L);

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


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
