`timescale 1ns / 1ps

// 实验03 虚拟版卡仿真激励（仅用于 Vivado 行为仿真，不上传到虚拟版卡平台）
// 仿真顶层：tb_top。流程与《端口与引脚规范》中的"读写示例"一一对应，
// 并额外验证地址取低 5 位、双端口同时读。自动比对，全部通过打印 ALL TESTS PASSED。
module tb_top;

    reg         clock;
    reg         reset;
    reg         reg_wen;
    reg  [31:0] read_addr1;
    reg  [31:0] read_addr2;
    reg  [31:0] write_addr;
    reg  [31:0] write_data;
    wire [31:0] read_data1;
    wire [31:0] read_data2;
    wire [31:0] write_addr_used;
    wire        write_enabled;

    integer errors;

    // 例化被测顶层
    top u_top (
        .clock           (clock),
        .reset           (reset),
        .reg_wen         (reg_wen),
        .read_addr1      (read_addr1),
        .read_addr2      (read_addr2),
        .write_addr      (write_addr),
        .write_data      (write_data),
        .read_data1      (read_data1),
        .read_data2      (read_data2),
        .write_addr_used (write_addr_used),
        .write_enabled   (write_enabled)
    );

    // 时钟：周期 20ns，上升沿写入（与网页"传信号"每次切换 CLK 对应）
    initial clock = 1'b0;
    always #10 clock = ~clock;

    // 波形（Vivado 中可在 Wave 窗口查看，此处另存 vcd 便于离线分析）
    initial begin
        $dumpfile("tb_top.vcd");
        $dumpvars(0, tb_top);
    end

    // 结果比对任务：输入在下降沿附近改变、上升沿后 5ns 采样，保证时序清晰
    task check(
        input [31:0]  exp_rd1,
        input [31:0]  exp_rd2,
        input [31:0]  exp_wau,
        input         exp_we,
        input [255:0] name
    );
        begin
            if (read_data1 !== exp_rd1 || read_data2 !== exp_rd2 ||
                write_addr_used !== exp_wau || write_enabled !== exp_we) begin
                errors = errors + 1;
                $display("[%0t] FAIL %0s", $time, name);
                $display("       rd1=%h(exp=%h) rd2=%h(exp=%h) wau=%h(exp=%h) we=%b(exp=%b)",
                         read_data1, exp_rd1, read_data2, exp_rd2,
                         write_addr_used, exp_wau, write_enabled, exp_we);
            end
            else begin
                $display("[%0t] PASS %0s : rd1=%h rd2=%h wau=%h we=%b",
                         $time, name, read_data1, read_data2,
                         write_addr_used, write_enabled);
            end
        end
    endtask

    initial begin
        errors = 0;
        // 初始化：SW01(写使能)=0、SW00(复位)=1，与板卡操作顺序一致
        reset      = 1'b1;
        reg_wen    = 1'b0;
        read_addr1 = 32'h00000000;
        read_addr2 = 32'h00000000;
        write_addr = 32'h00000000;
        write_data = 32'h00000000;

        // 1) 异步复位：reset=1 时无需时钟参与即清零
        #5;
        check(32'h0, 32'h0, 32'h0, 1'b0, "async reset clears all (no clock yet)");
        #10;                        // t=15：经历一个上升沿后撤掉复位
        reset = 1'b0;
        #10;                        // t=25：复位已释放

        // 2) 读写示例(1)：复位后写使能=0，两个读输出均为 0
        read_addr1 = 32'h00000005;  // 读 x5
        read_addr2 = 32'h00000000;  // 读 x0
        #10;                        // t=35：过一个上升沿后采样
        check(32'h0, 32'h0, 32'h0, 1'b0, "after reset, wen=0, rd(x5)=rd(x0)=0");

        // 3) 读写示例(2)：写 x5=12345678，一个上升沿后读出
        write_addr = 32'h00000005;
        write_data = 32'h12345678;
        reg_wen    = 1'b1;          // SW01 置 1
        #20;                        // t=55：经历一个上升沿
        check(32'h12345678, 32'h0, 32'h00000005, 1'b1, "write x5=12345678 then read");

        // 4) 读写示例(3)：写使能=0，改写数据并继续切时钟，x5 保持
        reg_wen    = 1'b0;          // SW01 置 0
        write_data = 32'hA5A5A5A5;  // 修改 INPUT3，但不会写入
        #20;                        // t=75：经历一个上升沿
        check(32'h12345678, 32'h0, 32'h00000005, 1'b0, "wen=0, x5 keeps 12345678");

        // 5) 读写示例(4)：尝试写 x0，写入被禁止（L00 熄灭）
        read_addr1 = 32'h00000000;  // INPUT0 也改为 0
        write_addr = 32'h00000000;  // INPUT2 改为 0
        write_data = 32'hDEADBEEF;
        reg_wen    = 1'b1;
        #20;                        // t=95：经历一个上升沿
        check(32'h0, 32'h0, 32'h0, 1'b0, "write to x0 blocked, L00 off");

        // 6) 附加：地址取低 5 位（0x21 -> 0x01），并双端口同时读
        read_addr1 = 32'h00000021;  // 读地址 1 低 5 位 = 1
        read_addr2 = 32'h00000005;  // 读地址 2 = 5
        write_addr = 32'h00000021;  // 写地址低 5 位 = 1
        write_data = 32'h000000A1;
        #20;                        // t=115：经历一个上升沿
        check(32'h000000A1, 32'h12345678, 32'h00000001, 1'b1,
              "addr low 5 bits: write x1 via 0x21, dual-port read");

        // 结束
        reg_wen = 1'b0;
        #20;
        if (errors == 0)
            $display("=== ALL TESTS PASSED ===");
        else
            $display("=== %0d TEST(S) FAILED ===", errors);
        $finish;
    end

    // 仿真超时保护
    initial begin
        #10000;
        $display("=== TIMEOUT: simulation did not finish ===");
        $finish;
    end

endmodule
