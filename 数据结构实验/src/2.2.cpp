#include <iostream>
#include <string>
using namespace std;
#define OK 1
#define ERROR 0
#define MAXSIZE 100
typedef int Status;
typedef char TElemType;
typedef struct BiThrNode {
    TElemType data;
    struct BiThrNode *lchild, *rchild;
    int LTag, RTag;
} BiThrNode, *BiThrTree;
Status InitBiThrTree(BiThrTree &T) {
    char ch;
    cin >> ch;
    if (ch == '#') {
        T = NULL;
        return OK;
    }
    T = new BiThrNode;
    T->data = ch;
    T->LTag = 0;
    T->RTag = 0;
    InitBiThrTree(T->lchild);
    InitBiThrTree(T->rchild);
    if (T->lchild == NULL) T->LTag = 1;
    if (T->rchild == NULL) T->RTag = 1;
    return OK;
}
Status InOrderThreading(BiThrTree &Thrt, BiThrTree T) {
    Thrt = new BiThrNode;
    Thrt->data = '@';
    Thrt->LTag = 0;
    Thrt->RTag = 1;
    if (T == NULL) {
        Thrt->lchild = Thrt;
        Thrt->rchild = Thrt;
        return OK;
    }
    Thrt->lchild = T;
    Thrt->rchild = NULL;
    BiThrTree pre = Thrt;
    BiThrTree S[MAXSIZE];
    int top = -1;
    BiThrTree p = T;
    while (p != NULL || top != -1) {
        while (p != NULL) {
            S[++top] = p;
            p = p->lchild;
        }
        if (top != -1) {
            p = S[top--];
            if (!p->lchild) {
                p->LTag = 1;
                p->lchild = pre;
            }
            if (!pre->rchild) {
                pre->RTag = 1;
                pre->rchild = p;
            }
            pre = p;
            p = p->rchild;
        }
    }
    pre->rchild = Thrt;
    pre->RTag = 1;
    Thrt->rchild = pre;
    return OK;
}
void PrintNodeInfo(BiThrTree p) {
    string leftElem = "^", rightElem = "^";
    if (p->lchild != NULL) {
        if (p->lchild->data == '@') leftElem = "@";
        else leftElem = string(1, p->lchild->data);
    }
    if (p->rchild != NULL) {
        if (p->rchild->data == '@') rightElem = "@";
        else rightElem = string(1, p->rchild->data);
    }
    cout << "| " << p->LTag << " | " << leftElem << " | " << p->data << " | " << rightElem << " | " << p->RTag << " |" << endl;
}
void InOrderTraverse(BiThrTree T) {
    if (T == NULL) return;
    if (T->data == '@') {
        if (T->lchild != T) InOrderTraverse(T->lchild);
        return;
    }
    if (T->LTag == 0) InOrderTraverse(T->lchild);
    PrintNodeInfo(T);
    if (T->RTag == 0) InOrderTraverse(T->rchild);
}
void InOrderTraverse_Thr(BiThrTree T) {
    if (T == NULL) return;
    BiThrTree p = T->lchild;
    while (p != T) {
        while (p->LTag == 0) {
            p = p->lchild;
        }
        PrintNodeInfo(p);
        while (p->RTag == 1 && p->rchild != T) {
            p = p->rchild;
            PrintNodeInfo(p);
        }
        p = p->rchild;
    }
}
int main() {
    BiThrTree T = NULL;
    BiThrTree Thrt = NULL;
    cout << "请输入题图中二叉树的先序扩展序列（#表示空结点）。" << endl;
    cout << "提示：输入 ABD#F###CE###" << endl;
    cout << "输入序列：";
    InitBiThrTree(T);
    cout << "\n（2）未线索化二叉树：按孩子中序遍历输出：" << endl;
    InOrderTraverse(T);
    cout << "\n（3）将T线索化成一棵中序线索二叉树..." << endl;
    InOrderThreading(Thrt, T);
    cout << "线索化完成！" << endl;
    cout << "\n（4）线索化二叉树：按孩子中序遍历输出：" << endl;
    InOrderTraverse(Thrt);
    cout << "\n（5）线索化二叉树：按线索和孩子中序遍历（非递归）输出：" << endl;
    InOrderTraverse_Thr(Thrt);
    return 0;
}
