module xadac_stage_clobber
    import xadac_pkg::*;
(
    input logic clk,
    input logic rstn,

    xadac_dec_if.slv dec_slv,
    xadac_dec_if.mst dec_mst,

    xadac_exe_if.slv exe_slv,
    xadac_exe_if.mst exe_mst
);

    // data passthrough =======================================================

    assign dec_mst.req = dec_slv.req;
    assign dec_slv.rsp = dec_mst.rsp;
    assign exe_mst.req = exe_slv.req;
    assign exe_slv.rsp = exe_mst.rsp;

    // control logic ==========================================================

    typedef struct {
        RegIdT vd_id;

        logic  vd_clobber;
        logic  vs1_read;
        logic  vs2_read;
        logic  vs3_read;

        logic  dec_req_done;
        logic  dec_rsp_done;
        logic  exe_req_done;
        logic  exe_rsp_done;
    } entry_t;

    entry_t [SbLen-1:0]  sb_d, sb_q;
    logic   [VrfLen-1:0] clobber_d, clobber_q;

    always_comb begin
        automatic entry_t entry;
        automatic RegIdT  vs1_id, vs2_id, vs3_id;

        instr_sb_d = instr_sb_q;
        reg_sb_d   = reg_sb_q;

        // dec req ============================================================

        entry = board_d[dec_slv.req.id];

        dec_mst.req_valid = (
            dec_slv.req_valid &&
            !entry.dec_req_done
        );

        dec_slv.req_ready = (dec_mst.req_valid && dec_mst.req_ready);

        if (dec_slv.req_valid && dec_slv.req_ready) begin
            entry.vd_id = dec_slv.req.instr[11:7];
            entry.dec_req_done = '1;
        end

        board_d[dec_slv.req.id] = entry;

        // dec rsp ============================================================

        entry = board_d[dec_mst.rsp.id];

        dec_slv.rsp_valid = (
            dec_mst.rsp_valid &&
            entry.dec_req_done &&
            !entry.dec_rsp_done
        );

        dec_mst.rsp_ready = (dec_slv.rsp_valid && dec_slv.rsp_ready);

        if (dec_mst.rsp_valid && dec_mst.rsp_ready) begin
            entry.vd_clobber = dec_mst.rsp.vd_clobber;
            entry.vs1_read   = dec_mst.rsp.vs1_read;
            entry.vs2_read   = dec_mst.rsp.vs2_read;
            entry.vs3_read   = dec_mst.rsp.vs3_read;
            entry.dec_rsp_done = '1;
            if (!dec_mst.rsp.accept) entry = '0;
        end

        board_d[dec_mst.rsp.id] = entry;

        // exe req ============================================================

        entry = board_d[exe_slv.req.id];
        vs1_id = exe_slv.req.instr[19:15];
        vs2_id = exe_slv.req.instr[24:20];
        vs3_id = exe_slv.req.instr[11: 7];

        exe_mst.req_valid = (
            exe_slv.req_valid &&
            entry.dec_rsp_done &&
            !entry.exe_req_done &&
            (!entry.vd_clobber || !clobber_d[entry.vd_id]) &&
            (!entry.rsp_vs1_read || !clobber_d[vs1_id]) &&
            (!entry.rsp_vs2_read || !clobber_d[vs2_id]) &&
            (!entry.rsp_vs3_read || !clobber_d[vs3_id])
        );

        exe_slv.req_ready = (exe_mst.req_valid && exe_mst.req_ready);

        if (exe_slv.req_valid && exe_slv.req_ready) begin
            if (entry.vd_clobber) clobber_d[entry.vd_id] = '1;
            entry.exe_req_done = '1;
        end

        board_d[exe_slv.req.id] = entry;

        // exe rsp ============================================================

        entry = board_d[exe_mst.rsp.id];

        exe_slv.rsp_valid = (
            exe_mst.rsp_valid &&
            entry.exe_req_done &&
            !entry.exe_rsp_done
        );

        exe_mst.rsp_ready = (exe_slv.rsp_valid && exe_slv.rsp_ready);

        if (exe_mst.rsp_valid && exe_mst.rsp_ready) begin
            if (entry.vd_clobber) clobber_d[entry.vd_id] = '0;
            entry.exe_rsp_done = '1;
        end

        board_d[exe_mst.rsp.id] = entry;

        // end ================================================================

        for (int i = 0; i < SbLen; i++) begin
            if (
                board_d[i].dec_req_done &&
                board_d[i].dec_rsp_done &&
                board_d[i].exe_req_done &&
                board_d[i].exe_rsp_done
            ) begin
                board_d[i] = '0;
            end
        end
    end

    always_ff @(posedge clk) begin
        if (!rstn) begin
            sb_q      <= '0;
            clobber_q <= '0;
        end
        else begin
            sb_q      <= sb_d;
            clobber_q <= clobber_d;
        end
    end

endmodule
