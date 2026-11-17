# -*- coding: utf-8 -*-
"""实验1: K-Means 聚类 FashionMNIST。

无监督聚类: 将 60000 张训练图像展平为 784 维向量, 用 K-Means(k=10) 聚类,
通过匈牙利算法将簇与真实类别一一对应后计算无监督聚类准确率,
同时输出 NMI/ARI, 并给出 PCA 二维可视化、聚类中心、簇-类别混淆矩阵等图。
"""
import os
import sys
import time

import numpy as np
import torch
from scipy.optimize import linear_sum_assignment
from sklearn.cluster import KMeans
from sklearn.decomposition import PCA
from sklearn.metrics import adjusted_rand_score, normalized_mutual_info_score

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from common import CLASS_NAMES_CN, RESULTS_DIR, save_json, seed_everything, setup_font

import matplotlib.pyplot as plt

setup_font()
seed_everything(42)


def load_flat_train_data():
    """加载 60000 张训练图, 展平为 (60000, 784) 并缩放到 [0,1]。"""
    from torchvision import datasets, transforms
    train_set = datasets.FashionMNIST(
        os.path.join(os.path.dirname(RESULTS_DIR), "data"),
        train=True, download=True, transform=transforms.ToTensor())
    x = torch.stack([s[0] for s in train_set])          # (60000,1,28,28)
    x = x.view(len(train_set), -1).numpy()              # (60000,784)
    y = np.array(train_set.targets)
    return x, y


def hungarian_accuracy(y_true, y_pred, k=10):
    """用匈牙利算法将簇编号与真实类别最优匹配, 返回 (准确率, 簇->类别映射)。"""
    cm = np.zeros((k, k), dtype=np.int64)   # 行=簇, 列=真实类
    for c, t in zip(y_pred, y_true):
        cm[c, t] += 1
    row, col = linear_sum_assignment(-cm)
    mapping = dict(zip(row, col))
    correct = cm[row, col].sum()
    return correct / len(y_true), mapping, cm


