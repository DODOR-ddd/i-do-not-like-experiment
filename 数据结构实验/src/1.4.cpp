/*
 * 实验1.4 串的堆分配存储结构（C++ 引用版）
 * 编译并运行：make run-1.4
 */
#include <stdio.h>
#include <stdlib.h>

#define OK 1
#define ERROR 0
#define TRUE 1
#define FALSE 0
#define INFEASIBLE -1
#define OVERFLOW -2

typedef int Status;     /* 函数类型名 */

/* 1. 串的堆分配存储结构定义 */
typedef struct {
    char *ch;           /* 非空串按串长分配存储区，否则 ch 为 NULL */
    int length;         /* 串长度 */
} HString;

/* ==================== 2. 基本操作函数 ==================== */

/* (1) 将字符串常量 chars 赋给串 S
 * 注意：chars 用 const char*，因为字符串常量在 C++ 里是只读的 */
Status StrAssign(HString &S, const char *chars)
{
    int len = 0, i;
    while (chars[len])          /* 手工求常量串长，不依赖 string.h */
        len++;
    if (S.ch)                   /* 释放旧空间，支持重复赋值 */
        free(S.ch);
    if (len == 0) {             /* 空串：ch 为 NULL */
        S.ch = NULL;
        S.length = 0;
        return OK;
    }
    S.ch = (char *)malloc(len * sizeof(char));
    if (!S.ch)
        return OVERFLOW;
    for (i = 0; i < len; i++)
        S.ch[i] = chars[i];
    S.length = len;
    return OK;
}

/* (2) 比较串 S 与 T：S>T 返回 1，S=T 返回 0，S<T 返回 -1。
 * 先逐字符比，全部相同时短串小 */
int StrCompare(HString S, HString T)
{
    int i;
    for (i = 0; i < S.length && i < T.length; i++)
        if (S.ch[i] != T.ch[i])
            return S.ch[i] > T.ch[i] ? 1 : -1;
    if (S.length == T.length)
        return 0;
    return S.length > T.length ? 1 : -1;
}

/* (3) 返回串 S 的长度 */
int StrLength(HString S)
{
    return S.length;
}

/* (4) 将 T1 和 T2 连接成新串赋给 S。
 * 先把两串复制进新空间、复制完才 free 旧空间，
 * 这样 Concat(S, S, T) 这类 S 同时作为源和目的的调用也安全 */
Status Concat(HString &S, HString T1, HString T2)
{
    char *newch;
    int i, newlen = T1.length + T2.length;

    if (newlen == 0) {          /* 两串都空：结果为空串 */
        if (S.ch)
            free(S.ch);
        S.ch = NULL;
        S.length = 0;
        return OK;
    }
    newch = (char *)malloc(newlen * sizeof(char));
    if (!newch)
        return OVERFLOW;
    for (i = 0; i < T1.length; i++)     /* T1 为空串时循环不执行，不会碰 NULL */
        newch[i] = T1.ch[i];
    for (i = 0; i < T2.length; i++)
        newch[T1.length + i] = T2.ch[i];
    if (S.ch)                   /* 复制完成后才释放 S 的旧空间 */
        free(S.ch);
    S.ch = newch;
    S.length = newlen;
    return OK;
}

/* (5) 用 sub 返回串 S 第 pos 个字符起长度为 len 的子串
 * 1<=pos<=StrLength(S)，1<=len<=StrLength(S)-pos+1 */
Status SubString(HString &sub, HString S, int pos, int len)
{
    int i;
    if (pos < 1 || pos > S.length || len < 1 || len > S.length - pos + 1)
        return ERROR;
    if (sub.ch)
        free(sub.ch);
    sub.ch = (char *)malloc(len * sizeof(char));
    if (!sub.ch)
        return OVERFLOW;
    for (i = 0; i < len; i++)
        sub.ch[i] = S.ch[pos - 1 + i];  /* 位序 pos 对应下标 pos-1 */
    sub.length = len;
    return OK;
}

/* (6) 遍历输出串 S 的各个字符 */
Status StrTraverse(HString S)
{
    int i;
    for (i = 0; i < S.length; i++)
        printf("%c", S.ch[i]);
    printf("\n");
    return OK;
}

/* ==================== 3. 其他操作函数（组合基本操作实现） ==================== */

/* (1) 将串 T 赋给串 S：与空串连接即得 T 的副本 */
Status StrCopy(HString &S, HString T)
{
    HString empty = {NULL, 0};  /* 空串：ch 必须初始化为 NULL */
    return Concat(S, T, empty);
}

/* (2) 返回串 T 在 S 中第 pos 个字符起首次出现的位置，没有则返回 0 */
int Index(HString S, HString T, int pos)
{
    HString sub = {NULL, 0};
    int i, n = StrLength(S), m = StrLength(T);

    if (m == 0 || pos < 1 || pos > n)
        return 0;
    for (i = pos; i + m - 1 <= n; i++) {    /* 逐个位置截取 m 长子串比较 */
        SubString(sub, S, i, m);
        if (StrCompare(sub, T) == 0) {
            free(sub.ch);
            return i;
        }
    }
    free(sub.ch);
    return 0;
}

