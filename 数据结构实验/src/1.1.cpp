
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#define OK 1
#define ERROR 0
#define TRUE 1
#define FALSE 0
#define INFEASIBLE -1
#define OVERFLOW -2
#define MAXSIZE 100

typedef int ElemType;  //声明ElemType的类型
typedef int Status;  //声明函数类型名
typedef struct SqList {
        ElemType *elem;  //顺序表数据存储空间基址
        int length;
}SqList;// LIST为用户定义的线性表类型

//1
Status InitList(SqList &L)
{
    L.elem = new ElemType[MAXSIZE];
    if (!L.elem)
        return OVERFLOW;
    L.length = 0;
    return OK;
}

Status ClearList(SqList &L)
{
    L.length = 0;
    return OK;
}

int Listlength(SqList L)
{
    return L.length;
}

Status GetElem(SqList L, int i, ElemType &e)
{
    if (i < 1 || i > L.length)
        return ERROR;
    e = L.elem[i - 1];
    return OK;
}

int LocateList(SqList L, ElemType e)
{
    int i;
    for (i = 0; i < L.length; i++)
        if (L.elem[i] == e)
            return i + 1;
    return 0;
}

Status ListInsert(SqList &L, int i, ElemType e)
{
    int j;
    if (i < 1 || i > L.length + 1)
        return ERROR;
    if (L.length >= MAXSIZE)
        return ERROR;
    for (j = L.length - 1; j >= i - 1; j--)
        L.elem[j + 1] = L.elem[j];
    L.elem[i - 1] = e;
    L.length++;
    return OK;
}

Status ListDelete(SqList &L, int i, ElemType &e)
{
    int j;
    if (i < 1 || i > L.length)
        return ERROR;
    e = L.elem[i - 1];
    for (j = i; j < L.length; j++)
        L.elem[j - 1] = L.elem[j];
    L.length--;
    return OK;
}

Status ListTraverse(SqList L)
{
    int i;
    for (i = 0; i < L.length; i++)
        printf("%d ", L.elem[i]);
    printf("\n");
    return OK;
}

//2
Status Union(SqList &La, SqList Lb)
{
    ElemType e;
    int i;
    for (i = 1; i <= Listlength(Lb); i++) {
        GetElem(Lb, i, e);
        if (LocateList(La, e) == 0)
            ListInsert(La, Listlength(La) + 1, e);
    }
    return OK;
}

Status Intersection(SqList &La, SqList Lb)
{
    ElemType e;
    int i = 1;
    while (i <= Listlength(La)) {
        GetElem(La, i, e);
        if (LocateList(Lb, e) == 0)
            ListDelete(La, i, e);
        else
            i++;
    }
    return OK;
}

Status Difference(SqList &La, SqList Lb)
{
    ElemType e;
    int i = 1;
    while (i <= Listlength(La)) {
        GetElem(La, i, e);
        if (LocateList(Lb, e) != 0)
            ListDelete(La, i, e);
        else
            i++;
    }
    return OK;
}

Status CreateList(SqList &L, ElemType a[], int n)
{
    int i;
    ClearList(L);
    for (i = 0; i < n; i++)
        ListInsert(L, Listlength(L) + 1, a[i]);
    return OK;
}

Status MergeList(SqList La, SqList Lb, SqList &Lc)
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

Status Purge(SqList &Lc)
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

//3
int main()
{
    SqList LA, LB, LC;
    ElemType e;
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

    delete[] LA.elem;
    delete[] LB.elem;
    delete[] LC.elem;
    return 0;
}
