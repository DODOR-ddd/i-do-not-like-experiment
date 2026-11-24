/*
 * 实验1.3 顺序栈与循环队列（C++ 引用版）
 * 编译并运行：make run-1.3
 */
#include <stdio.h>
#include <stdlib.h>

#define OK 1
#define ERROR 0
#define TRUE 1
#define FALSE 0
#define INFEASIBLE -1
#define OVERFLOW -2

typedef int Status;             /* 函数类型名 */

#define STACK_INIT_SIZE 10      /* 顺序栈初始大小 */
#define STACKINCREMENT  5       /* 顺序栈扩容增量 */

typedef int SElemType;          /* 栈元素类型 */

typedef struct {
    SElemType *base;            /* 栈底指针（存储空间基址） */
    SElemType *top;             /* 栈顶指针，指向栈顶元素的上一格 */
    int stacksize;              /* 当前已分配的容量 */
} SqStack;

#define MAXQSIZE 10             /* 队列的最大长度（实际可存 MAXQSIZE-1 个） */

typedef int QElemType;          /* 队列元素类型 */

typedef struct {
    QElemType *base;            /* 动态分配存储空间的地址（数组名） */
    int front;                  /* 头元素的下标 */
    int rear;                   /* 尾元素的下一格下标 */
} SqQueue;

/* ==================== 2. 顺序栈基本操作 ==================== */

/* (1) 构造空栈：malloc 申请 STACK_INIT_SIZE 大小空间 */
Status InitStack(SqStack &S)
{
    S.base = (SElemType *)malloc(STACK_INIT_SIZE * sizeof(SElemType));
    if (!S.base)
        return OVERFLOW;
    S.top = S.base;             /* top==base 表示空栈 */
    S.stacksize = STACK_INIT_SIZE;
    return OK;
}

/* (2) 清空：只需 top 回到 base */
Status ClearStack(SqStack &S)
{
    S.top = S.base;
    return OK;
}

/* (3) 返回栈中元素个数：top-base 即元素个数 */
int StackLength(SqStack S)
{
    return S.top - S.base;
}

/* (4) 用 e 返回栈顶元素的值（不出栈） */
Status GetTop(SqStack S, SElemType &e)
{
    if (S.top == S.base)        /* 空栈无栈顶 */
        return ERROR;
    e = *(S.top - 1);
    return OK;
}

/* (5) 进栈：栈满时按 STACKINCREMENT 扩容 */
Status Push(SqStack &S, SElemType e)
{
    SElemType *newbase;
    if (S.top - S.base >= S.stacksize) {        /* 栈满，追加空间 */
        newbase = (SElemType *)realloc(S.base,
                 (S.stacksize + STACKINCREMENT) * sizeof(SElemType));
        if (!newbase)
            return OVERFLOW;
        S.base = newbase;
        S.top = S.base + S.stacksize;           /* realloc 可能搬移内存，top 要重算 */
        S.stacksize += STACKINCREMENT;
    }
    *S.top++ = e;               /* 先存元素，top 再上移 */
    return OK;
}

/* (6) 出栈：用 e 返回栈顶元素值 */
Status Pop(SqStack &S, SElemType &e)
{
    if (S.top == S.base)        /* 空栈不能出 */
        return ERROR;
    e = *--S.top;               /* top 先下移，再取元素 */
    return OK;
}

/* ==================== 3. 循环队列基本操作 ==================== */

/* (1) 构造空队列：malloc 申请 MAXQSIZE 大小空间 */
Status InitQueue(SqQueue &Q)
{
    Q.base = (QElemType *)malloc(MAXQSIZE * sizeof(QElemType));
    if (!Q.base)
        return OVERFLOW;
    Q.front = Q.rear = 0;
    return OK;
}

/* (2) 清空：front、rear 归零 */
Status ClearQueue(SqQueue &Q)
{
    Q.front = Q.rear = 0;
    return OK;
}

/* (3) 返回队列长度：下标差对 MAXQSIZE 取模修正 */
int QueueLength(SqQueue Q)
{
    return (Q.rear - Q.front + MAXQSIZE) % MAXQSIZE;
}

/* (6) 判队空：front==rear 即空 */
Status QueueEmpty(SqQueue Q)
{
    return Q.front == Q.rear ? TRUE : FALSE;
}

