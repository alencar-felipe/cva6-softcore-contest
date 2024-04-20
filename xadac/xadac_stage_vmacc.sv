module xadac_stage_vmacc
    import xadac_pkg::*;
(
    input logic  clk,
    input logic  rstn,
    xadac_if.slv slv
);

    assign slv.resp_id = slv.req_id;
    assign slv.resp_valid = slv.req_valid;
    assign slv.req_ready = slv.resp_ready;
    assign slv.resp_rd = '0;

    always_comb begin
        automatic int ilen;
        automatic int jlen;

        slv.resp_vd = '0;

        ilen = VectorWidth/SumWidth;
        jlen = SumWidth/ElemWidth;

        if (slv.req_imm < jlen) jlen = slv.req_imm;

        if (
            (slv.req_valid && slv.req_ready) &&
            (slv.resp_valid && slv.resp_ready)
        ) begin
            slv.resp_vd = slv.req_vs3;
            for (int i = 0; i < ilen; i++) begin
                for (int j = 0; j < jlen; j++) begin
                    slv.resp_vd[SumWidth*i +: SumWidth] += $unsigned(
                        $signed  (slv.req_vs1[jlen*i + j]) *
                        $unsigned(slv.req_vs2[jlen*i + j])
                    );
                end
            end
        end
    end

endmodule
