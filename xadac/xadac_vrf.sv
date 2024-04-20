module xadac_vrf
    import xadac_pkg::*;
(
    input logic clk,
    input logic rstn,

    input  RegIdT  [2:0] rid,
    output VectorT [2:0] rdata,

    input RegIdT  wid,
    input VectorT wdata,
    input BeT     wbe,
    input logic   we
);

    localparam int unsigned VrfLen = 2**RegIdT;

    VectorT [VrfLen-1:0] vrf;

    always_ff @(posedge clk) begin
        for (int i = 0; i < VectorWidth/8; i++) begin
            if (we & wbe[i]) vrf[wid][i*8 +: 8] <= wdata[i*8 +: 8];
        end
    end

    always_comb begin
        for (int i = 0; i < 3; i++) begin
            rdata[i] = vrf[rid[i]];
        end
    end

endmodule
