module xadac_vrf
    import xadac_pkg::*;
(
    input logic clk,
    input logic rstn,

    input  RegIdT  [2:0] rid,
    output VectorT [2:0] rdata,
    input  RegIdT        wid,
    input  VectorT       wdata,
    input  logic         we
);

    VectorT [VrfLen-1:0] vrf;

    always_ff @(posedge clk) begin
        if (we) begin
            vrf[wid] <= wdata;
        end
    end

    always_comb begin
        for (int i = 0; i < 3; i++) begin
            rdata[i] = vrf[rid[i]];
        end
    end

endmodule
