#include <stdio.h>
#include <stdlib.h>
#define OK 1
#define ERROR 0
#define TRUE 1
#define FALSE 0
#define INFEASIBLE -1
#define OVERFLOW -2
typedef int Status;
#define STACK_INIT_SIZE 10
#define STACKINCREMENT 5
typedef int SElemType;
typedef struct {
    SElemType *base;
    SElemType *top;
    int stacksize;
} SqStack;
#define MAXQSIZE 10
typedef int QElemType;
typedef struct {
    QElemType *base;
    int front;
    int rear;
} SqQueue;
Status InitStack(SqStack &S)
{
    S.base = (SElemType *)malloc(STACK_INIT_SIZE * sizeof(SElemType));
    if (!S.base)
        return OVERFLOW;
    S.top = S.base;
    S.stacksize = STACK_INIT_SIZE;
    return OK;
}
Status ClearStack(SqStack &S)
{
    S.top = S.base;
    return OK;
}
int StackLength(SqStack S)
{
    return S.top - S.base;
}
Status GetTop(SqStack S, SElemType &e)
{
    if (S.top == S.base)
        return ERROR;
    e = *(S.top - 1);
    return OK;
}
Status Push(SqStack &S, SElemType e)
{
    SElemType *newbase;
    if (S.top - S.base >= S.stacksize) {
        newbase = (SElemType *)realloc(S.base,
                 (S.stacksize + STACKINCREMENT) * sizeof(SElemType));
        if (!newbase)
            return OVERFLOW;
        S.base = newbase;
        S.top = S.base + S.stacksize;
        S.stacksize += STACKINCREMENT;
    }
    *S.top++ = e;
    return OK;
}
Status Pop(SqStack &S, SElemType &e)
{
    if (S.top == S.base)
        return ERROR;
    e = *--S.top;
    return OK;
}
Status InitQueue(SqQueue &Q)
{
    Q.base = (QElemType *)malloc(MAXQSIZE * sizeof(QElemType));
    if (!Q.base)
        return OVERFLOW;
    Q.front = Q.rear = 0;
    return OK;
}
Status ClearQueue(SqQueue &Q)
{
    Q.front = Q.rear = 0;
    return OK;
}
int QueueLength(SqQueue Q)
{
    return (Q.rear - Q.front + MAXQSIZE) % MAXQSIZE;
}
Status QueueEmpty(SqQueue Q)
{
    return Q.front == Q.rear ? TRUE : FALSE;
}
Status QueueFull(SqQueue Q)
{
    return (Q.rear + 1) % MAXQSIZE == Q.front ? TRUE : FALSE;
}
Status EnQueue(SqQueue &Q, QElemType e)
{
    if (QueueFull(Q))
        return ERROR;
    Q.base[Q.rear] = e;
    Q.rear = (Q.rear + 1) % MAXQSIZE;
    return OK;
}
Status DeQueue(SqQueue &Q, QElemType &e)
{
    if (QueueEmpty(Q))
        return ERROR;
    e = Q.base[Q.front];
    Q.front = (Q.front + 1) % MAXQSIZE;
    return OK;
}
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
    while (m) {
        Push(S, m % n);
        m /= n;
    }
    while (StackLength(S) > 0) {
        Pop(S, e);
        printf("%d", e);
    }
    printf("\n");
    free(S.base);
    return OK;
}
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
int main()
{
    SqQueue Q;
    QElemType e;
    int m, i;
    printf("输入一个十进制整数 m：");
    scanf("%d", &m);
    Conversion(m, 2);
    Conversion(m, 8);
    InitQueue(Q);
    printf("\n<1> 连续入队 9 个整数 1~9：\n");
    for (i = 1; i <= 9; i++)
        EnQueue(Q, i);
    QueueStatus(Q);
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
    QueueStatus(Q);
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
