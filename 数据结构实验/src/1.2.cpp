#include <stdio.h>
#include <stdlib.h>
#define OK 1
#define ERROR 0
#define TRUE 1
#define FALSE 0
#define INFEASIBLE -1
#define OVERFLOW -2
typedef int ElemType;
typedef int Status;
typedef struct LNode {
    ElemType data;
    struct LNode *next;
} LNode, *LinkList;
Status InitList(LinkList &L)
{
    L = (LinkList)malloc(sizeof(LNode));
    if (!L)
        return OVERFLOW;
    L->next = NULL;
    return OK;
}
Status ClearList(LinkList &L)
{
    LNode *p = L->next, *q;
    while (p) {
        q = p->next;
        free(p);
        p = q;
    }
    L->next = NULL;
    return OK;
}
int Listlength(LinkList L)
{
    int n = 0;
    LNode *p = L->next;
    while (p) {
        n++;
        p = p->next;
    }
    return n;
}
Status GetElem(LinkList L, int i, ElemType &e)
{
    LNode *p = L->next;
    int j = 1;
    while (p && j < i) {
        p = p->next;
        j++;
    }
    if (!p || j > i)
        return ERROR;
    e = p->data;
    return OK;
}
LNode *LocateList(LinkList L, ElemType e)
{
    LNode *p = L->next;
    while (p && p->data != e)
        p = p->next;
    return p;
}
Status ListInsert(LinkList &L, int i, ElemType e)
{
    LNode *p = L;
    int j = 0;
    while (p && j < i - 1) {
        p = p->next;
        j++;
    }
    if (!p || j > i - 1)
        return ERROR;
    LNode *s = (LNode *)malloc(sizeof(LNode));
    if (!s)
        return OVERFLOW;
    s->data = e;
    s->next = p->next;
    p->next = s;
    return OK;
}
Status ListDelete(LinkList &L, int i, ElemType &e)
{
    LNode *p = L, *q;
    int j = 0;
    while (p->next && j < i - 1) {
        p = p->next;
        j++;
    }
    if (!(p->next) || j > i - 1)
        return ERROR;
    q = p->next;
    p->next = q->next;
    e = q->data;
    free(q);
    return OK;
}
Status ListTraverse(LinkList L)
{
    LNode *p = L->next;
    while (p) {
        printf("%d → ", p->data);
        p = p->next;
    }
    printf("∧\n");
    return OK;
}
Status Union(LinkList &La, LinkList Lb)
{
    ElemType e;
    int i;
    for (i = 1; i <= Listlength(Lb); i++) {
        GetElem(Lb, i, e);
        if (LocateList(La, e) == NULL)
            ListInsert(La, Listlength(La) + 1, e);
    }
    return OK;
}
Status Intersection(LinkList &La, LinkList Lb)
{
    ElemType e;
    int i = 1;
    while (i <= Listlength(La)) {
        GetElem(La, i, e);
        if (LocateList(Lb, e) == NULL)
            ListDelete(La, i, e);
        else
            i++;
    }
    return OK;
}
Status Difference(LinkList &La, LinkList Lb)
{
    ElemType e;
    int i = 1;
    while (i <= Listlength(La)) {
        GetElem(La, i, e);
        if (LocateList(Lb, e) != NULL)
            ListDelete(La, i, e);
        else
            i++;
    }
    return OK;
}
Status CreateList(LinkList &L, ElemType a[], int n)
{
    int i;
    ClearList(L);
    for (i = 0; i < n; i++)
        ListInsert(L, Listlength(L) + 1, a[i]);
    return OK;
}
Status MergeList(LinkList La, LinkList Lb, LinkList &Lc)
{
    ElemType ea, eb;
    int i = 1, j = 1;
    InitList(Lc);
    while (i <= Listlength(La) && j <= Listlength(Lb)) {
        GetElem(La, i, ea);
        GetElem(Lb, j, eb);
        if (ea <= eb) {
            ListInsert(Lc, Listlength(Lc) + 1, ea);
            i++;
        } else {
            ListInsert(Lc, Listlength(Lc) + 1, eb);
            j++;
        }
    }
    while (i <= Listlength(La)) {
        GetElem(La, i, ea);
        ListInsert(Lc, Listlength(Lc) + 1, ea);
        i++;
    }
    while (j <= Listlength(Lb)) {
        GetElem(Lb, j, eb);
        ListInsert(Lc, Listlength(Lc) + 1, eb);
        j++;
    }
    return OK;
}
Status Purge(LinkList &Lc)
{
    ElemType ei, e;
    int i = 1, j;
    while (i <= Listlength(Lc)) {
        GetElem(Lc, i, ei);
        j = i + 1;
        while (j <= Listlength(Lc)) {
            GetElem(Lc, j, e);
            if (e == ei)
                ListDelete(Lc, j, e);
            else
                j++;
        }
        i++;
    }
    return OK;
}
int main()
{
    LinkList LA, LB, LC;
    ElemType a[] = {2, 8, 27, 39, 66, 77, 89};
    ElemType b[] = {6, 18, 27, 59, 65, 77, 89, 120, 140};
    int na = 7, nb = 9;
    InitList(LA);
    InitList(LB);
    CreateList(LA, a, na);
    CreateList(LB, b, nb);
    printf("初始：\nLA = ");
    ListTraverse(LA);
    printf("LB = ");
    ListTraverse(LB);
    Union(LA, LB);
    printf("\n并运算 La=La U Lb：\nLA = ");
    ListTraverse(LA);
    CreateList(LA, a, na);
    Intersection(LA, LB);
    printf("\n交运算 La=La ∩ Lb：\nLA = ");
    ListTraverse(LA);
    CreateList(LA, a, na);
    Difference(LA, LB);
    printf("\n差运算 La=La - Lb：\nLA = ");
    ListTraverse(LA);
    CreateList(LA, a, na);
    MergeList(LA, LB, LC);
    printf("\n有序合并 Lc=La+Lb：\nLC = ");
    ListTraverse(LC);
    Purge(LC);
    printf("\n去掉重复元素后：\nLC = ");
    ListTraverse(LC);
    ClearList(LA); free(LA);
    ClearList(LB); free(LB);
    ClearList(LC); free(LC);
    return 0;
}
