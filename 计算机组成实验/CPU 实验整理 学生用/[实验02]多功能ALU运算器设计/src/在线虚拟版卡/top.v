// 实验02 多功能ALU运算器设计（虚拟版卡平台顶层）
// top 负责板卡接口适配并例化单个 alu 模块；运算与标志逻辑集中在 alu.v
// ALU 为纯组合逻辑，clock 仅为平台信号绑定保留，不参与运算
module top (
    input         clock,           // CLK 绑定 clock，ALU 不使用此信号
    input  [31:0] A,               // INPUT0 操作数 A
    input  [31:0] B,               // INPUT1 操作数 B
    input  [31:0] operation,       // INPUT2 低 4 位选择 ALU 操作
    output [31:0] result,          // OUTPUT0 运算结果
    output [31:0] operation_used,  // OUTPUT1 实际采用的操作码
    output        ZF,              // L00 零标志
    output        OF               // L01 溢出标志
);

    // 操作码取低 4 位（如 INPUT2=0000000D 表示 sra）
    wire [3:0] op = operation[3:0];

    // 实际采用的操作码：低 4 位零扩展为 32 位显示
    assign operation_used = {28'b0, op};

    // 例化多功能 ALU
    alu u_alu (
        .A      (A),
        .B      (B),
        .alu_op (op),
        .result (result),
        .ZF     (ZF),
        .OF     (OF)
    );

endmodule
