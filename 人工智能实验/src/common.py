# -*- coding: utf-8 -*-
"""公共模块: 中文字体、数据加载、训练/评估工具、画图工具。"""
import json
import os
import random
import time

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np
import torch
from matplotlib import font_manager
from torch import nn

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DATA_DIR = os.path.join(ROOT, "data")
RESULTS_DIR = os.path.join(ROOT, "results")
os.makedirs(RESULTS_DIR, exist_ok=True)

DEVICE = torch.device("cuda" if torch.cuda.is_available() else "cpu")

# FashionMNIST 十个类别
CLASS_NAMES = ["T-shirt/top", "Trouser", "Pullover", "Dress", "Coat",
               "Sandal", "Shirt", "Sneaker", "Bag", "Ankle boot"]
CLASS_NAMES_CN = ["T恤/上衣", "裤子", "套头衫", "连衣裙", "外套",
                  "凉鞋", "衬衫", "运动鞋", "包", "短靴"]

# FashionMNIST 全集均值/标准差(单通道)
FMNIST_MEAN, FMNIST_STD = (0.2860,), (0.3530,)


def setup_font():
    """尽量启用简体中文字体, 图片标题/标签可直接写中文。

    系统 ttc 集合字体 matplotlib 只注册第一个 face(通常是 JP),
    这里用 fontTools 把 SC face 提取成 .ttf 再注册, 失败则退回 JP face。
    """
    import glob

    registered = {f.name for f in font_manager.fontManager.ttflist}
    if "Noto Sans CJK SC" in registered:
        plt.rcParams["font.family"] = "Noto Sans CJK SC"
        plt.rcParams["axes.unicode_minus"] = False
        return

    # 系统 ttc 集合字体提取 SC face 存成独立 .ttf 后注册
    cache_dir = os.path.expanduser("~/.cache/matplotlib_cjk")
    os.makedirs(cache_dir, exist_ok=True)
    out = os.path.join(cache_dir, "NotoSansCJKSC.ttf")
    if os.path.exists(out):
        font_manager.fontManager.addfont(out)
    else:
        from fontTools.ttLib import TTCollection
        ttcs = sorted(glob.glob("/usr/share/fonts/**/*.tt[cf]", recursive=True))
        ttcs.sort(key=lambda p: "Regular" not in p)   # Regular 字重优先
        done = False
        for ttc in ttcs:
            try:
                for face in TTCollection(ttc).fonts:
                    name = face["name"].getDebugName(1) or ""
                    if name == "Noto Sans CJK SC":
                        face.save(out)
                        done = True
                        break
            except Exception:
                continue
            if done:
                break
        if not done:
            print("[warn] 未找到中文字体 Noto Sans CJK SC, 图中中文可能显示为方块")
            return
    font_manager.fontManager.addfont(out)
    plt.rcParams["font.family"] = "Noto Sans CJK SC"
    plt.rcParams["axes.unicode_minus"] = False


def seed_everything(seed=42):
    random.seed(seed)
    np.random.seed(seed)
    torch.manual_seed(seed)
    if torch.cuda.is_available():
        torch.cuda.manual_seed_all(seed)


def load_fashion_mnist():
    """返回 (train_loader, test_loader, train_set, test_set)。首次运行自动下载。"""
    from torchvision import datasets, transforms

    tf = transforms.Compose([
        transforms.ToTensor(),
        transforms.Normalize(FMNIST_MEAN, FMNIST_STD),
    ])
    train_set = datasets.FashionMNIST(DATA_DIR, train=True, download=True, transform=tf)
    test_set = datasets.FashionMNIST(DATA_DIR, train=False, download=True, transform=tf)
    train_loader = torch.utils.data.DataLoader(
        train_set, batch_size=128, shuffle=True, num_workers=4, pin_memory=True)
    test_loader = torch.utils.data.DataLoader(
        test_set, batch_size=256, shuffle=False, num_workers=4, pin_memory=True)
    return train_loader, test_loader, train_set, test_set


def train_one_epoch(model, loader, criterion, optimizer):
    """训练一个 epoch, 返回 (平均损失, 训练准确率, 耗时秒)。"""
    model.train()
    total_loss, correct, total = 0.0, 0, 0
    t0 = time.time()
    for x, y in loader:
        x, y = x.to(DEVICE, non_blocking=True), y.to(DEVICE, non_blocking=True)
        optimizer.zero_grad()
        out = model(x)
        loss = criterion(out, y)
        loss.backward()
        optimizer.step()
        total_loss += loss.item() * y.size(0)
        correct += (out.argmax(1) == y).sum().item()
        total += y.size(0)
    return total_loss / total, correct / total, time.time() - t0


