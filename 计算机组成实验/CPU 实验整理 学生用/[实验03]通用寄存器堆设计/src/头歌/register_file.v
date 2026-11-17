// 实验03 通用寄存器堆设计（头歌在线编程）
// 32×32 通用寄存器堆：2 个读端口（组合读）+ 1 个写端口（时钟上升沿写入）
// 行为级建模：寄存器存储、读写和复位逻辑均由本模块实现
module register_file (
    input         clock,        // 时钟，上升沿写入
    input         reset,        // 高电平异步复位
    input  [4:0]  src1_raddr,   // 读端口 1 地址
    input  [4:0]  src2_raddr,   // 读端口 2 地址
    input  [4:0]  reg_waddr,    // 写地址
    input  [31:0] reg_wdata,    // 写入数据
    input         reg_wen,      // 写使能
    output [31:0] src1_rdata,   // 读端口 1 数据
    output [31:0] src2_rdata    // 读端口 2 数据
);

    // 32 个 32 位寄存器构成的寄存器堆
    reg [31:0] regs [31:0];

    integer i;

    // 写入与复位：异步高电平复位（无需 clock 参与）；
    // 否则在 clock 上升沿且写使能有效时写入
    always @(posedge clock or posedge reset) begin
        if (reset) begin
            for (i = 0; i < 32; i = i + 1)
                regs[i] <= 32'b0;               // 复位将 32 个寄存器全部清零
        end
        else if (reg_wen && reg_waddr != 5'd0) begin
            regs[reg_waddr] <= reg_wdata;       // x0 不允许写入，恒为零
        end
    end

    // 组合读：没有使能或时钟信号控制，给出寄存器地址即可读出数据
    // regs[0] 从未被写入且复位后为 0，故读 x0 恒得 0
    assign src1_rdata = regs[src1_raddr];
    assign src2_rdata = regs[src2_raddr];

endmodule
