module xadac_vbias_unit
    import xadac_pkg::*;
(
    input logic     clk,
    input logic     rstn,
    xadac_ex_if.slv slv
);

    assign slv.resp_id = slv.req_id;
    assign slv.resp_valid = slv.req_valid;
    assign slv.req_ready = slv.resp_ready;
    assign slv.resp_rd = '0;

    always_comb begin
        slv.resp_vd = '0;

        if (
            (slv.req_valid && slv.req_ready) &&
            (slv.resp_valid && slv.resp_ready)
        ) begin
            for (ImmT i = 0; i < slv.req_imm; i++) begin
                slv.resp_vd[SumWidth*i +: SumWidth] = SumT'(slv.req_rs1);
            end
        end
    end

endmodule
