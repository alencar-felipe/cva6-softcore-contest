module xadac_ex
    import xadac_pkg::*;
(
    input logic     clk,
    input logic     rstn,
    xadac_ex_if.slv slv,
    OBI_BUS.Manager obi
);

    typedef struct {
        InstrT mask;
        InstrT match;
    } row_t;

    localparam int unsigned NoUnits    = 4;
    localparam int unsigned NoObiUnits = 2;

    localparam row_t Table [NoUnits] = '{
        '{mask: 32'h0000_707f, match: 32'h0000_3077},
        '{mask: 32'h01f0_707f, match: 32'h0000_1077},
        '{mask: 32'h01f0_707f, match: 32'h0000_0077},
        '{mask: 32'h0000_707f, match: 32'h0000_2077}
    };

    // Unit Interfaces ========================================================

    xadac_ex_if  unit_if  [NoUnits]    ();
    OBI_BUS      unit_obi [NoObiUnits] (.OBI_CFG (ObiCfg));

    IdT     unit_req_id         [NoUnits];
    InstrT  unit_req_instr      [NoUnits];
    XlenT   unit_req_rs1        [NoUnits];
    XlenT   unit_req_rs2        [NoUnits];
    VectorT unit_req_vs1        [NoUnits];
    VectorT unit_req_vs2        [NoUnits];
    VectorT unit_req_vs3        [NoUnits];
    logic   unit_req_rs1_read   [NoUnits];
    logic   unit_req_rs2_read   [NoUnits];
    logic   unit_req_vs1_read   [NoUnits];
    logic   unit_req_vs2_read   [NoUnits];
    logic   unit_req_vs3_read   [NoUnits];
    logic   unit_req_rd_clobber [NoUnits];
    logic   unit_req_vd_clobber [NoUnits];
    logic   unit_req_accept     [NoUnits];
    logic   unit_req_valid      [NoUnits];
    logic   unit_req_ready      [NoUnits];

    IdT     unit_resp_id       [NoUnits];
    XlenT   unit_resp_rd       [NoUnits];
    VectorT unit_resp_vd       [NoUnits];
    logic   unit_resp_rd_write [NoUnits];
    logic   unit_resp_vd_write [NoUnits];
    logic   unit_resp_valid    [NoUnits];
    logic   unit_resp_ready    [NoUnits];

    for (genvar i = 0; i < NoUnits; i++) begin : gen_unit_signal_assign
        assign unit_if[i].req_id    = unit_req_id   [i];
        assign unit_if[i].req_instr = unit_req_instr[i];
        assign unit_if[i].req_rs1   = unit_req_rs1  [i];
        assign unit_if[i].req_rs2   = unit_req_rs2  [i];
        assign unit_if[i].req_vs1   = unit_req_vs1  [i];
        assign unit_if[i].req_vs2   = unit_req_vs2  [i];
        assign unit_if[i].req_vs3   = unit_req_vs3  [i];
        assign unit_req_rs1_read  [i] = unit_if[i].req_rs1_read;
        assign unit_req_rs2_read  [i] = unit_if[i].req_rs2_read;
        assign unit_req_vs1_read  [i] = unit_if[i].req_vs1_read;
        assign unit_req_vs2_read  [i] = unit_if[i].req_vs2_read;
        assign unit_req_vs3_read  [i] = unit_if[i].req_vs3_read;
        assign unit_req_rd_clobber[i] = unit_if[i].req_rd_clobber;
        assign unit_req_vd_clobber[i] = unit_if[i].req_vd_clobber;
        assign unit_if[i].req_valid = unit_req_valid[i];
        assign unit_req_ready[i] = unit_if[i].req_ready;

        assign unit_resp_id      [i] = unit_if[i].resp_id;
        assign unit_resp_rd      [i] = unit_if[i].resp_rd;
        assign unit_resp_vd      [i] = unit_if[i].resp_vd;
        assign unit_resp_rd_write[i] = unit_if[i].resp_rd_write;
        assign unit_resp_vd_write[i] = unit_if[i].resp_vd_write;
        assign unit_resp_valid[i] = unit_if[i].resp_valid;
        assign unit_if[i].resp_ready = unit_resp_ready[i];
    end

    // Mux ====================================================================

    always_comb begin : mux
        slv.req_rs1_read   = '0;
        slv.req_rs2_read   = '0;
        slv.req_vs1_read   = '0;
        slv.req_vs2_read   = '0;
        slv.req_vs3_read   = '0;
        slv.req_rd_clobber = '0;
        slv.req_vd_clobber = '0;
        slv.req_accept     = '0;
        slv.req_ready      = '0;

        for (int i = 0; i < NoUnits; i++) begin
            unit_req_instr[i] = '0;
            unit_req_id   [i] = '0;
            unit_req_rs1  [i] = '0;
            unit_req_rs2  [i] = '0;
            unit_req_vs1  [i] = '0;
            unit_req_vs2  [i] = '0;
            unit_req_vs3  [i] = '0;
            unit_req_valid[i] = '0;
        end

        if (slv.req_valid) begin
            for (int i = 0; i < NoUnits; i++) begin
                if (slv.instr & Table[i].mask == Table[i].match) begin
                    unit_req_instr[i] = slv.req_instr;
                    unit_req_id   [i] = slv.req_id;
                    unit_req_rs1  [i] = slv.req_rs1;
                    unit_req_rs2  [i] = slv.req_rs2;
                    unit_req_vs1  [i] = slv.req_vs1;
                    unit_req_vs2  [i] = slv.req_vs2;
                    unit_req_vs3  [i] = slv.req_vs3;
                    slv.req_rs1_read   = unit_req_rs1_read  [i];
                    slv.req_rs2_read   = unit_req_rs2_read  [i];
                    slv.req_vs1_read   = unit_req_vs1_read  [i];
                    slv.req_vs2_read   = unit_req_vs2_read  [i];
                    slv.req_vs3_read   = unit_req_vs3_read  [i];
                    slv.req_rd_clobber = unit_req_rd_clobber[i];
                    slv.req_vd_clobber = unit_req_vd_clobber[i];
                    slv.req_accept     = unit_req_accept    [i];
                    unit_req_valid[i]  = slv.req_valid;
                    slv.req_ready      = unit_req_valid[i];
                    break;
                end
                slv.req_accept = '0;
                slv.req_ready  = '1;
            end
        end
    end

    logic [$clog2(NoUnits)-1:0] demux_unit_d, demux_unit_q;
    logic                       demux_busy_d, demux_busy_q;

    always_comb begin : demux_comb
        demux_unit_d = demux_unit_q;
        demux_busy_d = demux_busy_q;

        slv.resp_id       = '0;
        slv.resp_rd       = '0;
        slv.resp_vd       = '0;
        slv.resp_rd_write = '0;
        slv.resp_vd_write = '0;
        slv.resp_valid    = '0;

        for (int i = 0; i < NoUnits; i++) begin
            unit_resp_ready[demux_unit_d] = '0;
        end

        if (!resp_demux_d) begin
            for (int i = 0; i < NoUnits; i++) begin
                if (unit_resp_valid[i]) begin
                    demux_unit_d = i;
                    demux_busy_d = '1;
                    break;
                end
            end
        end

        if (demux_busy_d) begin
            slv.resp_id         = unit_resp_id      [demux_unit_d];
            slv.resp_rd         = unit_resp_rd      [demux_unit_d];
            slv.resp_vd         = unit_resp_vd      [demux_unit_d];
            slv.resp_rd_write   = unit_resp_rd_write[demux_unit_d];
            slv.resp_vd_write   = unit_resp_vd_write[demux_unit_d];
            slv.resp_valid      = unit_resp_valid   [demux_unit_d];
            unit_resp_ready[demux_unit_d] = slv.resp_ready;
        end

        if (slv.resp_valid && slv.resp_ready) begin
            demux_busy_d = '0;
        end
    end

    always_ff @(posedge clk, negedge rstn) begin : demux_seq
        if (!rstn) begin
            demux_unit_q <= '0;
            demux_busy_q <= '0;
        end
        else begin
            demux_unit_q <= demux_unit_d;
            demux_busy_q <= demux_unit_d;
        end
    end

    // OBI Mux ================================================================

    obi_mux_intf #(
        .SbrPortObiCfg   (ObiCfg),
        .NumSbrPorts     (NoObiUnits),
        .NumMaxTrans     (2**IdWidth),
        .UseIdForRouting ('0)
    ) i_obi_mux (
        .clk_i      (clk),
        .rst_ni     (rstn),
        .testmode_i ('0),
        .sbr_ports  (unit_obi),
        .mgr_port   (obi)
    );

    // Units ==================================================================

    xadac_vactv_unit i_vactv_unit (
        .clk  (clk),
        .rstn (rstn),
        .slv  (unit_if[0]),
        .obi  (unit_obi[0])
    );

    xadac_vbias_unit i_vbias_unit (
        .clk  (clk),
        .rstn (rstn),
        .slv  (unit_if[1])
    );

    xadac_vload_unit i_vload_unit (
        .clk  (clk),
        .rstn (rstn),
        .slv  (unit_if[2]),
        .obi  (unit_obi[1])
    );

    xadac_vmacc_unit i_vmacc_unit (
        .clk  (clk),
        .rstn (rstn),
        .slv  (unit_if[3])
    );

endmodule
