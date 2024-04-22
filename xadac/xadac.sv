module xadac
    import xadac_pkg::*;
(
    input logic     clk,
    input logic     rstn,
    xadac_if.slv    slv,
    OBI_BUS.Manager obi
);

    localparam SizeT NoUnit = 4;
    localparam SizeT NoObiUnits = 2;

    xadac_if slv_vrf ();
    xadac_if slv_mux ();
    xadac_if slv_unit [NoUnit] ();
    OBI_BUS  obi_unit [NoObiUnits] (.OBI_CFG (ObiCfg));

    xadac_vclobber i_vclobber (
        .clk  (clk),
        .rstn (rstn),
        .slv  (slv),
        .mst  (slv_vrf)
    );

    xadac_vrf i_vrf (
        .clk  (clk),
        .rstn (rstn),
        .slv  (slv_vrf),
        .mst  (slv_mux)
    );

    xadac_mux #(
        .NoMst (NoUnit),
        .Mask  ('0),
        .Match ('0)
    ) i_mux (
        .clk  (clk),
        .rstn (rstn),
        .slv  (slv_mux),
        .mst  (slv_unit)
    );

    xadac_vload i_vload (
        .clk  (clk),
        .rstn (rstn),
        .slv  (slv_unit[0]),
        .obi  (obi_unit[0])
    );

    xadac_vbias i_vbias (
        .clk  (clk),
        .rstn (rstn),
        .slv  (slv_unit[1])
    );

    xadac_vmacc i_vmacc (
        .clk  (clk),
        .rstn (rstn),
        .slv  (slv_unit[2])
    );

    xadac_vactv i_vactv (
        .clk  (clk),
        .rstn (rstn),
        .slv  (slv_unit[3]),
        .obi  (obi_unit[2])
    );

    obi_mux_intf #(
        .SbrPortObiCfg   (ObiCfg),
        .NumSbrPorts     (NoObiUnits),
        .NumMaxTrans     (SbLen),
        .UseIdForRouting ('0)
    ) i_obi_mux (
        .clk_i      (clk),
        .rst_ni     (rstn),
        .testmode_i ('0),
        .sbr_ports  (obi_unit),
        .mgr_port   (obi)
    );

endmodule
