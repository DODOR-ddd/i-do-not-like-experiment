//=============================================================================
// 文件名   : alu.v
// 模块名   : alu
// 功能     : 32 位多功能算术逻辑运算单元（16 种运算，行为级建模）
//            ALU_OP[3:0] 选择运算功能，并产生零标志 ZF 和溢出标志 OF
// 说明     : 本文件为完整模块，可直接用于 Vivado 仿真 / 虚拟版卡平台上传；
//            在头歌平台上提交时，只需把 /*** Begin ***/ 与 /*** End ***/
//            之间的代码粘贴到题目模板的对应位置即可（端口已由模板给出）。
//=============================================================================
`timescale 1ns / 1ps

module alu (
    input  wire [31:0] A,           // 32位输入A
    input  wire [31:0] B,           // 32位输入B
    input  wire [3:0]  alu_op,      // 4位ALU操作控制信号
    output reg  [31:0] result,      // 32位运算结果
    output wire        ZF,          // 零标志位
    output wire        OF           // 溢出标志位
);

  /********** Begin *********/

  // 移位量：32 位数据的移位只需 5 位，取 B 的低 5 位
  wire [4:0] shamt = B[4:0];

  // 16 选 1 的运算部件：用行为级 case 语句描述，综合后即为多路选择器
  always @(*) begin
    case (alu_op)
      4'b0000: result = A + B;                                       // add  : a+b
      4'b0001: result = A << shamt;                                  // sll  : a 逻辑左移
      4'b0010: result = ($signed(A) < $signed(B)) ? 32'd1 : 32'd0;   // slt  : 有符号比较
      4'b0011: result = (A < B) ? 32'd1 : 32'd0;                     // sltu : 无符号比较
      4'b0100: result = A ^ B;                                       // xor
      4'b0101: result = A >> shamt;                                  // srl  : 逻辑右移
      4'b0110: result = A | B;                                       // or
      4'b0111: result = A & B;                                       // and
      4'b1000: result = A - B;                                       // sub  : a-b
      4'b1001: result = A + 32'd1;                                   // inca : a+1
      4'b1010: result = A;                                           // outa : 输出 a
      4'b1011: result = B + 32'd1;                                   // incb : b+1
      4'b1100: result = 32'd0;                                       // out0 : 输出 0
      4'b1101: result = $signed(A) >>> shamt;                        // sra  : 算术右移
      default: result = 32'd0;                                       // 1110/1111 保留
    endcase
  end

  // 零标志：运算结果全零时 ZF=1，否则 ZF=0（对全部 16 种运算均有效）
  assign ZF = (result == 32'd0);

  // 溢出标志：只对算术运算有效，有符号结果超出 32 位补码表示范围时 OF=1
  //   add : 同号相加，结果符号与被加数不同
  //   sub : 异号相减（a-b 即 a+(-b)），结果符号与被减数不同
  //   inc : 正数加 1 后符号位变成 1（0x7FFF_FFFF + 1）
  wire add_ovf  = (A[31] == B[31])   && (result[31] != A[31]);
  wire sub_ovf  = (A[31] != B[31])   && (result[31] != A[31]);
  wire inca_ovf = (A[31] == 1'b0)    && (result[31] == 1'b1);
  wire incb_ovf = (B[31] == 1'b0)    && (result[31] == 1'b1);

  assign OF = (alu_op == 4'b0000) ? add_ovf  :
              (alu_op == 4'b1000) ? sub_ovf  :
              (alu_op == 4'b1001) ? inca_ovf :
              (alu_op == 4'b1011) ? incb_ovf : 1'b0;

  /********** End *********/

endmodule
