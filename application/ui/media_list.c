/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-05 14:32:07
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-06-22 19:45:11
 * @FilePath: /gui/application/ui/media_list.c
 * @Description: None
 * @other: None
 */
#include <stdlib.h>
#include "media_list.h"


/* list */
Status InitList(LinkList *L)
{
    L->head = (LNode *) malloc(sizeof (LNode));
    L->head->next = NULL;
    L->len = 0;
    return OK;
}

Status ListAppend(LinkList *L, ElemType e)
{
    Node *p = L->head;
    while (p->next) {
        p = p->next;
    }
    LNode* s = (LNode *) malloc(sizeof (LNode));
    s->data = e;
    s->next = NULL;
    p->next = s;
    L->len++;
    return OK;
}

LNode* ListHead(LinkList *L)
{
    return L->head;
}

Status DestroyList(LinkList *L)
{
    LNode *p = L->head;
    while(p) {
        LNode* q = p;
        p = p->next;
        free(q);
    }
    free(L);
    L = NULL;
    return OK;
}

/* double list */
Status InitDList(LinkDList *L)
{
    L->head = (DLNode *) malloc(sizeof (DLNode));
    L->head->data = NULL;
    L->head->next = L->head->pre = L->head;
    L->len = 0;
    return OK;
}

Status DListAppend(LinkDList *L, ElemType e)
{
    DLNode *p = L->head;
    DLNode* s = (DLNode *) malloc(sizeof (DLNode));
    s->data = e;
    s->pre = p->pre;
    s->next = p;
    p->pre->next = s;
    p->pre = s;
    L->len++;
    return OK;
}

DLNode* DListHead(LinkDList *L)
{
    return L->head;
}

Status DestroyDList(LinkDList *L)
{
    DLNode *head = L->head;
    DLNode *p = head;
    do {
        DLNode* q = p;
        p = p->next;
        free(q);
    } while(p != head);
    free(L);
    //L = NULL;
    return OK;
}

/* queue */
Status InitQueue(LinkQueue *Q)
{
    Q->front = Q->rear = (Node *)malloc(sizeof (Node));
    Q->front->next = NULL;
    Q->len = 0;
    return OK;
}

Status EnQueue(LinkQueue *Q, ElemType e)
{
    Node *p = (Node *)malloc(sizeof (Node));
    p->data = e;
    p->next = NULL;
    Q->rear->next = p;
    Q->rear = p;
    Q->len++;
    return OK;
}

Status DeQueue(LinkQueue *Q, ElemType *e)
{
    if (Q->rear == Q->front) return ERROR;
    Node *p = Q->front->next;
    *e = p->data;
    Q->front->next = p->next;
    if (Q->rear == p) Q->rear = Q->front;
    Q->len--;
    free(p);
    return OK;
}

bool QueueIsEmpty(LinkQueue Q)
{
    return (Q.front == Q.rear) ? true : false;
}

void DestroyQueue(LinkQueue *Q)
{
    ElemType dummy;
    while ( !QueueIsEmpty(*Q) ) {
        DeQueue(Q, &dummy);
        free(dummy);
    }
}

/* stack */
Status InitStack(LinkStack *S)
{
    *S = NULL;
    return OK;
}

Status Push(LinkStack *S, ElemType e)
{
    StackNode *p = (StackNode *)malloc(sizeof (StackNode));
    p->data = e;
    p->next = *S;
    *S = p;
    return OK;
}

Status Pop(LinkStack *S, ElemType *e)
{
    if (*S == NULL) return ERROR;
    *e = (*S)->data;
    StackNode *p = *S;
    *S = (*S)->next;
    free(p);
    return OK;
}

bool StackIsEmpty(LinkStack S)
{
    return (S == NULL) ? true : false;
}

Status DestroyStack(LinkStack *S)
{
    ElemType dummy;
    while ( !StackIsEmpty(*S) ) {
        Pop(S, &dummy);
        free(dummy);
    }
    return OK;
}