def main():
    t_start = time.time()
    print("== 实验一: K-Means 聚类 FashionMNIST ==")
    x, y_true = load_flat_train_data()
    print(f"数据: x={x.shape}, dtype={x.dtype}, 取值范围 [{x.min():.1f},{x.max():.1f}]")

    # ---- KMeans 聚类 ----
    print("开始 KMeans(n_clusters=10, n_init=10) ...")
    t0 = time.time()
    km = KMeans(n_clusters=10, n_init=10, random_state=42, verbose=0)
    y_pred = km.fit_predict(x)
    t_km = time.time() - t0
    print(f"聚类完成, 耗时 {t_km:.1f}s, 目标函数 inertia={km.inertia_:.1f}")

    # ---- 评估 ----
    acc, mapping, cm = hungarian_accuracy(y_true, y_pred)
    nmi = normalized_mutual_info_score(y_true, y_pred)
    ari = adjusted_rand_score(y_true, y_pred)
    print(f"无监督聚类准确率(匈牙利匹配): {acc*100:.2f}%")
    print(f"NMI={nmi:.4f}  ARI={ari:.4f}")
    for c in range(10):
        cnt = (y_pred == c).sum()
        print(f"  簇{c} -> 类别{CLASS_NAMES_CN[mapping[c]]:<6} 共 {cnt} 张")

    # ---- 图1: 聚类中心可视化 ----
    fig, axes = plt.subplots(2, 5, figsize=(12, 5.5))
    for c, ax in enumerate(axes.ravel()):
        ax.imshow(km.cluster_centers_[c].reshape(28, 28), cmap="gray")
        ax.set_title(f"簇{c} ≈ {CLASS_NAMES_CN[mapping[c]]}", fontsize=11)
        ax.axis("off")
    fig.suptitle("K-Means 聚类中心(k=10)", fontsize=14)
    fig.tight_layout()
    fig.savefig(os.path.join(RESULTS_DIR, "kmeans_centroids.png"), dpi=150)
    plt.close(fig)
    print("[saved] kmeans_centroids.png")

    # ---- 图2: 簇-真实类别混淆矩阵 ----
    fig, ax = plt.subplots(figsize=(7.5, 6.5))
    im = ax.imshow(cm, cmap="Purples")
    ax.set_xticks(range(10), CLASS_NAMES_CN, rotation=45, ha="right", fontsize=9)
    ax.set_yticks(range(10), [f"簇{i}" for i in range(10)], fontsize=9)
    thresh = cm.max() / 2
    for i in range(10):
        for j in range(10):
            if cm[i, j] > 0:
                ax.text(j, i, str(cm[i, j]), ha="center", va="center", fontsize=8,
                        color="white" if cm[i, j] > thresh else "black")
    ax.set_xlabel("真实类别")
    ax.set_ylabel("聚类簇")
    ax.set_title(f"簇-真实类别分布 (匹配准确率 {acc*100:.2f}%)")
    fig.colorbar(im, ax=ax, fraction=0.046)
    fig.tight_layout()
    fig.savefig(os.path.join(RESULTS_DIR, "kmeans_cluster_confusion.png"), dpi=150)
    plt.close(fig)
    print("[saved] kmeans_cluster_confusion.png")

    # ---- 图3: PCA 二维可视化(抽 8000 个点) ----
    rng = np.random.RandomState(0)
    idx = rng.choice(len(x), 8000, replace=False)
    t0 = time.time()
    pca = PCA(n_components=2, random_state=42)
    xy = pca.fit_transform(x[idx])
    print(f"PCA 耗时 {time.time()-t0:.1f}s, 两主元方差解释率: "
          f"{pca.explained_variance_ratio_[0]:.3f}/{pca.explained_variance_ratio_[1]:.3f}")

    fig, axes = plt.subplots(1, 2, figsize=(14, 6))
    for c in range(10):
        m = y_pred[idx] == c
        axes[0].scatter(xy[m, 0], xy[m, 1], s=4, alpha=0.4, label=f"簇{c}")
    axes[0].set_title("K-Means 聚类结果 (PCA 2D)")
    axes[0].legend(markerscale=2, fontsize=8)
    for t in range(10):
        m = y_true[idx] == t
        axes[1].scatter(xy[m, 0], xy[m, 1], s=4, alpha=0.4, label=CLASS_NAMES_CN[t])
    axes[1].set_title("真实标签分布 (PCA 2D)")
    axes[1].legend(markerscale=2, fontsize=8)
    for ax in axes:
        ax.grid(alpha=0.3)
        ax.set_xlabel(f"PC1 ({pca.explained_variance_ratio_[0]*100:.1f}%)")
        ax.set_ylabel(f"PC2 ({pca.explained_variance_ratio_[1]*100:.1f}%)")
    fig.tight_layout()
    fig.savefig(os.path.join(RESULTS_DIR, "kmeans_pca_2d.png"), dpi=150)
    plt.close(fig)
    print("[saved] kmeans_pca_2d.png")

    # ---- 图4: 每个簇的样本示例 ----
    fig, axes = plt.subplots(10, 8, figsize=(10, 13))
    for c in range(10):
        member = np.where(y_pred == c)[0]
        picks = rng.choice(member, 8, replace=len(member) < 8)
        for j, i in enumerate(picks):
            ax = axes[c, j]
            ax.imshow(x[i].reshape(28, 28), cmap="gray")
            ax.axis("off")
            if j == 0:
                ax.set_title(f"簇{c}\n≈{CLASS_NAMES_CN[mapping[c]]}", fontsize=9, loc="left")
    fig.suptitle("每个聚类簇的样本示例(按簇-类别最优匹配标注)", fontsize=13)
    fig.tight_layout()
    fig.savefig(os.path.join(RESULTS_DIR, "kmeans_samples.png"), dpi=150)
    plt.close(fig)
    print("[saved] kmeans_samples.png")

    # ---- 保存指标 ----
    save_json({
        "experiment": "K-Means 聚类 FashionMNIST",
        "n_samples": int(len(x)),
        "feature_dim": 784,
        "k": 10,
        "n_init": 10,
        "inertia": float(km.inertia_),
        "cluster_accuracy": float(acc),
        "NMI": float(nmi),
        "ARI": float(ari),
        "cluster_to_class": {int(c): CLASS_NAMES_CN[mapping[c]] for c in mapping},
        "cluster_sizes": {int(c): int((y_pred == c).sum()) for c in range(10)},
        "pca_variance_ratio": [float(v) for v in pca.explained_variance_ratio_],
        "kmeans_time_sec": round(t_km, 1),
        "total_time_sec": round(time.time() - t_start, 1),
    }, "exp1_kmeans_metrics.json")

    print(f"\n实验一完成, 总耗时 {time.time()-t_start:.1f}s")


if __name__ == "__main__":
    main()
