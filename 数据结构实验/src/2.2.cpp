/*
 * 实验2.2 中序线索二叉树：线索化与遍历
 * 运行：make run-2.2（输入先序扩展序列，如 ABD#F###CE###）
 */
#include <iostream>
#include <string>

using namespace std;

#define OK 1
#define ERROR 0
#define MAXSIZE 100

typedef int Status;
typedef char TElemType;

// 线索二叉链表结构定义
typedef struct BiThrNode {
    TElemType data;
    struct BiThrNode *lchild, *rchild;
    int LTag, RTag;
} BiThrNode, *BiThrTree;

// （1）根据先序遍历的字符序列创建未线索化的二叉树
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
    
    // 如果左/右孩子为空，将标记设为 1
    if (T->lchild == NULL) T->LTag = 1;
    if (T->rchild == NULL) T->RTag = 1;
    
    return OK;
}

// （2）中序遍历进行线索化（带头结点）
Status InOrderThreading(BiThrTree &Thrt, BiThrTree T) {
    // 创建头结点
    Thrt = new BiThrNode;
    Thrt->data = '@';
    Thrt->LTag = 0;
    Thrt->RTag = 1;
    
    if (T == NULL) {
        Thrt->lchild = Thrt;
        Thrt->rchild = Thrt;
        return OK;
    }
    
    Thrt->lchild = T;  // 头结点的左指针指向根结点
    Thrt->rchild = NULL; // 初始化为NULL，后续在遍历中指向中序最后一个结点
    BiThrTree pre = Thrt;
    
    // 非递归中序遍历线索化
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
            
            // 访问 p 时进行线索化
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
    
    // 遍历结束后，对最后一个结点线索化
    pre->rchild = Thrt;
    pre->RTag = 1;
    Thrt->rchild = pre;
    
    return OK;
}

// 辅助函数：按格式输出结点信息
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

// （3）按孩子中序遍历线索二叉树（递归）
void InOrderTraverse(BiThrTree T) {
    if (T == NULL) return;
    if (T->data == '@') {          // 传入头结点：改从其左孩子（根结点）开始
        if (T->lchild != T) InOrderTraverse(T->lchild);
        return;
    }

    // 只有 LTag == 0 才代表是真正的左孩子
    if (T->LTag == 0) InOrderTraverse(T->lchild);
    PrintNodeInfo(T);
    // 只有 RTag == 0 才代表是真正的右孩子
    if (T->RTag == 0) InOrderTraverse(T->rchild);
}

// （4）按线索和孩子中序遍历线索二叉树（非递归）
void InOrderTraverse_Thr(BiThrTree T) {
    if (T == NULL) return;
    
    BiThrTree p = T->lchild; // p 指向根结点
    while (p != T) { // 空树或遍历结束时，p == T
        // 一直向左走到头，找到中序第一个结点
        while (p->LTag == 0) {
            p = p->lchild;
        }
        PrintNodeInfo(p);
        
        // 如果右指针是线索，一直顺着线索向右走，直到遇到真正的右孩子
        while (p->RTag == 1 && p->rchild != T) {
            p = p->rchild;
            PrintNodeInfo(p);
        }
        
        // 进入右子树
        p = p->rchild;
    }
}

// 主函数
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
    InOrderTraverse(Thrt);         // 按题目要求传入头结点，函数内部从根结点开始

    cout << "\n（5）线索化二叉树：按线索和孩子中序遍历（非递归）输出：" << endl;
    InOrderTraverse_Thr(Thrt); // 从头结点开始

    return 0;
}