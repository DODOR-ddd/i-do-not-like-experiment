#include <iostream>
#include <iomanip>
#include <cstring>
#include <limits>
using namespace std;
#define NIL 0xFFFFFFFF
typedef struct {
    unsigned int weight;
    unsigned int parent, lchild, rchild;
} HTNode, *HTree;
typedef char **HTCode;
void Select(HTree HT, int end, int &s1, int &s2) {
    unsigned int min1 = NIL;
    unsigned int min2 = NIL;
    s1 = s2 = -1;
    for (int i = 0; i <= end; ++i) {
        if (HT[i].parent == NIL) {
            if (HT[i].weight < min1) {
                min2 = min1;
                s2 = s1;
                min1 = HT[i].weight;
                s1 = i;
            } else if (HT[i].weight < min2) {
                min2 = HT[i].weight;
                s2 = i;
            }
        }
    }
}
void InitHTree(HTree &HT, int *w, int n) {
    if (n <= 1) return;
    int m = 2 * n - 1;
    HT = new HTNode[m];
    for (int i = 0; i < n; ++i) {
        HT[i].weight = w[i];
        HT[i].parent = NIL;
        HT[i].lchild = NIL;
        HT[i].rchild = NIL;
    }
    for (int i = n; i < m; ++i) {
        HT[i].weight = 0;
        HT[i].parent = NIL;
        HT[i].lchild = NIL;
        HT[i].rchild = NIL;
    }
}
void PrintHTree(HTree HT, int n) {
    int m = 2 * n - 1;
    cout << setfill('-') << setw(55) << "-" << setfill(' ') << endl;
    cout << "| 索引 |  weight  |  parent  |  lchild  |  rchild  |" << endl;
    cout << setfill('-') << setw(55) << "-" << setfill(' ') << endl;
    for (int i = 0; i < m; ++i) {
        cout << "| " << setw(4) << i << " | ";
        cout << setw(8) << HT[i].weight << " | ";
        if (HT[i].parent == NIL) cout << setw(8) << -1 << " | ";
        else cout << setw(8) << HT[i].parent << " | ";
        if (HT[i].lchild == NIL) cout << setw(8) << -1 << " | ";
        else cout << setw(8) << HT[i].lchild << " | ";
        if (HT[i].rchild == NIL) cout << setw(8) << -1 << " |" << endl;
        else cout << setw(8) << HT[i].rchild << " |" << endl;
    }
    cout << setfill('-') << setw(55) << "-" << setfill(' ') << endl;
}
void CreateHTree(HTree &HT, int n) {
    int m = 2 * n - 1;
    for (int i = n; i < m; ++i) {
        int s1, s2;
        Select(HT, i - 1, s1, s2);
        HT[s1].parent = i;
        HT[s2].parent = i;
        HT[i].lchild = s1;
        HT[i].rchild = s2;
        HT[i].weight = HT[s1].weight + HT[s2].weight;
    }
}
void HTCoding(HTree HT, HTCode &HC, int n) {
    HC = new char*[n];
    char *cd = new char[n];
    cd[n - 1] = '\0';
    cout << "\n各叶子结点的赫夫曼编码：" << endl;
    cout << "叶子索引\t权重\t\t编码" << endl;
    for (int i = 0; i < n; ++i) {
        int start = n - 1;
        int c = i;
        unsigned int f = HT[i].parent;
        while (f != NIL) {
            --start;
            if (HT[f].lchild == (unsigned int)c) {
                cd[start] = '0';
            } else {
                cd[start] = '1';
            }
            c = f;
            f = HT[f].parent;
        }
        HC[i] = new char[n - start];
        strcpy(HC[i], &cd[start]);
        cout << "  " << i << "\t\t" << HT[i].weight << "\t\t" << HC[i] << endl;
    }
    delete[] cd;
}
int main() {
    HTree HT = NULL;
    HTCode HC = NULL;
    int w[] = {5, 29, 7, 8, 14, 23, 3, 11};
    int n = sizeof(w) / sizeof(w[0]);
    cout << "（1）初始化赫夫曼树（n = " << n << "）：" << endl;
    InitHTree(HT, w, n);
    PrintHTree(HT, n);
    cout << "\n（2）构造完整的赫夫曼树：" << endl;
    CreateHTree(HT, n);
    PrintHTree(HT, n);
    cout << "\n（3）生成并输出赫夫曼编码：" << endl;
    HTCoding(HT, HC, n);
    for (int i = 0; i < n; ++i) {
        delete[] HC[i];
    }
    delete[] HC;
    delete[] HT;
    return 0;
}
