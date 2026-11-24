/*
 * 实验2.1 二叉树的二叉链表存储：先序建树、递归/非递归遍历、
 * 度统计 TNodes、树高 High、二叉排序树 CreateBST
 * 运行：make run-2.1（输入先序扩展序列，如 ABD#F###CE###）
 */
#include <iostream>
#include <string>
using namespace std;

#define OK 1
#define ERROR 0
#define MAXSIZE 100

typedef int Status;
typedef char TElemType;

typedef struct BiTNode {
    TElemType data;
    struct BiTNode *lchild, *rchild;
} BiTNode, *BiTree;

/* 输出栈中所有指针所指结点的字符，栈底到栈顶 */
void PrintStack(BiTree S[], int top) {
    cout << "[";
    for (int i = 0; i <= top; ++i) {
        if (i > 0) cout << " ";
        cout << S[i]->data;
    }
    cout << "]";
}

/* 1. 根据先序遍历扩展序列创建二叉树，#表示空 */
Status CreateBiTree(BiTree &T) {
    char ch;
    cin >> ch;
    if (ch == '#') {
        T = NULL;
        return OK;
    }
    T = new BiTNode;
    if (!T) return ERROR;
    T->data = ch;
    CreateBiTree(T->lchild);
    CreateBiTree(T->rchild);
    return OK;
}

/* 2. 递归先序遍历 */
void PreOrderTraverse(BiTree T) {
    if (T != NULL) {
        cout << T->data;
        PreOrderTraverse(T->lchild);
        PreOrderTraverse(T->rchild);
    }
}

/* 3. 递归中序遍历 */
void InOrderTraverse(BiTree T) {
    if (T != NULL) {
        InOrderTraverse(T->lchild);
        cout << T->data;
        InOrderTraverse(T->rchild);
    }
}

/* 4. 递归后序遍历 */
void PostOrderTraverse(BiTree T) {
    if (T != NULL) {
        PostOrderTraverse(T->lchild);
        PostOrderTraverse(T->rchild);
        cout << T->data;
    }
}

/* 5. 非递归先序遍历，输出进栈、出栈及栈内容 */
void NRPreOrderTraverse(BiTree T) {
    BiTree S[MAXSIZE];
    int top = -1;
    BiTree p = T;
    string result;

    cout << "非递归先序遍历过程：" << endl;
    while (p != NULL || top != -1) {
        while (p != NULL) {
            S[++top] = p;
            cout << "进栈 " << p->data << "，栈：";
            PrintStack(S, top);
            cout << endl;

            cout << "访问 " << p->data << endl;
            result += p->data;

            p = p->lchild;
        }

        if (top != -1) {
            p = S[top--];
            cout << "出栈 " << p->data << "，栈：";
            PrintStack(S, top);
            cout << endl;

            p = p->rchild;
        }
    }
    cout << "非递归先序序列：" << result << endl;
}

/* 6. 非递归中序遍历，输出进栈、出栈及栈内容 */
void NRInOrderTraverse(BiTree T) {
    BiTree S[MAXSIZE];
    int top = -1;
    BiTree p = T;
    string result;

    cout << "非递归中序遍历过程：" << endl;
    while (p != NULL || top != -1) {
        while (p != NULL) {
            S[++top] = p;
            cout << "进栈 " << p->data << "，栈：";
            PrintStack(S, top);
            cout << endl;

            p = p->lchild;
        }

        if (top != -1) {
            p = S[top--];
            cout << "出栈 " << p->data << "，栈：";
            PrintStack(S, top);
            cout << endl;

            cout << "访问 " << p->data << endl;
            result += p->data;

            p = p->rchild;
        }
    }
    cout << "非递归中序序列：" << result << endl;
}

