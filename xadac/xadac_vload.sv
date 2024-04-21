module xadac_vload
    import xadac_pkg::*;
(
    input logic     clk,
    input logic     rstn,
    xadac_if.slv    slv,
    OBI_BUS.Manager obi
);

    typedef struct packed {
        AddrT    addr;
        VecAddrT vd_addr;
        VecLenT  vlen;
        VecDataT rdata;
        logic    exe_req_done;
        logic    exe_rsp_done;
        logic    obi_a_done;
        logic    obi_r_done;
    } entry_t;

    entry_t [SbLen-1:0] sb_d, sb_q;

    ExeRspT exe_rsp_d;
    logic   exe_rsp_valid_d;

    logic obi_req_d;
    AddrT obi_addr_d;
    IdT   obi_aid_d;

    assign obi.we     = '0;
    assign obi.be     = '0;
    assign obi.wdata  = '0;
    assign obi.rready = '1;

    always_comb begin : comb
        automatic IdT id;

        sb_d    = sb_q;

        exe_rsp_d       = slv.exe_rsp;
        exe_rsp_valid_d = slv.exe_rsp_valid;

        obi_req_d  = obi.req;
        obi_addr_d = obi.addr;
        obi_aid_d  = obi.aid;

        // dec ================================================================

        slv.dec_rsp_valid = slv.dec_req_valid;
        slv.dec_req_ready = (slv.dec_rsp_valid && slv.dec_rsp_ready);

        slv.dec_rsp.id = slv.dec_req.id;
        slv.dec_rsp.rd_clobber = '0;
        slv.dec_rsp.vd_clobber = '1;
        slv.dec_rsp.rs_read[0] = '1;
        slv.dec_rsp.rs_read[1] = '0;
        slv.dec_rsp.vs_read[0] = '0;
        slv.dec_rsp.vs_read[1] = '0;
        slv.dec_rsp.vs_read[2] = '0;
        slv.dec_rsp.accept = '1;

        // obi r channel - end ================================================

        id = obi.rid;

        if (obi.rvalid && obi.rready) begin
            sb_d[id].data       = obi.rdata;
            sb_d[id].obi_r_done = '1;
        end

        // obi a channel - end ================================================

        id = obi.aid;

        if (obi.req && obi.gnt) begin
            obi_req_d           = '0;
            obi_addr_d          = '0;
            obi_aid_d           = '0;
            sb_d[id].obi_a_done = '1;
        end

        // exe rsp channel - end ==============================================

        id = slv.exe_rsq.id;

        if (slv.exe_rsp_valid && obi.exe_rsp_ready) begin
            exe_rsp_d             = '0;
            exe_rsp_valid_d       = '0;
            sb_d[id].exe_rsp_done = '1;
        end

        // exe req channel ====================================================

        id = slv.exe_req.id;

        slv.exe_req_ready = (slv.exe_req_valid && !sb_d[id].exe_req_done);

        if (slv.exe_req_valid && slv.exe_req_ready) begin
            sb_d[id].addr     = AddrT'(slv.req_rs1);
            sb_d[id].vd_addr  = slv.exe_req.instr[11:7];
            sb_d[id].vlen     = slv.exe_req.instr[25 +: VecLenWidth];
            sb_d[id].req_done = '1;
        end

        // obi a channel - start ==============================================

        for(id = 0; id < SbLen; id++) begin
            if(!obi_req_d && sb_d[id].req_done && !sb_d[id].obi_a_done) begin
                obi_req_d  = '1;
                obi_addr_d = sb_d[id].addr;
                obi_aid_d  = id;
            end
        end

        // exe rsp channel - start ============================================

        for(id = 0; id < SbLen; id++) begin
            if(
                !exe_rsp_valid_d &&
                sb_d[id].obi_r_done &&
                !sb_d[id].exe_rsp_done
            ) begin

                automatic VecDataT vd_data = '0;
                for (VecLenT i = 0; i < VectorWidth/ElemWidth; i++) begin
                    automatic VecLenT j = (i % sb_d[id].vlen);
                    vd_data[VecElemWidth*i +: VecElemWidth] =
                        sb_d[id].rdata[VecElemWidth*j +: VecElemWidth];
                end

                exe_rsp_d          = '0;
                exe_rsp_d.id       = id;
                exe_rsp_d.vd_addr  = sb_d[id].vd_addr;
                exe_rsp_d.vd_data  = vd_data;
                exe_rsp_d.vd_write = '1;

                exe_rsp_valid_d = '1;
            end
        end

        // end ================================================================

        for (id = 0; id < SbLen; id++) begin
            if (
                sb_d[id].exe_req_done &&
                sb_d[id].exe_rsp_done &&
                sb_d[id].obi_a_done &&
                sb_d[id].obi_r_done
            ) begin
                sb_d[id] = '0;
            end
        end
    end

    always_ff @(posedge clk, negedge rstn) begin
        if (!rstn) begin
            sb_q <= '0;

            slv.exe_rsp       <= '0;
            slv.exe_rsp_valid <= '0;

            obi.req  <= '0;
            obi.addr <= '0;
            obi.aid  <= '0;
        end
        else begin
            sb_q <= sb_q;

            slv.exe_rsp       <= exe_rsp_d;
            slv.exe_rsp_valid <= exe_rsp_valid_d;

            obi.req  <= obi_req_d;
            obi.addr <= obi_addr_d;
            obi.aid  <= obi_aid_d;
        end
    end

endmodule