/* (3) 将 S 中所有子串 T1 替换为 T2：从头扫描，
 * 命中则向结果串拼 T2 并跳过 T1 长度，否则逐字符拼入 */
Status StrReplace(HString &S, HString T1, HString T2)
{
    HString res  = {NULL, 0};   /* 结果串，初始为空 */
    HString sub  = {NULL, 0};
    HString tail = {NULL, 0};
    int i = 1, m = StrLength(T1);

    if (m == 0)
        return ERROR;
    while (i + m - 1 <= StrLength(S)) {
        SubString(sub, S, i, m);
        if (StrCompare(sub, T1) == 0) {
            Concat(res, res, T2);           /* 命中：拼入 T2，跳过整个 T1 */
            i += m;
        } else {
            SubString(sub, S, i, 1);        /* 未命中：拼入当前一个字符 */
            Concat(res, res, sub);
            i++;
        }
    }
    if (i <= StrLength(S)) {                /* 末尾剩余部分整体拼入 */
        SubString(tail, S, i, StrLength(S) - i + 1);
        Concat(res, res, tail);
    }
    free(sub.ch);
    free(tail.ch);
    if (S.ch)
        free(S.ch);
    S = res;                                /* 新串替换旧串 */
    return OK;
}

/* (4) 在串 S 第 pos 个字符之前插入串 T，1<=pos<=StrLength(S)+1 */
Status StrInsert(HString &S, int pos, HString T)
{
    HString head = {NULL, 0}, tail = {NULL, 0};

    if (pos < 1 || pos > StrLength(S) + 1)
        return ERROR;
    if (pos > 1)
        SubString(head, S, 1, pos - 1);                 /* 前 pos-1 个字符 */
    if (pos <= StrLength(S))
        SubString(tail, S, pos, StrLength(S) - pos + 1); /* 第 pos 个起尾部 */
    Concat(S, head, T);         /* S = 前段 + T */
    Concat(S, S, tail);         /* S = S + 尾段（Concat 支持源与目的相同） */
    free(head.ch);
    free(tail.ch);
    return OK;
}

/* (5) 删除 S 中第 pos 个字符起连续 len 个字符 */
Status StrDelete(HString &S, int pos, int len)
{
    HString head = {NULL, 0}, tail = {NULL, 0};

    if (pos < 1 || len < 1 || pos + len - 1 > StrLength(S))
        return ERROR;
    if (pos > 1)
        SubString(head, S, 1, pos - 1);                 /* 删除点之前 */
    if (pos + len <= StrLength(S))
        SubString(tail, S, pos + len, StrLength(S) - pos - len + 1); /* 之后 */
    Concat(S, head, tail);      /* S = 前段 + 后段 */
    free(head.ch);
    free(tail.ch);
    return OK;
}

/* ==================== 4. 主函数 ==================== */
int main()
{
    /* 堆分配串的关键：所有 ch 必须初始化为 NULL，
     * 否则操作函数里 free 旧空间时释放的是垃圾指针 */
    HString T1 = {NULL, 0}, T2 = {NULL, 0}, T3 = {NULL, 0};
    HString S   = {NULL, 0};
    int pos;

    /* (1) 数据准备 */
    StrAssign(T1, "data");
    StrAssign(T2, "structure");
    StrAssign(T3, "Struct");
    printf("(1) 数据准备：\nT1 = ");
    StrTraverse(T1);
    printf("T2 = ");
    StrTraverse(T2);
    printf("T3 = ");
    StrTraverse(T3);
    printf("StrCompare(T1, T2) = %d（T1<T2 时为 -1）\n", StrCompare(T1, T2));

    /* (2) 复制 T1 到 S */
    StrCopy(S, T1);
    printf("\n(2) StrCopy 把 T1 赋给 S：\nS = ");
    StrTraverse(S);

    /* (3) T2 插入到 S 尾部 */
    StrInsert(S, StrLength(S) + 1, T2);
    printf("\n(3) StrInsert 把 T2 插到 S 尾部：\nS = ");
    StrTraverse(S);

    /* (4) 把 S 中的 T1 替换成 T3 */
    StrReplace(S, T1, T3);
    printf("\n(4) StrReplace 把 S 中的 T1 替换成 T3：\nS = ");
    StrTraverse(S);

    /* (5) 查找 T2 在 S 中的位置 */
    pos = Index(S, T2, 1);
    printf("\n(5) Index 查找 T2 在 S 中的位置：%d\n", pos);

    /* (6) 删除 S 中的子串 T2 */
    StrDelete(S, pos, StrLength(T2));
    printf("\n(6) StrDelete 删除 S 中的 T2 后：\nS = ");
    StrTraverse(S);

    free(T1.ch);
    free(T2.ch);
    free(T3.ch);
    free(S.ch);
    return 0;
}
