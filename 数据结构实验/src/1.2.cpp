#include <stdio.h>
#include <stdlib.h>

#define OK         1
#define ERROR      0
#define TRUE       1
#define FALSE      0
#define INFEASIBLE -1
#define OVERFLOW   -2

typedef int ElemType;       /* 数据元素类型 */
typedef int Status;         /* 函数返回值类型名 */

/* 1. 单链表存储结构的定义（带头结点） */
typedef struct LNode {      /* 结点类型定义 */
    ElemType data;
    struct LNode *next;     /* 注意这里要有分号 */
} LNode, *LinkList;

/* ---------- 2. 单链表基本操作 ---------- */

/* (1) 初始化：申请头结点，next 置空，得到空表。你文件里这段已经写对了 */
Status InitList(LinkList &L)
{
    L = (LinkList)malloc(sizeof(LNode));
    if (!L)
        return OVERFLOW;
    L->next = NULL;
    return OK;
}

/* (2) 清空：真正逐个 free 数据结点（对比顺序表只需 length=0），头结点保留 */
Status ClearList(LinkList &L)
{
    LNode *p = L->next, *q;
    while (p) {
        q = p->next;           /* 先记下后继再释放，否则断链 */
        free(p);
        p = q;
    }
    L->next = NULL;            /* 头结点回到初始化状态 */
    return OK;
}

/* (3) 求表长：数据结点个数，从头结点的 next 数起 */
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

/* (4) 取第 i 个数据结点的值：p 从第 1 个数据结点出发走 i-1 步 */
Status GetElem(LinkList L, int i, ElemType &e)
{
    LNode *p = L->next;        /* p 指向第 1 个数据结点 */
    int j = 1;                 /* j 是 p 当前的位序 */
    while (p && j < i) {       /* 还没走到第 i 个且链没断 */
        p = p->next;
        j++;
    }
    if (!p || j > i)           /* i 越界（>length 走到 NULL；i<1 则 j 反超） */
        return ERROR;
    e = p->data;
    return OK;
}

/* (5) 查找：返回第一个等于 e 的结点地址，找不到返回 NULL */
LNode *LocateList(LinkList L, ElemType e)
{
    LNode *p = L->next;
    while (p && p->data != e)  /* 边走边比，一步到位 */
        p = p->next;
    return p;                  /* 全程约定：NULL = 不存在 */
}

/* (6) 在第 i 个数据结点之前插入 e：先找到第 i-1 个结点 p，新结点插它后面 */
Status ListInsert(LinkList &L, int i, ElemType e)
{
    LNode *p = L;              /* 注意：这里从"头结点"出发，因为允许 i=1 */
    int j = 0;                 /* 头结点算第 0 个 */
    while (p && j < i - 1) {
        p = p->next;
        j++;
    }
    if (!p || j > i - 1)       /* 1<=i<=length+1 之外都拒绝 */
        return ERROR;
    LNode *s = (LNode *)malloc(sizeof(LNode));
    if (!s)
        return OVERFLOW;
    s->data = e;
    s->next = p->next;         /* 两步接线，顺序不能反：先接右边再接左边 */
    p->next = s;
    return OK;
}

/* (7) 删除第 i 个数据结点：找第 i-1 个结点 p，摘掉 p->next */
Status ListDelete(LinkList &L, int i, ElemType &e)
{
    LNode *p = L, *q;
    int j = 0;
    while (p->next && j < i - 1) {  /* 保证 p->next 存在才有得删 */
        p = p->next;
        j++;
    }
    if (!(p->next) || j > i - 1)    /* i 越界：>length 或 <1 */
        return ERROR;
    q = p->next;               /* q 是要删的第 i 个结点 */
    p->next = q->next;         /* 先摘链 */
    e = q->data;               /* 带回被删值 */
    free(q);                   /* 再释放 */
    return OK;
}

/* (8) 遍历：按题目要求的 "2→8→…→89→∧" 格式输出 */
Status ListTraverse(LinkList L)
{
    LNode *p = L->next;
    while (p) {
        printf("%d → ", p->data);
        p = p->next;
    }
    printf("∧\n");             /* 空表也会输出一个 ∧ */
    return OK;
}

/* ---------- 3.1 应用案例1：集合运算（逻辑与顺序表版完全一致） ---------- */

