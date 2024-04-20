module xadac_stage_vrf
    import xadac_pkg::*;
(
    input logic clk,
    input logic rstn,

    xadac_dec_if.slv dec_slv,
    xadac_dec_if.mst dec_mst,

    xadac_exe_if.slv exe_slv,
    xadac_exe_if.mst exe_mst
);

    // decode passthrough =====================================================

    assign dec_mst.req       = dec_slv.req;
    assign dec_mst.req_valid = dec_slv.req_valid;
    assign dec_slv.req_ready = dec_mst.req_ready;

    assign dec_slv.rsp       = dec_mst.rsp;
    assign dec_slv.rsp_valid = dec_mst.rsp_valid;
    assign dec_mst.rsp_ready = dec_slv.rsp_ready;

    // vrf instance ===========================================================

    RegIdT  [NoVs:0] vrf_rid;
    VectorT [NoVs:0] vrf_rdata;
    RegIdT           vrf_wid;
    VectorT          vrf_wdata;
    logic            vrf_we;

    xadac_vrf i_vrf (
        .clk  (clk),
        .rstn (rstn),

        .rid   (vrf_rid),
        .rdata (vrf_rdata),
        .wid   (vrf_wid),
        .wdata (vrf_wdata),
        .we    (vrf_we)
    );

    // execute channel ========================================================

    always_comb begin : comb_exe_req
        vrf_rid = exe_slv.req.vs_data;

        exe_mst.req.id      = exe_slv.req.id;
        exe_mst.req.instr   = exe_slv.req.instr;
        exe_mst.req.rs_addr = exe_slv.req.rs_addr;
        exe_mst.req.rs_data = exe_slv.req.rs_data;
        exe_mst.req.vs_addr = exe_slv.req.vs_addr;
        exe_mst.req.vs_data = vrf_rdata;

        exe_mst.req_valid = exe_slv.req_valid;
        exe_slv.req_ready = exe_mst.req_ready;
    end

    always_comb begin : comb_exe_rsp
        if (exe_mst.rsp_valid && exe_mst.rsp_ready) begin
            vrf_wid   = exe_mst.rsp.vd_id;
            vrf_wdata = exe_mst.rsp.vd;
            vrf_we    = exe_mst.rsp.vd_write;
        end
        else begin
            vrf_wid   = '0;
            vrf_wdata = '0;
            vrf_we    = '0;
        end

        exe_slv.rsp       = exe_mst.rsp;
        exe_slv.rsp_valid = exe_mst.rsp_valid;
        exe_mst.rsp_ready = exe_slv.rsp_ready;
    end

endmodule
