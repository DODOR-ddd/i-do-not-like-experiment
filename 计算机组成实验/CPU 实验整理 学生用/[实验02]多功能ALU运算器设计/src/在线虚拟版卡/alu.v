// 实验02 多功能ALU运算器设计（虚拟版卡平台）
// 32 位多功能 ALU：16 种运算功能 + ZF/OF 标志位，行为级建模
// 运算功能表与 ALU_OP[3:0] 编码见实验指导书表 5-14，1110~1111 保留（默认输出 0）
module alu (
    input  [31:0] A,          // 操作数 A
    input  [31:0] B,          // 操作数 B，低 5 位为移位量
    input  [3:0]  alu_op,     // 操作选择
    output reg [31:0] result, // 运算结果
    output reg        ZF,     // 零标志：结果全零时为 1
    output reg        OF      // 溢出标志：有符号加、减、自增溢出时为 1
);

    wire signed [31:0] sa = A;  // A 的有符号解释（用于 slt / sra）
    wire signed [31:0] sb = B;  // B 的有符号解释

    always @(*) begin
        OF = 1'b0;              // 只有加、减、自增运算可能溢出
        case (alu_op)
            4'b0000: begin      // add 算术加法：a+b
                result = A + B;
                OF = (A[31] == B[31]) && (result[31] != A[31]);
            end
            4'b0001: result = A << B[4:0];               // sll 逻辑左移
            4'b0010: result = (sa < sb) ? 32'd1 : 32'd0; // slt 有符号数比较
            4'b0011: result = (A  < B ) ? 32'd1 : 32'd0; // sltu 无符号数比较
            4'b0100: result = A ^ B;                     // xor 逻辑异或
            4'b0101: result = A >> B[4:0];               // srl 逻辑右移，高位补 0
            4'b0110: result = A | B;                     // or  逻辑或
            4'b0111: result = A & B;                     // and 逻辑与
            4'b1000: begin      // sub 算术减法：a-b
                result = A - B;
                OF = (A[31] != B[31]) && (result[31] != A[31]);
            end
            4'b1001: begin      // inca 输出 a+1
                result = A + 32'd1;
                OF = (~A[31]) && result[31];
            end
            4'b1010: result = A;                          // outa 输出 a
            4'b1011: begin      // incb 输出 b+1
                result = B + 32'd1;
                OF = (~B[31]) && result[31];
            end
            4'b1100: result = 32'd0;                      // out0 输出 0
            4'b1101: result = sa >>> B[4:0];              // sra 算术右移，高位补符号位
            default: result = 32'd0;                      // 1110~1111 保留，输出 0
        endcase
        ZF = (result == 32'd0); // ZF = ~(|result)
    end

endmodule
