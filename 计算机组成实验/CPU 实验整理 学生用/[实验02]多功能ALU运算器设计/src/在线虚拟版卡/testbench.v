`timescale 1ns / 1ps

// 实验02 虚拟版卡仿真激励（仅用于 Vivado 行为仿真，不上传到虚拟版卡平台）
// 仿真顶层：tb_top。16 种运算逐个测试，每种运算至少 2 组典型操作数，
// 覆盖：加减溢出/回绕、结果为零、有符号与无符号比较差异、移位边界、保留码默认输出；
// 其中第 1、2、21、32 组与《端口与引脚规范》"操作示例"一致。
// 自动比对结果/零标志/溢出标志，全部通过打印 ALL TESTS PASSED。
module tb_top;

    reg         clock;
    reg  [31:0] A;
    reg  [31:0] B;
    reg  [31:0] operation;
    wire [31:0] result;
    wire [31:0] operation_used;
    wire        ZF;
    wire        OF;

    integer errors;

    // 例化被测顶层
    top u_top (
        .clock          (clock),
        .A              (A),
        .B              (B),
        .operation      (operation),
        .result         (result),
        .operation_used (operation_used),
        .ZF             (ZF),
        .OF             (OF)
    );

    // 平台 CLK 绑定保留；ALU 为组合逻辑，功能不依赖该时钟
    initial clock = 1'b0;
    always #10 clock = ~clock;

    // 波形（Vivado 中可在 Wave 窗口查看，此处另存 vcd 便于离线分析）
    initial begin
        $dumpfile("tb_top.vcd");
        $dumpvars(0, tb_top);
    end

    // 单组测试：施加输入，每组持续 40ns 便于波形观察，超时前自动比对
    task run_test(
        input [31:0]  tA,
        input [31:0]  tB,
        input [3:0]   t_op,
        input [31:0]  exp_result,
        input         exp_ZF,
        input         exp_OF,
        input [255:0] name
    );
        begin
            A         = tA;
            B         = tB;
            operation = {28'b0, t_op};
            #40;
            if (result !== exp_result || ZF !== exp_ZF || OF !== exp_OF ||
                operation_used !== {28'b0, t_op}) begin
                errors = errors + 1;
                $display("[%0t] FAIL %0s", $time, name);
                $display("       result=%h(exp=%h) ZF=%b(exp=%b) OF=%b(exp=%b)",
                         result, exp_result, ZF, exp_ZF, OF, exp_OF);
            end
            else begin
                $display("[%0t] PASS %0s : result=%h ZF=%b OF=%b",
                         $time, name, result, ZF, OF);
            end
        end
    endtask

    initial begin
        errors    = 0;
        A         = 32'h0;
        B         = 32'h0;
        operation = 32'h0;

        // ---- op0 add：算术加法 ----
        run_test(32'h00000003, 32'h00000001, 4'h0, 32'h00000004, 1'b0, 1'b0, "add 3+1 (board)");
        run_test(32'h7fffffff, 32'h00000001, 4'h0, 32'h80000000, 1'b0, 1'b1, "add signed ovf (board)");
        run_test(32'h80000000, 32'h80000000, 4'h0, 32'h00000000, 1'b1, 1'b1, "add ovf to zero");
        run_test(32'hffffffff, 32'h00000001, 4'h0, 32'h00000000, 1'b1, 1'b0, "add -1+1 wrap");
        // ---- op1 sll：逻辑左移 ----
        run_test(32'h00000001, 32'h00000004, 4'h1, 32'h00000010, 1'b0, 1'b0, "sll 1<<4");
        run_test(32'h80000000, 32'h00000001, 4'h1, 32'h00000000, 1'b1, 1'b0, "sll shifted out");
        // ---- op2 slt：有符号数比较 ----
        run_test(32'hffffffff, 32'h00000000, 4'h2, 32'h00000001, 1'b0, 1'b0, "slt -1<0");
        run_test(32'h00000001, 32'hffffffff, 4'h2, 32'h00000000, 1'b1, 1'b0, "slt 1<-1 false");
        run_test(32'h80000000, 32'h7fffffff, 4'h2, 32'h00000001, 1'b0, 1'b0, "slt min<max");
        // ---- op3 sltu：无符号数比较 ----
        run_test(32'hffffffff, 32'h00000000, 4'h3, 32'h00000000, 1'b1, 1'b0, "sltu max<0 false");
        run_test(32'h00000000, 32'hffffffff, 4'h3, 32'h00000001, 1'b0, 1'b0, "sltu 0<max");
        // ---- op4 xor：逻辑异或 ----
        run_test(32'h12345678, 32'h87654321, 4'h4, 32'h95511559, 1'b0, 1'b0, "xor");
        run_test(32'hffffffff, 32'h00000000, 4'h4, 32'hffffffff, 1'b0, 1'b0, "xor ones");
        run_test(32'hffffffff, 32'hffffffff, 4'h4, 32'h00000000, 1'b1, 1'b0, "xor same zero");
        // ---- op5 srl：逻辑右移 ----
        run_test(32'h80000000, 32'h00000001, 4'h5, 32'h40000000, 1'b0, 1'b0, "srl fill zero");
        run_test(32'hf0000000, 32'h00000004, 4'h5, 32'h0f000000, 1'b0, 1'b0, "srl >>4");
        // ---- op6 or：逻辑或 ----
        run_test(32'hf0f0f0f0, 32'h0f0f0f0f, 4'h6, 32'hffffffff, 1'b0, 1'b0, "or all ones");
        run_test(32'h00000000, 32'h00000000, 4'h6, 32'h00000000, 1'b1, 1'b0, "or zeros");
        // ---- op7 and：逻辑与 ----
        run_test(32'hf0f0f0f0, 32'hffffffff, 4'h7, 32'hf0f0f0f0, 1'b0, 1'b0, "and mask");
        run_test(32'hff00ff00, 32'h00ff00ff, 4'h7, 32'h00000000, 1'b1, 1'b0, "and disjoint zero");
        // ---- op8 sub：算术减法 ----
        run_test(32'h00000003, 32'h00000003, 4'h8, 32'h00000000, 1'b1, 1'b0, "sub 3-3 zero (board)");
        run_test(32'h80000000, 32'h00000001, 4'h8, 32'h7fffffff, 1'b0, 1'b1, "sub INT_MIN-1 ovf");
        run_test(32'h00000005, 32'h00000003, 4'h8, 32'h00000002, 1'b0, 1'b0, "sub 5-3");
        // ---- op9 inca：输出 a+1 ----
        run_test(32'h00000005, 32'h5a5a5a5a, 4'h9, 32'h00000006, 1'b0, 1'b0, "inca 5+1");
        run_test(32'h7fffffff, 32'h5a5a5a5a, 4'h9, 32'h80000000, 1'b0, 1'b1, "inca INT_MAX ovf");
        run_test(32'hffffffff, 32'h5a5a5a5a, 4'h9, 32'h00000000, 1'b1, 1'b0, "inca -1+1 zero");
        // ---- opa outa / opb incb ----
        run_test(32'hdeadbeef, 32'h12345678, 4'ha, 32'hdeadbeef, 1'b0, 1'b0, "outa");
        run_test(32'h00000000, 32'h12345678, 4'ha, 32'h00000000, 1'b1, 1'b0, "outa zero");
        run_test(32'ha5a5a5a5, 32'h00000005, 4'hb, 32'h00000006, 1'b0, 1'b0, "incb 5+1");
        run_test(32'ha5a5a5a5, 32'h7fffffff, 4'hb, 32'h80000000, 1'b0, 1'b1, "incb INT_MAX ovf");
        // ---- opc out0：输出 0 ----
        run_test(32'h12345678, 32'h87654321, 4'hc, 32'h00000000, 1'b1, 1'b0, "out0");
        // ---- opd sra：算术右移 ----
        run_test(32'h80000000, 32'h00000001, 4'hd, 32'hc0000000, 1'b0, 1'b0, "sra sign fill (board)");
        run_test(32'hf0000000, 32'h00000004, 4'hd, 32'hff000000, 1'b0, 1'b0, "sra neg >>4");
        run_test(32'h7fffffff, 32'h00000004, 4'hd, 32'h07ffffff, 1'b0, 1'b0, "sra pos >>4");
        // ---- ope/opf：保留，默认输出 0 ----
        run_test(32'h12345678, 32'h87654321, 4'he, 32'h00000000, 1'b1, 1'b0, "reserved 0xE");
        run_test(32'h12345678, 32'h87654321, 4'hf, 32'h00000000, 1'b1, 1'b0, "reserved 0xF");

        if (errors == 0)
            $display("=== ALL TESTS PASSED ===");
        else
            $display("=== %0d TEST(S) FAILED ===", errors);
        $finish;
    end

    // 仿真超时保护
    initial begin
        #100000;
        $display("=== TIMEOUT: simulation did not finish ===");
        $finish;
    end

endmodule
