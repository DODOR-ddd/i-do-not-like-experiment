// 实验03 通用寄存器堆设计（虚拟版卡平台顶层）
// top 负责板卡接口适配并例化单个 register_file 模块
// 网页输入框为 32 位（8 位十六进制），地址类信号取低 5 位
module top (
    input         clock,            // CLK 绑定 clock
    input         reset,            // SW00 高电平复位
    input         reg_wen,          // SW01 写使能
    input  [31:0] read_addr1,       // INPUT0 读地址 1，取低 5 位
    input  [31:0] read_addr2,       // INPUT1 读地址 2，取低 5 位
    input  [31:0] write_addr,       // INPUT2 写地址，取低 5 位
    input  [31:0] write_data,       // INPUT3 写入数据
    output [31:0] read_data1,       // OUTPUT0 读端口 1 数据
    output [31:0] read_data2,       // OUTPUT1 读端口 2 数据
    output [31:0] write_addr_used,  // OUTPUT2 实际写地址
    output        write_enabled     // L00 允许写入非零寄存器
);

    // 地址均取低 5 位（寄存器堆只有 32 个寄存器，5 位编址）
    wire [4:0] raddr1 = read_addr1[4:0];
    wire [4:0] raddr2 = read_addr2[4:0];
    wire [4:0] waddr  = write_addr[4:0];

    // 实际写地址：低 5 位零扩展为 32 位显示
    assign write_addr_used = {27'b0, waddr};

    // 允许写入非零寄存器：写使能有效且目标不是 x0
    assign write_enabled = reg_wen && (waddr != 5'd0);

    // 例化通用寄存器堆
    register_file u_register_file (
        .clock      (clock),
        .reset      (reset),
        .src1_raddr (raddr1),
        .src2_raddr (raddr2),
        .reg_waddr  (waddr),
        .reg_wdata  (write_data),
        .reg_wen    (reg_wen),
        .src1_rdata (read_data1),
        .src2_rdata (read_data2)
    );

endmodule
