#!/usr/bin/env python3
"""
usage: python3 caesar_gui.py
"""

import sys

from PyQt6.QtCore import Qt
from PyQt6.QtGui import QFont
from PyQt6.QtWidgets import (
    QApplication,
    QLabel,
    QPlainTextEdit,
    QPushButton,
    QSpinBox,
    QVBoxLayout,
    QHBoxLayout,
    QWidget,
)


def caesar(text: str, offset: int) -> str:
    offset %= 26
    out = []
    for ch in text:
        if "a" <= ch <= "z":
            out.append(chr((ord(ch) - ord("a") + offset) % 26 + ord("a")))
        elif "A" <= ch <= "Z":
            out.append(chr((ord(ch) - ord("A") + offset) % 26 + ord("A")))
        else:
            out.append(ch)
    return "".join(out)


class CaesarWindow(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("凯撒密码 加密/解密器")
        self.resize(880, 520)
        self._build_ui()

    def _build_ui(self):
        root = QVBoxLayout(self)
        key_row = QHBoxLayout()
        key_row.addStretch(1)
        key_row.addWidget(QLabel("移位数 k:"))
        self.shift_spin = QSpinBox()
        self.shift_spin.setRange(0, 25)
        self.shift_spin.setValue(0)
        key_row.addWidget(self.shift_spin)
        key_row.addStretch(1)
        root.addLayout(key_row)


        io_row = QHBoxLayout()

        io_row.addWidget(QLabel("输入（明文 / 密文）"))
        self.input_edit = QPlainTextEdit()
        self.input_edit.setPlaceholderText("在此输入要加密或解密的文本…")
        io_row.addWidget(self.input_edit, stretch=1)

        btn_col = QVBoxLayout()
        btn_col.addStretch(1)
        enc_btn = QPushButton("加密 →")
        dec_btn = QPushButton("← 解密")
        swap_btn = QPushButton("⇅ 交换输入/输出")
        enc_btn.clicked.connect(self.encrypt)
        dec_btn.clicked.connect(self.decrypt)
        swap_btn.clicked.connect(self.swap_io)
        for b in (enc_btn, dec_btn, swap_btn):
            btn_col.addWidget(b)
        btn_col.addStretch(1)
        io_row.addLayout(btn_col)

        out_col = QVBoxLayout()
        out_col.addWidget(QLabel("输出"))
        self.output_edit = QPlainTextEdit()
        self.output_edit.setReadOnly(True)
        self.output_edit.setFont(QFont("Monospace"))
        out_col.addWidget(self.output_edit, stretch=1)
        io_row.addLayout(out_col)

        root.addLayout(io_row, stretch=1)


        tool_row = QHBoxLayout()
        copy_btn = QPushButton("复制结果")
        clear_btn = QPushButton("清空")
        copy_btn.clicked.connect(self.copy_output)
        clear_btn.clicked.connect(self.clear_all)
        tool_row.addStretch(1)
        tool_row.addWidget(copy_btn)
        tool_row.addWidget(clear_btn)
        root.addLayout(tool_row)

    def _shift(self) -> int:
        return self.shift_spin.value()

    def encrypt(self):
        self.output_edit.setPlainText(caesar(self.input_edit.toPlainText(), self._shift()))

    def decrypt(self):
        self.output_edit.setPlainText(caesar(self.input_edit.toPlainText(), -self._shift()))

    def swap_io(self):
        self.input_edit.setPlainText(self.output_edit.toPlainText())

    def copy_output(self):
        QApplication.clipboard().setText(self.output_edit.toPlainText())

    def clear_all(self):
        self.input_edit.clear()
        self.output_edit.clear()


def main():
    app = QApplication(sys.argv)
    win = CaesarWindow()
    win.show()
    sys.exit(app.exec())


if __name__ == "__main__":
    main()