/* (1) 并 La = La ∪ Lb：Lb 中 La 没有的元素接到 La 表尾 */
Status Union(LinkList &La, LinkList Lb)
{
    ElemType e;
    int i;
    for (i = 1; i <= Listlength(Lb); i++) {
        GetElem(Lb, i, e);
        if (LocateList(La, e) == NULL)     /* 返回指针，判 NULL */
            ListInsert(La, Listlength(La) + 1, e);
    }
    return OK;
}

/* (2) 交 La = La ∩ Lb：La 中不在 Lb 的删掉；删后 i 不动 */
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

/* (3) 差 La = La - Lb：条件与交相反；删后 i 同样不动 */
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

/* 用数组尾插法建表/恢复现场（对应 1.1 的 CreateList） */
Status CreateList(LinkList &L, ElemType a[], int n)
{
    int i;
    ClearList(L);
    for (i = 0; i < n; i++)
        ListInsert(L, Listlength(L) + 1, a[i]);
    return OK;
}

/* ---------- 3.2 应用案例2：有序表合并 Lc = La + Lb ---------- */
Status MergeList(LinkList La, LinkList Lb, LinkList &Lc)
{
    ElemType ea, eb;
    int i = 1, j = 1;                      /* i、j 是位序，从 1 起 */

    InitList(Lc);                          /* 建带头结点的空表 Lc */
    while (i <= Listlength(La) && j <= Listlength(Lb)) {
        GetElem(La, i, ea);
        GetElem(Lb, j, eb);
        if (ea <= eb) {                    /* 相等取 La 的，保证稳定 */
            ListInsert(Lc, Listlength(Lc) + 1, ea);
            i++;
        } else {
            ListInsert(Lc, Listlength(Lc) + 1, eb);
            j++;
        }
    }
    while (i <= Listlength(La)) {          /* La 剩余整段接上 */
        GetElem(La, i, ea);
        ListInsert(Lc, Listlength(Lc) + 1, ea);
        i++;
    }
    while (j <= Listlength(Lb)) {          /* Lb 剩余整段接上 */
        GetElem(Lb, j, eb);
        ListInsert(Lc, Listlength(Lc) + 1, eb);
        j++;
    }
    return OK;
}

/* ---------- 3.3 应用案例3：去掉重复多余元素 ---------- */
Status Purge(LinkList &Lc)
{
    ElemType ei, e;
    int i = 1, j;
    while (i <= Listlength(Lc)) {
        GetElem(Lc, i, ei);                /* 第 i 个是保留基准 */
        j = i + 1;
        while (j <= Listlength(Lc)) {
            GetElem(Lc, j, e);
            if (e == ei)
                ListDelete(Lc, j, e);      /* 删后 j 不动 */
            else
                j++;
        }
        i++;
    }
    return OK;
}

/* ---------- 4. 主函数 ---------- */
int main()
{
    LinkList LA, LB, LC;
    ElemType a[] = {2, 8, 27, 39, 66, 77, 89};
    ElemType b[] = {6, 18, 27, 59, 65, 77, 89, 120, 140};
    int na = 7, nb = 9;

    InitList(LA);                          /* 先各自有头结点，CreateList 才能清空 */
    InitList(LB);

    /* (1) 数据准备 */
    CreateList(LA, a, na);
    CreateList(LB, b, nb);
    printf("初始：\nLA = ");
    ListTraverse(LA);
    printf("LB = ");
    ListTraverse(LB);

    /* (2) 并、交、差：每次调用前先恢复 LA */
    Union(LA, LB);
    printf("\n并运算 La=La U Lb：\nLA = ");
    ListTraverse(LA);

    CreateList(LA, a, na);                 /* 恢复 LA */
    Intersection(LA, LB);
    printf("\n交运算 La=La ∩ Lb：\nLA = ");
    ListTraverse(LA);

    CreateList(LA, a, na);                 /* 恢复 LA */
    Difference(LA, LB);
    printf("\n差运算 La=La - Lb：\nLA = ");
    ListTraverse(LA);

    /* (3) 有序合并 */
    CreateList(LA, a, na);                 /* 恢复 LA */
    MergeList(LA, LB, LC);
    printf("\n有序合并 Lc=La+Lb：\nLC = ");
    ListTraverse(LC);

    /* (4) 去重 */
    Purge(LC);
    printf("\n去掉重复元素后：\nLC = ");
    ListTraverse(LC);

    ClearList(LA); free(LA);               /* 释放所有结点，包括头结点 */
    ClearList(LB); free(LB);
    ClearList(LC); free(LC);
    return 0;
}
