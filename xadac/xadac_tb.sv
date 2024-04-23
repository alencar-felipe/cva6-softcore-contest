`timescale 1ns / 1ps

module xadac_tb;

    import xadac_pkg::*;

    logic clk;
    logic rstn;

    xadac_if mst ();

    AXI_BUS #(
        .AXI_ID_WIDTH   (IdWidth),
        .AXI_ADDR_WIDTH (AddrWidth),
        .AXI_DATA_WIDTH (VecDataWidth),
        .AXI_USER_WIDTH ('0)
    ) axi ();

    xadac dut (
        .clk  (clk),
        .rstn (rstn),
        .slv  (mst),
        .axi  (axi)
    );

    initial begin
        clk = 0;
        forever #10 clk = ~clk;
    end

    initial begin
        rstn = 0;
        #20 rstn = 1;
    end

    initial begin
        mst.dec_req_valid = 1'b0;
        mst.dec_rsp_ready = 1'b0;
        mst.exe_req_valid = 1'b0;
        mst.exe_rsp_ready = 1'b0;

        axi.aw_ready = 1'b0;
        axi.w_ready  = 1'b0;
        axi.b_valid  = 1'b0;

        axi.ar_ready = 1'b0;
        axi.r_valid  = 1'b0;
    end

endmodule