/* 7. 非递归后序遍历，输出进栈、出栈、读栈顶及栈内容 */
void NRPostOrderTraverse(BiTree T) {
    BiTree S[MAXSIZE];
    int top = -1;
    BiTree p = T;
    BiTree lastVisited = NULL;
    string result;

    cout << "非递归后序遍历过程：" << endl;
    while (p != NULL || top != -1) {
        while (p != NULL) {
            S[++top] = p;
            cout << "进栈 " << p->data << "，栈：";
            PrintStack(S, top);
            cout << endl;

            p = p->lchild;
        }

        if (top != -1) {
            p = S[top];
            cout << "读栈顶 " << p->data << "，栈：";
            PrintStack(S, top);
            cout << endl;

            if (p->rchild != NULL && p->rchild != lastVisited) {
                p = p->rchild;
            } else {
                p = S[top--];
                cout << "出栈 " << p->data << "，栈：";
                PrintStack(S, top);
                cout << endl;

                cout << "访问 " << p->data << endl;
                result += p->data;

                lastVisited = p;
                p = NULL;
            }
        }
    }
    cout << "非递归后序序列：" << result << endl;
}

/* 应用1：返回二叉树T中度数为d的结点数 */
int TNodes(BiTree T, int d) {
    if (T == NULL) return 0;

    int degree = 0;
    if (T->lchild != NULL) degree++;
    if (T->rchild != NULL) degree++;

    int count = (degree == d) ? 1 : 0;
    return count + TNodes(T->lchild, d) + TNodes(T->rchild, d);
}

/* 应用2：求二叉树高度 */
int High(BiTree T) {
    if (T == NULL) return 0;
    int leftHigh = High(T->lchild);
    int rightHigh = High(T->rchild);
    return (leftHigh > rightHigh ? leftHigh : rightHigh) + 1;
}

/* 二叉排序树插入 */
Status BSTInsert(BiTree &T, TElemType e) {
    if (T == NULL) {
        T = new BiTNode;
        T->data = e;
        T->lchild = T->rchild = NULL;
        return OK;
    }
    if (e == T->data) {
        return ERROR; 
    } else if (e < T->data) {
        return BSTInsert(T->lchild, e);
    } else {
        return BSTInsert(T->rchild, e);
    }
}

/* 应用3：根据字符序列创建二叉排序树 */
Status CreateBST(BiTree &T, const char *chars) {
    T = NULL;
    for (int i = 0; chars[i] != '\0'; ++i) {
        BSTInsert(T, chars[i]);
    }
    return OK;
}

int main() {
    BiTree T = NULL;
    BiTree T1 = NULL;
    BiTree T2 = NULL;

    cout << "请输入题图中二叉树的先序扩展序列（#表示空结点）。" << endl;
    cout << "提示：输入 ABD#F###CE###" << endl;
    cout << "输入序列：";
    CreateBiTree(T);

    cout << "\n-----------------------" << endl;
    cout << "递归先序遍历：";
    PreOrderTraverse(T);
    cout << "\n递归中序遍历：";
    InOrderTraverse(T);
    cout << "\n递归后序遍历：";
    PostOrderTraverse(T);
    cout << "\n-----------------------\n" << endl;

    NRPreOrderTraverse(T);
    cout << endl;
    NRInOrderTraverse(T);
    cout << endl;
    NRPostOrderTraverse(T);
    cout << endl;

    cout << "-----------------------" << endl;
    cout << "二叉树T度数为0的结点数(叶子)： " << TNodes(T, 0) << endl;
    cout << "二叉树T度数为1的结点数： " << TNodes(T, 1) << endl;
    cout << "二叉树T度数为2的结点数： " << TNodes(T, 2) << endl;
    cout << "-----------------------\n" << endl;

    CreateBST(T1, "DBFCAEG");
    CreateBST(T2, "ABCDEFG");

    cout << "T1(DBFCAEG)中序遍历：";
    InOrderTraverse(T1);
    cout << "\nT1高度：" << High(T1) << endl;

    cout << "T2(ABCDEFG)中序遍历：";
    InOrderTraverse(T2);
    cout << "\nT2高度：" << High(T2) << endl;

    cout << "\n【分析比较】" << endl;
    cout << "T1和T2的中序遍历结果均为ABCDEFG，说明二叉排序树的中序遍历序列是有序序列。" << endl;
    cout << "但T1的插入序列分散，得到的高度为3；" << endl;
    cout << "T2按升序ABCDEFG插入，二叉排序树退化为单支树，高度达到7。" << endl;
    cout << "结论：二叉排序树的形态和高度与关键字的插入顺序密切相关。" << endl;

    return 0;
}
