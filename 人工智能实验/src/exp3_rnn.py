# -*- coding: utf-8 -*-
"""实验3: 时序神经网络 (RNN / GRU) 分类 FashionMNIST。

把每张 28x28 图像看成 28 个时间步的序列 (自上而下的 28 行, 每行 28 维),
用循环网络建模后取最终隐状态做十分类。为便于对比, 同时训练:
  - vanilla RNN (2层, hidden=128)
  - GRU (2层, hidden=128)
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


class SeqClassifier(nn.Module):
    """将 28x28 图像作为 28 个时间步序列的循环网络分类器。"""

    def __init__(self, rnn_type="gru", hidden=128, num_layers=2, num_classes=10):
        super().__init__()
        self.rnn_type = rnn_type
        if rnn_type == "rnn":
            self.rnn = nn.RNN(28, hidden, num_layers,
                              batch_first=True, dropout=0.3)
        elif rnn_type == "gru":
            self.rnn = nn.GRU(28, hidden, num_layers,
                              batch_first=True, dropout=0.3)
        elif rnn_type == "lstm":
            self.rnn = nn.LSTM(28, hidden, num_layers,
                               batch_first=True, dropout=0.3)
        else:
            raise ValueError(rnn_type)
        self.dropout = nn.Dropout(0.3)
        self.fc = nn.Linear(hidden, num_classes)

    def forward(self, x):
        # x: (B,1,28,28) -> (B,28,28) 序列: 28 个时间步, 每步 28 维(一行像素)
        x = x.squeeze(1)
        out, _ = self.rnn(x)
        h_last = out[:, -1, :]              # 最后一个时间步的隐状态
        return self.fc(self.dropout(h_last))


def run_one(rnn_type, train_loader, test_loader, test_set, n_epochs=5):
    print(f"\n---- 训练 {rnn_type.upper()} (hidden=128, layers=2) ----")
    model = SeqClassifier(rnn_type).to(DEVICE)
    n_params = sum(p.numel() for p in model.parameters())
    print(f"参数量: {n_params:,}")

    criterion = nn.CrossEntropyLoss()
    optimizer = torch.optim.Adam(model.parameters(), lr=1e-3)
    scheduler = torch.optim.lr_scheduler.CosineAnnealingLR(optimizer, T_max=n_epochs)

    history = {"train_loss": [], "train_acc": [], "test_loss": [],
               "test_acc": [], "epoch_time": []}
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
            torch.save(model.state_dict(), os.path.join(RESULTS_DIR, f"{rnn_type}_best.pt"))
            star = "  <- best"
        print(f"Epoch {ep}/{n_epochs} ({t:.1f}s)  "
              f"train loss {tr_loss:.4f} acc {tr_acc*100:.2f}%  |  "
              f"test loss {ev['loss']:.4f} acc {ev['acc']*100:.2f}%{star}")
    total_time = sum(history["epoch_time"])

    ev = evaluate(model, test_loader, criterion, return_preds=True)
    print(f"{rnn_type.upper()} 最终测试准确率: {ev['acc']*100:.2f}%  "
          f"(最佳 {best_acc*100:.2f}%),  推理 {ev['time']:.2f}s/10000张")
    report = classification_report(ev["y_true"], ev["y_pred"], digits=4, output_dict=True)
    print(classification_report(ev["y_true"], ev["y_pred"], digits=4))

    tag = {"rnn": "RNN", "gru": "GRU", "lstm": "LSTM"}[rnn_type]
    plot_curves(history, tag, os.path.join(RESULTS_DIR, f"{rnn_type}_curves.png"))
    plot_confusion_matrix(ev["y_true"], ev["y_pred"],
                          f"{tag} 混淆矩阵 (测试集, acc={ev['acc']*100:.2f}%)",
                          os.path.join(RESULTS_DIR, f"{rnn_type}_confusion.png"))
    plot_sample_predictions(model, test_set, f"{tag} 测试样本预测示例",
                            os.path.join(RESULTS_DIR, f"{rnn_type}_predictions.png"))
    print(f"[saved] {rnn_type}_curves.png / {rnn_type}_confusion.png / {rnn_type}_predictions.png")

    return {
        "model": f"{tag} hidden=128 layers=2",
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
        "total_train_time_sec": round(total_time, 1),
    }


def main():
    setup_font()
    seed_everything(42)
    print("== 实验三: RNN/GRU 时序神经网络分类 FashionMNIST ==")
    print(f"设备: {DEVICE}")
    train_loader, test_loader, _, test_set = load_fashion_mnist()

    results = {}
    for rnn_type in ("rnn", "gru"):
        results[rnn_type] = run_one(rnn_type, train_loader, test_loader, test_set)

    # ---- 对比图: RNN vs GRU 准确率曲线 ----
    import matplotlib.pyplot as plt
    epochs = range(1, 6)
    fig, ax = plt.subplots(figsize=(7, 4.5))
    ax.plot(epochs, [a * 100 for a in results["rnn"]["history"]["test_acc"]],
            "o-", label="RNN")
    ax.plot(epochs, [a * 100 for a in results["gru"]["history"]["test_acc"]],
            "s-", label="GRU")
    ax.set_xlabel("Epoch")
    ax.set_ylabel("测试准确率 (%)")
    ax.set_title("RNN vs GRU 测试准确率对比")
    ax.grid(alpha=0.3)
    ax.legend()
    fig.tight_layout()
    fig.savefig(os.path.join(RESULTS_DIR, "rnn_gru_compare.png"), dpi=150)
    plt.close(fig)
    print("[saved] rnn_gru_compare.png")

    print(f"\nRNN 最终: {results['rnn']['final_test_acc']*100:.2f}%  "
          f"GRU 最终: {results['gru']['final_test_acc']*100:.2f}%")
    save_json({
        "experiment": "RNN/GRU 时序神经网络分类 FashionMNIST",
        "sequence": "28个时间步 x 每步28维(逐行扫描图像)",
        "results": results,
    }, "exp3_rnn_metrics.json")
    print("实验三完成")


if __name__ == "__main__":
    main()
