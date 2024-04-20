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

endmodule
