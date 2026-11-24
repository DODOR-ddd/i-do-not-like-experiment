/*
 * 实验2.3 赫夫曼树：初始化、构造与赫夫曼编码
 * 运行：make run-2.3（无输入）
 */
#include <iostream>
#include <iomanip>
#include <cstring>
#include <limits>

using namespace std;

// 定义宏，用无符号整型的最大值表示 -1
#define NIL 0xFFFFFFFF

// 1. 赫夫曼树的存储结构定义
typedef struct {
    unsigned int weight;
    unsigned int parent, lchild, rchild;
} HTNode, *HTree;

typedef char **HTCode;

// 辅助函数：在 0 到 end 范围内，找出两个权值最小且没有父结点的结点
void Select(HTree HT, int end, int &s1, int &s2) {
    unsigned int min1 = NIL; // 最小权值
    unsigned int min2 = NIL; // 次小权值
    s1 = s2 = -1;

    for (int i = 0; i <= end; ++i) {
        // 只找没有父结点的结点（即未被合并的结点）
        if (HT[i].parent == NIL) {
            if (HT[i].weight < min1) {
                // 原来的最小变成次小
                min2 = min1;
                s2 = s1;
                // 更新最小
                min1 = HT[i].weight;
                s1 = i;
            } else if (HT[i].weight < min2) {
                // 更新次小
                min2 = HT[i].weight;
                s2 = i;
            }
        }
    }
}

// （1）初始化赫夫曼树
void InitHTree(HTree &HT, int *w, int n) {
    if (n <= 1) return;
    int m = 2 * n - 1; // 总结点数

    // 申请由 2n-1 个元素组成的一维数组
    HT = new HTNode[m];

    // 初始化叶子结点
    for (int i = 0; i < n; ++i) {
        HT[i].weight = w[i];
        HT[i].parent = NIL;
        HT[i].lchild = NIL;
        HT[i].rchild = NIL;
    }

    // 初始化非叶子结点
    for (int i = n; i < m; ++i) {
        HT[i].weight = 0;
        HT[i].parent = NIL;
        HT[i].lchild = NIL;
        HT[i].rchild = NIL;
    }
}

// 辅助函数：格式化输出哈夫曼树数组的内容
void PrintHTree(HTree HT, int n) {
    int m = 2 * n - 1;
    cout << setfill('-') << setw(55) << "-" << setfill(' ') << endl;
    cout << "| 索引 |  weight  |  parent  |  lchild  |  rchild  |" << endl;
    cout << setfill('-') << setw(55) << "-" << setfill(' ') << endl;
    for (int i = 0; i < m; ++i) {
        cout << "| " << setw(4) << i << " | ";
        cout << setw(8) << HT[i].weight << " | ";
        
        // 如果为 NIL (-1)，则输出 -1
        if (HT[i].parent == NIL) cout << setw(8) << -1 << " | ";
        else cout << setw(8) << HT[i].parent << " | ";

        if (HT[i].lchild == NIL) cout << setw(8) << -1 << " | ";
        else cout << setw(8) << HT[i].lchild << " | ";

        if (HT[i].rchild == NIL) cout << setw(8) << -1 << " |" << endl;
        else cout << setw(8) << HT[i].rchild << " |" << endl;
    }
    cout << setfill('-') << setw(55) << "-" << setfill(' ') << endl;
}

// （2）构造赫夫曼树
void CreateHTree(HTree &HT, int n) {
    int m = 2 * n - 1;
    // 从第 n 个结点开始构造，总共需要构造 n-1 个内部结点
    for (int i = n; i < m; ++i) {
        int s1, s2;
        // 在 0 到 i-1 范围内选出两个权值最小的结点
        Select(HT, i - 1, s1, s2);
        
        HT[s1].parent = i;
        HT[s2].parent = i;
        
        // 默认左孩子权值较小，右孩子权值较大
        HT[i].lchild = s1;
        HT[i].rchild = s2;
        HT[i].weight = HT[s1].weight + HT[s2].weight;
    }
}

// （3）生成赫夫曼编码并输出
void HTCoding(HTree HT, HTCode &HC, int n) {
    HC = new char*[n];        // 分配指针数组存储 n 个编码
    char *cd = new char[n];   // 临时工作空间，最长编码长度为 n-1（加结束符共 n）
    cd[n - 1] = '\0';         // 字符串结束符

    cout << "\n各叶子结点的赫夫曼编码：" << endl;
    cout << "叶子索引\t权重\t\t编码" << endl;

    for (int i = 0; i < n; ++i) {
        int start = n - 1;
        int c = i;
        unsigned int f = HT[i].parent;

        // 从叶子结点向上回溯到根结点
        while (f != NIL) {
            --start;
            if (HT[f].lchild == (unsigned int)c) {
                cd[start] = '0'; // 左分支为 '0'
            } else {
                cd[start] = '1'; // 右分支为 '1'
            }
            c = f;
            f = HT[f].parent;
        }

        // 为当前编码分配实际所需的内存并复制
        HC[i] = new char[n - start];
        strcpy(HC[i], &cd[start]);

        cout << "  " << i << "\t\t" << HT[i].weight << "\t\t" << HC[i] << endl;
    }

    delete[] cd; // 释放临时空间
}

int main() {
    HTree HT = NULL;
    HTCode HC = NULL;

    int w[] = {5, 29, 7, 8, 14, 23, 3, 11};
    int n = sizeof(w) / sizeof(w[0]); // 获取叶子结点数 8

    cout << "（1）初始化赫夫曼树（n = " << n << "）：" << endl;
    InitHTree(HT, w, n);
    PrintHTree(HT, n);

    cout << "\n（2）构造完整的赫夫曼树：" << endl;
    CreateHTree(HT, n);
    PrintHTree(HT, n);

    cout << "\n（3）生成并输出赫夫曼编码：" << endl;
    HTCoding(HT, HC, n);

    // 释放内存
    for (int i = 0; i < n; ++i) {
        delete[] HC[i];
    }
    delete[] HC;
    delete[] HT;

    return 0;
}