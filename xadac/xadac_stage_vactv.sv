module xadac_stage_vactv
    import xadac_if::*;
(
    input logic  clk,
    input logic  rstn,
    xadac_if.slv slv
);

    // Scoreboard =============================================================

    localparam int unsigned NoEntries = 2**IdWidth;

    typedef struct {
        AddrT   addr;
        BeT     be;
        VectorT wdata;
        logic   req_done;
        logic   obi_a_done;
        logic   obi_r_done;
    } entry_t;

    entry_t [NoEntries-1:0] entries_d, entries_q;

    // Resp Channel ===========================================================

    IdT   resp_id_d;
    logic resp_valid_d;

    assign slv.resp_rd = '0;
    assign slv.resp_vd = '0;

    // OBI A Channel ==========================================================

    logic   obi_req_d;
    AddrT   obi_addr_d;
    BeT     obi_be_d;
    VectorT obi_wdata_d;
    IdT     obi_aid_d;

    assign obi.we = 1;

    // OBI R Channel ==========================================================

    assign obi.rready = 1;

    // ========================================================================

    always_comb begin
        entries_d    = entries_q;

        resp_id_d    = slv.resp_id;
        resp_valid_d = slv.resp_valid;

        obi_req_d    = obi.req;
        obi_addr_d   = obi.addr;
        obi_be_d     = obi.be;
        obi_wdata_d  = obi.wdata;
        obi_aid_d    = obi.aid;

        // OBI R Channel - Complete ===========================================

        if (obi.rvalid && obi.rready) begin
            entries_d[obi.rid].obi_r_done = 1;
        end

        // OBI A Channel - Complete ===========================================

        if (obi.req && obi.gnt) begin
            entries_d[obi.aid].obi_a_done = 1;
            obi_req_d = 0;
        end

        // Resp Channel - Complete ============================================

        if (slv.resp_valid && obi.resp_ready) begin
            entries_d[slv.req_id] = '0;
            resp_valid_d = 0;
        end

        // Req Channel - Complete =============================================

        slv.req_ready = slv.req_valid && !entries_d[slv.req_id].req_done;

        if (slv.req_valid && slv.req_ready) begin
            entries_d[slv.req_id].addr = AddrT'(slv.req_rs1);
            entries_d[slv.req_id].be = (1 << slv.req_imm) - 1;
            automatic VectorT wdata = 0;
            for (ImmT i = 0; i < slv.req_imm; i++) begin
                automatic SumT sum = slv.req_vs3[SumWidth*i +: SumWidth];
                wdata = (sum > 0) ? (sum << slv.req_rs2) : 0;
            end
            entries_d[slv.req_id].wdata = wdata;
            entries_d[slv.req_id].req_done = 1;
        end

        // OBI A Channel - Start ==============================================

        for(IdT id = 0; id < NoEntries; id++) begin
            if(
                !obi_req_d &&
                entries_d[id].req_done &&
                !entries_d[id].obi_a_done
            ) begin
                obi_req_d   = 1;
                obi_addr_d  = entries_d[id].addr;
                obi_be_d    = entries_d[id].be;
                obi_wdata_d = entries_d[id].wdata;
                obi_aid_d   = id;
            end
        end

        // Resp Channel - Start ===============================================

        for(IdT id = 0; id < NoEntries; id++) begin
            if(!resp_valid_d && entries_d[id].obi_r_done) begin
                resp_id_d    = id;
                resp_valid_d = 1;
            end
        end
    end

    always_ff @(posedge clk, negedge rstn) begin
        if (!rstn) begin
            entries_q <= '0;

            slv.resp_id    <= '0;
            slv.resp_valid <= '0;

            obi.req   <= '0;
            obi.addr  <= '0;
            obi.be    <= '0;
            obi.wdata <= '0;
            obi.aid   <= '0;
        end
        else begin
            entries_q <= entries_q;

            slv.resp_id    <= resp_id_d;
            slv.resp_valid <= resp_valid_d;

            obi.req   <= obi_req_d;
            obi.addr  <= obi_addr_d;
            obi.be    <= obi_be_d;
            obi.wdata <= obi_wdata_d;
            obi.aid   <= obi_aid_d;
        end
    end
endmodule
