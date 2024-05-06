// Copyright 2017-2019 ETH Zurich and University of Bologna.
// Copyright and related rights are licensed under the Solderpad Hardware
// License, Version 0.51 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://solderpad.org/licenses/SHL-0.51. Unless required by applicable law
// or agreed to in writing, software, hardware and materials distributed under
// this License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.
//
// Author: Florian Zaruba, ETH Zurich
// Date: 19.03.2017
// Description: Ariane Top-level module


module ariane
    import ariane_pkg::*;
    import riscv::*;
    import xadac_pkg::*;
#(
    parameter config_pkg::cva6_cfg_t CVA6Cfg = config_pkg::cva6_cfg_empty,
    parameter bit IsRVFI = bit'(0),
    parameter type rvfi_instr_t = logic,
    parameter int unsigned AxiAddrWidth = ariane_axi::AddrWidth,
    parameter int unsigned AxiDataWidth = ariane_axi::DataWidth,
    parameter int unsigned AxiIdWidth   = ariane_axi::IdWidth,
    parameter type axi_ar_chan_t = ariane_axi::ar_chan_t,
    parameter type axi_aw_chan_t = ariane_axi::aw_chan_t,
    parameter type axi_w_chan_t  = ariane_axi::w_chan_t,
    parameter type noc_req_t = ariane_axi::req_t,
    parameter type noc_resp_t = ariane_axi::resp_t
) (
    input logic clk_i,
    input logic rst_ni,
    // Core ID, Cluster ID and boot address are considered more or less static
    input logic [riscv::VLEN-1:0] boot_addr_i,  // reset boot address
    input logic [riscv::XLEN-1:0] hart_id_i,    // hart id in a multicore environment (reflected in a CSR)

    // Interrupt inputs
    input logic [1:0] irq_i,        // level sensitive IR lines, mip & sip (async)
    input logic       ipi_i,        // inter-processor interrupts (async)
    // Timer facilities
    input logic time_irq_i,   // timer interrupt in (async)
    input logic debug_req_i,  // debug request (async)
    // RISC-V formal interface port (`rvfi`):
    // Can be left open when formal tracing is not needed.
    output rvfi_instr_t [CVA6Cfg.NrCommitPorts-1:0] rvfi_o,
    // memory side
    output noc_req_t  noc_req_o,
    input  noc_resp_t noc_resp_i
);

    xadac_if xadac ();

    dcache_req_i_t dcache_req;
    dcache_req_o_t dcache_rsp;

    cva6 #(
        .CVA6Cfg       (CVA6Cfg),
        .IsRVFI        (IsRVFI),
        .rvfi_instr_t  (rvfi_instr_t ),
        .axi_ar_chan_t (axi_ar_chan_t),
        .axi_aw_chan_t (axi_aw_chan_t),
        .axi_w_chan_t  (axi_w_chan_t),
        .noc_req_t     (noc_req_t),
        .noc_resp_t    (noc_resp_t)
    ) i_cva6 (
        .clk_i            (clk_i),
        .rst_ni           (rst_ni),
        .boot_addr_i      (boot_addr_i),
        .hart_id_i        (hart_id_i),
        .irq_i            (irq_i),
        .ipi_i            (ipi_i),
        .time_irq_i       (time_irq_i),
        .debug_req_i      (debug_req_i),
        .rvfi_o           (rvfi_o),
        .noc_req_o        (noc_req_o),
        .noc_resp_i       (noc_resp_i),

        .xadac            (xadac),
        .ext_dcache_req_i (dcache_req),
        .ext_dcache_rsp_o (dcache_rsp)
    );

  if (CVA6Cfg.CvxifEn) begin : gen_xadac

    xadac_if xadac_cut ();

    AXI_BUS #(
        .AXI_ID_WIDTH   (IdWidth),
        .AXI_ADDR_WIDTH (AddrWidth),
        .AXI_DATA_WIDTH (VecDataWidth),
        .AXI_USER_WIDTH (DCACHE_USER_WIDTH)
    ) axi_wide ();

    AXI_BUS #(
        .AXI_ID_WIDTH   (IdWidth),
        .AXI_ADDR_WIDTH (AddrWidth),
        .AXI_DATA_WIDTH (XLEN),
        .AXI_USER_WIDTH (DCACHE_USER_WIDTH)
    ) axi_narrow ();

    AXI_BUS #(
        .AXI_ID_WIDTH   (IdWidth),
        .AXI_ADDR_WIDTH (AddrWidth),
        .AXI_DATA_WIDTH (XLEN),
        .AXI_USER_WIDTH (DCACHE_USER_WIDTH)
    ) axi_narrow_cut ();

    xadac_if_cut i_xadac_if_cut (
        .clk  (clk_i),
        .rstn (rst_ni),
        .slv  (xadac),
        .mst  (xadac_cut)
    );

    xadac i_xadac (
        .clk  (clk_i),
        .rstn (rst_ni),
        .slv  (xadac_cut),
        .axi  (axi_wide)
    );

    xadac_axi_wizard i_xadac_axi_wizard (
        .clk  (clk_i),
        .rstn (rst_ni),
        .slv  (axi_wide),
        .mst  (axi_narrow)
    );

    xadac_axi_cut #(
        .IdWidth   (IdWidth),
        .AddrWidth (AddrWidth),
        .DataWidth (XLEN),
        .UserWidth (DCACHE_USER_WIDTH)
    ) i_xadac_axi_cut (
        .clk  (clk_i),
        .rstn (rst_ni),
        .slv  (axi_narrow),
        .mst  (axi_narrow_cut)
    );

    axi_dcache_adapter i_dcache_axi_adapter (
        .clk  (clk_i),
        .rstn (rst_ni),

        .axi (axi_narrow_cut),

        .dcache_req (dcache_req),
        .dcache_rsp (dcache_rsp)
    );

  end

endmodule // ariane
