# -*- coding: utf-8 -*-
"""实验2: 浅层卷积神经网络 CNN 分类 FashionMNIST。

网络结构 (两个卷积块 + 两层全连接):
  Conv(1->32,3x3,p1)-BN-ReLU -> MaxPool2x2   (28x28 -> 14x14)
  Conv(32->64,3x3,p1)-BN-ReLU -> MaxPool2x2  (14x14 -> 7x7)
  Flatten(3136) -> FC(128)-ReLU-Dropout(0.5) -> FC(10)
训练: Adam lr=1e-3, batch=128, 5 epochs。
"""
import os
import sys
import time

import torch
from torch import nn

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from common import (DEVICE, RESULTS_DIR, evaluate, load_fashion_mnist,
                    plot_confusion_matrix, plot_curves, plot_sample_predictions,
                    save_json, seed_everything, setup_font, train_one_epoch)
from sklearn.metrics import classification_report


class ShallowCNN(nn.Module):
    """浅层 CNN: 2 个卷积块 + 2 层全连接。"""

    def __init__(self, num_classes=10):
        super().__init__()
        self.features = nn.Sequential(
            nn.Conv2d(1, 32, kernel_size=3, padding=1),   # 1x28x28 -> 32x28x28
            nn.BatchNorm2d(32),
            nn.ReLU(inplace=True),
            nn.MaxPool2d(2),                              # 32x14x14
            nn.Conv2d(32, 64, kernel_size=3, padding=1),  # 64x14x14
            nn.BatchNorm2d(64),
            nn.ReLU(inplace=True),
            nn.MaxPool2d(2),                              # 64x7x7
        )
        self.classifier = nn.Sequential(
            nn.Flatten(),
            nn.Linear(64 * 7 * 7, 128),
            nn.ReLU(inplace=True),
            nn.Dropout(0.5),
            nn.Linear(128, num_classes),
        )

    def forward(self, x):
        return self.classifier(self.features(x))


def main():
    setup_font()
    seed_everything(42)
    print("== 实验二: 浅层 CNN 分类 FashionMNIST ==")
    print(f"设备: {DEVICE}")
    train_loader, test_loader, _, test_set = load_fashion_mnist()

    model = ShallowCNN().to(DEVICE)
    n_params = sum(p.numel() for p in model.parameters())
    print(model)
    print(f"参数量: {n_params:,}")

    criterion = nn.CrossEntropyLoss()
    optimizer = torch.optim.Adam(model.parameters(), lr=1e-3)
    scheduler = torch.optim.lr_scheduler.CosineAnnealingLR(optimizer, T_max=5)

    history = {"train_loss": [], "train_acc": [], "test_loss": [],
               "test_acc": [], "epoch_time": []}
    n_epochs = 5
    best_acc = 0.0
    for ep in range(1, n_epochs + 1):
        tr_loss, tr_acc, t = train_one_epoch(model, train_loader, criterion, optimizer)
        scheduler.step()
        ev = evaluate(model, test_loader, criterion)
        history["train_loss"].append(tr_loss)
        history["train_acc"].append(tr_acc)
        history["test_loss"].append(ev["loss"])
        history["test_acc"].append(ev["acc"])
        history["epoch_time"].append(t)
        star = ""
        if ev["acc"] > best_acc:
            best_acc = ev["acc"]
            torch.save(model.state_dict(), os.path.join(RESULTS_DIR, "shallow_cnn_best.pt"))
            star = "  <- best"
        print(f"Epoch {ep}/{n_epochs} ({t:.1f}s)  "
              f"train loss {tr_loss:.4f} acc {tr_acc*100:.2f}%  |  "
              f"test loss {ev['loss']:.4f} acc {ev['acc']*100:.2f}%{star}")
    total_train_time = sum(history["epoch_time"])

    # ---- 测试集详细评估 ----
    ev = evaluate(model, test_loader, criterion, return_preds=True)
    print(f"\n最终测试集准确率: {ev['acc']*100:.2f}%  (最佳 epoch 准确率 {best_acc*100:.2f}%)")
    print(f"测试集推理耗时: {ev['time']:.2f}s / 10000 张")
    report = classification_report(ev["y_true"], ev["y_pred"],
                                   target_names=None, digits=4, output_dict=True)
    print(classification_report(ev["y_true"], ev["y_pred"], digits=4))

    # ---- 图1: 曲线 ----
    plot_curves(history, "浅层CNN", os.path.join(RESULTS_DIR, "cnn_curves.png"))
    print("[saved] cnn_curves.png")
    # ---- 图2: 混淆矩阵 ----
    plot_confusion_matrix(ev["y_true"], ev["y_pred"],
                          f"浅层CNN 混淆矩阵 (测试集, acc={ev['acc']*100:.2f}%)",
                          os.path.join(RESULTS_DIR, "cnn_confusion.png"))
    print("[saved] cnn_confusion.png")
    # ---- 图3: 样本预测展示 ----
    plot_sample_predictions(model, test_set, "浅层CNN 测试样本预测示例",
                            os.path.join(RESULTS_DIR, "cnn_predictions.png"))
    print("[saved] cnn_predictions.png")

    save_json({
        "experiment": "浅层CNN 分类 FashionMNIST",
        "model": "Conv32-Conv64-FC128-FC10 (含BatchNorm/Dropout)",
        "params": n_params,
        "epochs": n_epochs,
        "optimizer": "Adam(lr=1e-3, CosineAnnealingLR)",
        "batch_size": 128,
        "history": history,
        "final_test_acc": ev["acc"],
        "best_test_acc": best_acc,
        "final_test_loss": ev["loss"],
        "per_class_acc": {f"class_{i}": report[str(i)]["recall"] for i in range(10)},
        "macro_f1": report["macro avg"]["f1-score"],
        "total_train_time_sec": round(total_train_time, 1),
    }, "exp2_cnn_metrics.json")

    print(f"实验二完成, 训练总耗时 {total_train_time:.1f}s")


if __name__ == "__main__":
    main()
