module xadac_vactv
    import xadac_if::*;
(
    input logic     clk,
    input logic     rstn,
    xadac_if.slv    slv,
    OBI_BUS.Manager obi
);

    typedef struct packed {
        AddrT   addr;
        VectorT wdata;
        BeT     be;
        logic   exe_req_done;
        logic   exe_rsp_done;
        logic   obi_a_done;
        logic   obi_r_done;
    } entry_t;

    entry_t [SbLen-1:0] sb_d, sb_q;

    ExeRspT exe_rsp_d;
    logic   exe_rsp_valid_d;

    logic   obi_req_d;
    AddrT   obi_addr_d;
    BeT     obi_be_d;
    VectorT obi_wdata_d;
    IdT     obi_aid_d;

    assign obi.we     = '1;
    assign obi.rready = '1;

    always_comb begin
        automatic IdT id;

        sb_d    = sb_q;

        exe_rsp_d       = slv.exe_rsp;
        exe_rsp_valid_d = slv.exe_rsp_valid;

        obi_req_d    = obi.req;
        obi_addr_d   = obi.addr;
        obi_be_d     = obi.be;
        obi_wdata_d  = obi.wdata;
        obi_aid_d    = obi.aid;

        // dec ================================================================

        slv.dec_rsp_valid = slv.dec_req_valid;
        slv.dec_req_ready = (slv.dec_rsp_valid && slv.dec_rsp_ready);

        slv.dec_rsp.id = slv.dec_req.id;
        slv.dec_rsp.rd_clobber = '0;
        slv.dec_rsp.vd_clobber = '0;
        slv.dec_rsp.rs_read[0] = '1;
        slv.dec_rsp.rs_read[1] = '0;
        slv.dec_rsp.vs_read[0] = '0;
        slv.dec_rsp.vs_read[1] = '1;
        slv.dec_rsp.accept = '1;

        // obi r channel ======================================================

        id = obi.rid;

        if (obi.rvalid && obi.rready) begin
            sb_d[id].obi_r_done = 1;
        end

        // obi a channel - end ================================================

        id = obi.aid;

        if (obi.req && obi.gnt) begin
            obi_req_d           = '0;
            sb_d[id].obi_a_done = '1;
        end

        // exe rsp channel - end ==============================================

        id = slv.exe_rsp.id;

        if (slv.exe_rsp_valid && slv.exe_rsp_ready) begin
            resp_valid_d          = '0;
            sb_d[id].exe_rsp_done = '1;
        end

        // exe req channel ====================================================

        id = slv.exe_req.id;

        slv.req_ready = (slv.req_valid && !sb_d[id].exe_req_done);

        if (slv.exe_req_valid && slv.exe_req_ready) begin
            automatic VecLenT   vlen;
            automatic RegDataT  shift;
            automatic VecDataT  wdata;
            automatic VecSumT   sum;
            automatic VecElemT  elem;

            shift = RegDataT'(slv.exe_req.rs_data[1]);
            vlen  = slv.exe_req.instr[25 +: VecLenWidth];

            wdata = '0;
            for (VecLenT i = 0; i < vlen; i++) begin
                sum = slv.exe_req.vs_data[2][SumWidth*i +: SumWidth];
                elem = (sum > 0) ? (sum >> shift) : 0;
                wdata[ElemWidth*i +: ElemWidth] = elem;
            end

            sb_d[id].addr  = AddrT'(slv.exe_req.rs_data[0]);
            sb_d[id].be    = BeT'((1 << vlen) - 1);
            sb_d[id].wdata = wdata;

            sb_d[id].exe_req_done = '1;
        end

        // obi a channel - start ==============================================

        for(IdT id = 0; id < SbLen; id++) begin
            if(
                !obi_req_d &&
                sb_d[id].req_done &&
                !sb_d[id].obi_a_done
            ) begin
                obi_req_d   = '1;
                obi_addr_d  = sb_d[id].addr;
                obi_be_d    = sb_d[id].be;
                obi_wdata_d = sb_d[id].wdata;
                obi_aid_d   = id;
            end
        end

        // exe rsp channel - start ===============================================

        for(IdT id = 0; id < SbLen; id++) begin
            if(
                !resp_valid_d &&
                sb_d[id].obi_r_done &&
                !sb_d[id].exe_rsp_done
            ) begin
                exe_rsp_d    = '0;
                exe_rsp_d.id = id;

                resp_valid_d = '1;
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

            obi.req   <= '0;
            obi.addr  <= '0;
            obi.be    <= '0;
            obi.wdata <= '0;
            obi.aid   <= '0;
        end
        else begin
            sb_q <= sb_q;

            slv.exe_rsp       <= exe_rsp_d;
            slv.exe_rsp_valid <= exe_rsp_valid_d;

            obi.req   <= obi_req_d;
            obi.addr  <= obi_addr_d;
            obi.be    <= obi_be_d;
            obi.wdata <= obi_wdata_d;
            obi.aid   <= obi_aid_d;
        end
    end
endmodule
