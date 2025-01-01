/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-06-05 14:32:07
 * @LastEditors: totoro huangjian921@outlook.com
 * @LastEditTime: 2022-09-11 19:14:43
 * @FilePath: /gui/application/ui/media/media_list.c
 * @Description: None
 * @other: None
 */
#include <stdlib.h>
#include "media_list.h"

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
