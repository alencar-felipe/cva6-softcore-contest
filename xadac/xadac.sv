module xadac
    import xadac_pkg::*;
(
    input logic  clk,
    input logic  rstn,

    xadac_dec_if.slv dec,
    xadac_exe_if.slv exe,
    OBI_BUS.Manager  obi
);

endmodule