@torch.no_grad()
def evaluate(model, loader, criterion=None, return_preds=False):
    """在数据集上评估, 返回 dict(loss, acc, y_true, y_pred, time)。"""
    model.eval()
    total_loss, correct, total = 0.0, 0, 0
    ys, preds = [], []
    t0 = time.time()
    for x, y in loader:
        x, y = x.to(DEVICE, non_blocking=True), y.to(DEVICE, non_blocking=True)
        out = model(x)
        if criterion is not None:
            total_loss += criterion(out, y).item() * y.size(0)
        correct += (out.argmax(1) == y).sum().item()
        total += y.size(0)
        ys.append(y.cpu())
        preds.append(out.argmax(1).cpu())
    res = {
        "acc": correct / total,
        "y_true": torch.cat(ys).numpy(),
        "y_pred": torch.cat(preds).numpy(),
        "time": time.time() - t0,
    }
    if criterion is not None:
        res["loss"] = total_loss / total
    return res if return_preds else {k: v for k, v in res.items()
                                     if k not in ("y_true", "y_pred")}


def plot_curves(history, title, path):
    """训练/测试损失与准确率曲线。"""
    epochs = range(1, len(history["train_loss"]) + 1)
    fig, axes = plt.subplots(1, 2, figsize=(11, 4))
    axes[0].plot(epochs, history["train_loss"], "o-", label="训练损失")
    axes[0].plot(epochs, history["test_loss"], "s--", label="测试损失")
    axes[0].set_xlabel("Epoch")
    axes[0].set_ylabel("CrossEntropy Loss")
    axes[0].set_title(f"{title} 损失曲线")
    axes[0].legend()
    axes[0].grid(alpha=0.3)
    axes[1].plot(epochs, [a * 100 for a in history["train_acc"]], "o-", label="训练准确率")
    axes[1].plot(epochs, [a * 100 for a in history["test_acc"]], "s--", label="测试准确率")
    axes[1].set_xlabel("Epoch")
    axes[1].set_ylabel("Accuracy (%)")
    axes[1].set_title(f"{title} 准确率曲线")
    axes[1].legend()
    axes[1].grid(alpha=0.3)
    fig.tight_layout()
    fig.savefig(path, dpi=150)
    plt.close(fig)


def plot_confusion_matrix(y_true, y_pred, title, path):
    """混淆矩阵热力图(百分比按真实类归一化)。"""
    from sklearn.metrics import confusion_matrix
    cm = confusion_matrix(y_true, y_pred)
    fig, ax = plt.subplots(figsize=(7.5, 6.5))
    im = ax.imshow(cm, cmap="Purples")
    ax.set_xticks(range(10), CLASS_NAMES_CN, rotation=45, ha="right", fontsize=9)
    ax.set_yticks(range(10), CLASS_NAMES_CN, fontsize=9)
    thresh = cm.max() / 2.0
    for i in range(10):
        for j in range(10):
            if cm[i, j] > 0:
                ax.text(j, i, str(cm[i, j]), ha="center", va="center",
                        color="white" if cm[i, j] > thresh else "black", fontsize=8)
    ax.set_xlabel("预测类别")
    ax.set_ylabel("真实类别")
    ax.set_title(title)
    fig.colorbar(im, ax=ax, fraction=0.046)
    fig.tight_layout()
    fig.savefig(path, dpi=150)
    plt.close(fig)
    return cm


def plot_sample_predictions(model, test_set, title, path, n=12):
    """抽取若干测试样本, 展示 真实/预测 标签。"""
    model.eval()
    idxs = np.random.RandomState(0).choice(len(test_set), n, replace=False)
    xs = torch.stack([test_set[i][0] for i in idxs]).to(DEVICE)
    ys = np.array([test_set[i][1] for i in idxs])
    with torch.no_grad():
        preds = model(xs).argmax(1).cpu().numpy()
    fig, axes = plt.subplots(2, n // 2, figsize=(n * 1.15, 3.4))
    for ax, i, p, t in zip(axes.ravel(), idxs, preds, ys):
        ax.imshow(test_set[i][0].squeeze().numpy(), cmap="gray")
        ok = p == t
        ax.set_title(f"真:{CLASS_NAMES_CN[t]}\n预:{CLASS_NAMES_CN[p]}", fontsize=8,
                     color="green" if ok else "red")
        ax.axis("off")
    fig.suptitle(title, fontsize=12)
    fig.tight_layout()
    fig.savefig(path, dpi=150)
    plt.close(fig)


def save_json(obj, name):
    path = os.path.join(RESULTS_DIR, name)
    with open(path, "w", encoding="utf-8") as f:
        json.dump(obj, f, ensure_ascii=False, indent=2)
    print(f"[saved] {path}")
    return path