/* (7) 判队满：牺牲一格空间，(rear+1)%MAXQSIZE==front 即满 */
Status QueueFull(SqQueue Q)
{
    return (Q.rear + 1) % MAXQSIZE == Q.front ? TRUE : FALSE;
}

/* (4) 入队：队满返回 ERROR */
Status EnQueue(SqQueue &Q, QElemType e)
{
    if (QueueFull(Q))
        return ERROR;
    Q.base[Q.rear] = e;
    Q.rear = (Q.rear + 1) % MAXQSIZE;   /* 下标循环前进 */
    return OK;
}

/* (5) 出队：用 e 返回其值（题目此处签名误写为 SqStack，应为 SqQueue） */
Status DeQueue(SqQueue &Q, QElemType &e)
{
    if (QueueEmpty(Q))
        return ERROR;
    e = Q.base[Q.front];
    Q.front = (Q.front + 1) % MAXQSIZE;
    return OK;
}

/* ==================== 4. 应用案例 ==================== */

/* (1) 数制转换：把十进制整数 m 转成 n 进制（n=2 或 8）并输出结果。
 * 余数依次进栈，再依次出栈即为从高位到低位的转换结果 */
Status Conversion(int m, int n)
{
    SqStack S;
    SElemType e;

    if (n != 2 && n != 8) {
        printf("仅支持 2 或 8 进制\n");
        return ERROR;
    }
    if (m < 0) {
        printf("暂不支持负数\n");
        return ERROR;
    }
    InitStack(S);
    printf("%d 的 %d 进制表示为：", m, n);
    if (m == 0)
        printf("0");
    while (m) {                 /* 除 n 取余，余数进栈 */
        Push(S, m % n);
        m /= n;
    }
    while (StackLength(S) > 0) { /* 逆序出栈：高位在前 */
        Pop(S, e);
        printf("%d", e);
    }
    printf("\n");
    free(S.base);
    return OK;
}

/* (2) 用栈把队列逆序重排：全部出队进栈，再全部出栈入队 */
Status QueueReverse(SqQueue &Q)
{
    SqStack S;
    QElemType e;

    InitStack(S);
    while (!QueueEmpty(Q)) {
        DeQueue(Q, e);
        Push(S, e);
    }
    while (StackLength(S) > 0) {
        Pop(S, e);
        EnQueue(Q, e);
    }
    free(S.base);
    return OK;
}

/* 辅助函数：显示队列当前状态（内容、长度、front、rear） */
Status QueueStatus(SqQueue Q)
{
    int i;
    printf("  队列内容(队头→队尾)：");
    for (i = Q.front; i != Q.rear; i = (i + 1) % MAXQSIZE)
        printf("%d ", Q.base[i]);
    printf("\n  长度 = %d，front = %d，rear = %d\n",
           QueueLength(Q), Q.front, Q.rear);
    return OK;
}

/* ==================== 5. 主函数 ==================== */
int main()
{
    SqQueue Q;
    QElemType e;
    int m, i;

    /* (1) 数制转换 */
    printf("输入一个十进制整数 m：");
    scanf("%d", &m);
    Conversion(m, 2);
    Conversion(m, 8);

    /* (2) 循环队列操作 */
    InitQueue(Q);

    printf("\n<1> 连续入队 9 个整数 1~9：\n");
    for (i = 1; i <= 9; i++)
        EnQueue(Q, i);
    QueueStatus(Q);             /* front=0, rear=9 */

    printf("\n<2> 再入队 1 个整数 10：\n");
    if (EnQueue(Q, 10) == ERROR)
        printf("  入队失败：队列已满（QueueFull = TRUE）\n");
    QueueStatus(Q);

    printf("\n<3> 连续出队 5 个：");
    for (i = 0; i < 5; i++) {
        DeQueue(Q, e);
        printf("%d ", e);
    }
    printf("\n    再入队 2 个整数 10、11：\n");
    EnQueue(Q, 10);
    EnQueue(Q, 11);
    QueueStatus(Q);             /* 长度=6, front=5, rear=1 */

    printf("\n<4> 调用 QueueReverse 逆序重排：\n");
    QueueReverse(Q);
    QueueStatus(Q);
    printf("  依次出队并输出：");
    while (!QueueEmpty(Q)) {
        DeQueue(Q, e);
        printf("%d ", e);
    }
    printf("\n");

    free(Q.base);
    return 0;
}
