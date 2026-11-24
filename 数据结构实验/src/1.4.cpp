#include <stdio.h>
#include <stdlib.h>
#define OK 1
#define ERROR 0
#define TRUE 1
#define FALSE 0
#define INFEASIBLE -1
#define OVERFLOW -2
typedef int Status;
typedef struct {
    char *ch;
    int length;
} HString;
Status StrAssign(HString &S, const char *chars)
{
    int len = 0, i;
    while (chars[len])
        len++;
    if (S.ch)
        free(S.ch);
    if (len == 0) {
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
int StrLength(HString S)
{
    return S.length;
}
Status Concat(HString &S, HString T1, HString T2)
{
    char *newch;
    int i, newlen = T1.length + T2.length;
    if (newlen == 0) {
        if (S.ch)
            free(S.ch);
        S.ch = NULL;
        S.length = 0;
        return OK;
    }
    newch = (char *)malloc(newlen * sizeof(char));
    if (!newch)
        return OVERFLOW;
    for (i = 0; i < T1.length; i++)
        newch[i] = T1.ch[i];
    for (i = 0; i < T2.length; i++)
        newch[T1.length + i] = T2.ch[i];
    if (S.ch)
        free(S.ch);
    S.ch = newch;
    S.length = newlen;
    return OK;
}
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
        sub.ch[i] = S.ch[pos - 1 + i];
    sub.length = len;
    return OK;
}
Status StrTraverse(HString S)
{
    int i;
    for (i = 0; i < S.length; i++)
        printf("%c", S.ch[i]);
    printf("\n");
    return OK;
}
Status StrCopy(HString &S, HString T)
{
    HString empty = {NULL, 0};
    return Concat(S, T, empty);
}
int Index(HString S, HString T, int pos)
{
    HString sub = {NULL, 0};
    int i, n = StrLength(S), m = StrLength(T);
    if (m == 0 || pos < 1 || pos > n)
        return 0;
    for (i = pos; i + m - 1 <= n; i++) {
        SubString(sub, S, i, m);
        if (StrCompare(sub, T) == 0) {
            free(sub.ch);
            return i;
        }
    }
    free(sub.ch);
    return 0;
}
Status StrReplace(HString &S, HString T1, HString T2)
{
    HString res = {NULL, 0};
    HString sub = {NULL, 0};
    HString tail = {NULL, 0};
    int i = 1, m = StrLength(T1);
    if (m == 0)
        return ERROR;
    while (i + m - 1 <= StrLength(S)) {
        SubString(sub, S, i, m);
        if (StrCompare(sub, T1) == 0) {
            Concat(res, res, T2);
            i += m;
        } else {
            SubString(sub, S, i, 1);
            Concat(res, res, sub);
            i++;
        }
    }
    if (i <= StrLength(S)) {
        SubString(tail, S, i, StrLength(S) - i + 1);
        Concat(res, res, tail);
    }
    free(sub.ch);
    free(tail.ch);
    if (S.ch)
        free(S.ch);
    S = res;
    return OK;
}
Status StrInsert(HString &S, int pos, HString T)
{
    HString head = {NULL, 0}, tail = {NULL, 0};
    if (pos < 1 || pos > StrLength(S) + 1)
        return ERROR;
    if (pos > 1)
        SubString(head, S, 1, pos - 1);
    if (pos <= StrLength(S))
        SubString(tail, S, pos, StrLength(S) - pos + 1);
    Concat(S, head, T);
    Concat(S, S, tail);
    free(head.ch);
    free(tail.ch);
    return OK;
}
Status StrDelete(HString &S, int pos, int len)
{
    HString head = {NULL, 0}, tail = {NULL, 0};
    if (pos < 1 || len < 1 || pos + len - 1 > StrLength(S))
        return ERROR;
    if (pos > 1)
        SubString(head, S, 1, pos - 1);
    if (pos + len <= StrLength(S))
        SubString(tail, S, pos + len, StrLength(S) - pos - len + 1);
    Concat(S, head, tail);
    free(head.ch);
    free(tail.ch);
    return OK;
}
int main()
{
    HString T1 = {NULL, 0}, T2 = {NULL, 0}, T3 = {NULL, 0};
    HString S = {NULL, 0};
    int pos;
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
    StrCopy(S, T1);
    printf("\n(2) StrCopy 把 T1 赋给 S：\nS = ");
    StrTraverse(S);
    StrInsert(S, StrLength(S) + 1, T2);
    printf("\n(3) StrInsert 把 T2 插到 S 尾部：\nS = ");
    StrTraverse(S);
    StrReplace(S, T1, T3);
    printf("\n(4) StrReplace 把 S 中的 T1 替换成 T3：\nS = ");
    StrTraverse(S);
    pos = Index(S, T2, 1);
    printf("\n(5) Index 查找 T2 在 S 中的位置：%d\n", pos);
    StrDelete(S, pos, StrLength(T2));
    printf("\n(6) StrDelete 删除 S 中的 T2 后：\nS = ");
    StrTraverse(S);
    free(T1.ch);
    free(T2.ch);
    free(T3.ch);
    free(S.ch);
    return 0;
}
