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


module ariane import ariane_pkg::*; #(
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
  input  logic                         clk_i,
  input  logic                         rst_ni,
  // Core ID, Cluster ID and boot address are considered more or less static
  input  logic [riscv::VLEN-1:0]       boot_addr_i,  // reset boot address
  input  logic [riscv::XLEN-1:0]       hart_id_i,    // hart id in a multicore environment (reflected in a CSR)

  // Interrupt inputs
  input  logic [1:0]                   irq_i,        // level sensitive IR lines, mip & sip (async)
  input  logic                         ipi_i,        // inter-processor interrupts (async)
  // Timer facilities
  input  logic                         time_irq_i,   // timer interrupt in (async)
  input  logic                         debug_req_i,  // debug request (async)
  // RISC-V formal interface port (`rvfi`):
  // Can be left open when formal tracing is not needed.
  output rvfi_instr_t [CVA6Cfg.NrCommitPorts-1:0] rvfi_o,
  // memory side
  output noc_req_t                     noc_req_o,
  input  noc_resp_t                    noc_resp_i
);

  localparam X_NUM_RS    = ariane_pkg::NR_RGPR_PORTS;
  localparam X_ID_WIDTH  = ariane_pkg::TRANS_ID_BITS;
  localparam X_MEM_WIDTH = riscv::XLEN; //AxiDataWidth;
  localparam X_RFR_WIDTH = riscv::XLEN;
  localparam X_RFW_WIDTH = riscv::XLEN;
  localparam X_MISA      = 0;

  cvxif_pkg::cvxif_req_t  cvxif_req;
  cvxif_pkg::cvxif_resp_t cvxif_resp;

  cva6 #(
    .CVA6Cfg ( CVA6Cfg ),
    .IsRVFI ( IsRVFI ),
    .rvfi_instr_t ( rvfi_instr_t ),
    .axi_ar_chan_t (axi_ar_chan_t),
    .axi_aw_chan_t (axi_aw_chan_t),
    .axi_w_chan_t (axi_w_chan_t),
    .noc_req_t (noc_req_t),
    .noc_resp_t (noc_resp_t)
  ) i_cva6 (
    .clk_i                ( clk_i                     ),
    .rst_ni               ( rst_ni                    ),
    .boot_addr_i          ( boot_addr_i               ),
    .hart_id_i            ( hart_id_i                 ),
    .irq_i                ( irq_i                     ),
    .ipi_i                ( ipi_i                     ),
    .time_irq_i           ( time_irq_i                ),
    .debug_req_i          ( debug_req_i               ),
    .rvfi_o               ( rvfi_o                    ),
    .cvxif_req_o          ( cvxif_req                 ),
    .cvxif_resp_i         ( cvxif_resp                ),
    .noc_req_o            ( noc_req_o                 ),
    .noc_resp_i           ( noc_resp_i                )
  );

  // if (CVA6Cfg.CvxifEn) begin : gen_example_coprocessor
  //   cvxif_example_coprocessor i_cvxif_coprocessor (
  //     .clk_i                ( clk_i                          ),
  //     .rst_ni               ( rst_ni                         ),
  //     .cvxif_req_i          ( cvxif_req                      ),
  //     .cvxif_resp_o         ( cvxif_resp                     )
  //   );
  // end

  if (CVA6Cfg.CvxifEn) begin : gen_vicuna

    vproc_xif #(
      .X_NUM_RS    (X_NUM_RS),
      .X_ID_WIDTH  (X_ID_WIDTH),
      .X_MEM_WIDTH (X_MEM_WIDTH),
      .X_RFR_WIDTH (X_RFR_WIDTH),
      .X_RFW_WIDTH (X_RFW_WIDTH),
      .X_MISA      (X_MISA)
    ) vproc_xif ();

    // Compressed =============================================================

    assign x_compressed_ready = '0;

    assign x_compressed_resp = '0;

    // Issue ==================================================================

    assign vproc_xif.issue_valid = cvxif_req.x_issue_valid;
    assign cvxif_resp.x_issue_ready = vproc_xif.issue_ready;

    assign vproc_xif.issue_req.instr    = cvxif_req.x_issue_req.instr;
    assign vproc_xif.issue_req.mode     = cvxif_req.x_issue_req.mode;
    assign vproc_xif.issue_req.id       = cvxif_req.x_issue_req.id;
    assign vproc_xif.issue_req.rs       = cvxif_req.x_issue_req.rs;
    assign vproc_xif.issue_req.rs_valid = cvxif_req.x_issue_req.rs_valid;

    assign cvxif_resp.x_issue_resp.accept    = vproc_xif.issue_resp.accept;
    assign cvxif_resp.x_issue_resp.writeback = vproc_xif.issue_resp.writeback;
    assign cvxif_resp.x_issue_resp.dualwrite = vproc_xif.issue_resp.dualwrite;
    assign cvxif_resp.x_issue_resp.dualread  = vproc_xif.issue_resp.dualread != 0;
    assign cvxif_resp.x_issue_resp.loadstore = vproc_xif.issue_resp.loadstore;
    assign cvxif_resp.x_issue_resp.exc       = vproc_xif.issue_resp.exc;

    // Commit =================================================================

    assign vproc_xif.commit_valid = cvxif_req.x_commit_valid;

    assign vproc_xif.commit.id          = cvxif_req.x_commit.id;
    assign vproc_xif.commit.commit_kill = cvxif_req.x_commit.x_commit_kill;

    // Memory req/resp ========================================================

    assign cvxif_resp.x_mem_valid = vproc_xif.mem_valid;
    assign vproc_xif.mem_ready = cvxif_req.x_mem_ready;

    assign cvxif_resp.x_mem_req.id    = vproc_xif.mem_req.id;
    assign cvxif_resp.x_mem_req.addr  = vproc_xif.mem_req.addr;
    assign cvxif_resp.x_mem_req.mode  = vproc_xif.mem_req.mode;
    assign cvxif_resp.x_mem_req.we    = vproc_xif.mem_req.we;
    assign cvxif_resp.x_mem_req.size  = vproc_xif.mem_req.size;
    assign cvxif_resp.x_mem_req.be    = vproc_xif.mem_req.be;
    assign cvxif_resp.x_mem_req.attr  = vproc_xif.mem_req.attr;
    assign cvxif_resp.x_mem_req.wdata = vproc_xif.mem_req.wdata;
    assign cvxif_resp.x_mem_req.last  = vproc_xif.mem_req.last;
    assign cvxif_resp.x_mem_req.spec  = vproc_xif.mem_req.spec;

    assign vproc_xif.mem_resp.exc     = cvxif_req.x_mem_resp.exc;
    assign vproc_xif.mem_resp.exccode = cvxif_req.x_mem_resp.exccode;
    assign vproc_xif.mem_resp.dbg     = '0;

    // Memory result ==========================================================

    assign vproc_xif.mem_result_valid = cvxif_req.x_mem_result_valid;

    assign vproc_xif.mem_result.id    = cvxif_req.x_mem_result.id;
    assign vproc_xif.mem_result.rdata = cvxif_req.x_mem_result.rdata;
    assign vproc_xif.mem_result.err   = cvxif_req.x_mem_result.err;

    // Result =================================================================

    assign cvxif_resp.x_result_valid = vproc_xif.result_valid;
    assign vproc_xif.result_ready = cvxif_req.x_result_ready;

    assign cvxif_resp.x_result.id       = vproc_xif.result.id;
    assign cvxif_resp.x_result.data     = vproc_xif.result.data;
    assign cvxif_resp.x_result.rd       = vproc_xif.result.rd;
    assign cvxif_resp.x_result.we       = vproc_xif.result.we != 0;
    assign cvxif_resp.x_result.exc      = vproc_xif.result.exc;
    assign cvxif_resp.x_result.exccode  = vproc_xif.result.exccode;

    vproc_core #(
      .XIF_ID_W  (X_ID_WIDTH),
      .XIF_MEM_W (X_MEM_WIDTH),

      .DONT_CARE_ZERO ('0),
      .ASYNC_RESET    ('1)
    ) i_vproc_core (
      .clk_i  (clk_i),
      .rst_ni (rst_ni),

      .xif_issue_if  (vproc_xif),
      .xif_commit_if (vproc_xif),
      .xif_mem_if    (vproc_xif),
      .xif_memres_if (vproc_xif),
      .xif_result_if (vproc_xif),

      .pending_load_o  (),
      .pending_store_o (),

      .csr_vtype_o      (),
      .csr_vl_o         (),
      .csr_vlenb_o      (),
      .csr_vstart_o     (),
      .csr_vstart_i     ('0),
      .csr_vstart_set_i ('0),
      .csr_vxrm_o       (),
      .csr_vxrm_i       ('0),
      .csr_vxrm_set_i   ('0),
      .csr_vxsat_o      (),
      .csr_vxsat_i      ('0),
      .csr_vxsat_set_i  ('0),

      .pend_vreg_wr_map_o ()
    );

  end

`ifndef VERILATOR

  assert property (
    @(posedge clk_i)
    cvxif_req.x_issue_valid && cvxif_resp.x_issue_ready |->
    cvxif_resp.x_issue_resp.accept
  ) else $fatal(1, "Issued instruction was not accepted.");

  assert property (
    @(posedge clk_i)
    cvxif_req.x_issue_valid && !cvxif_resp.x_issue_ready |=>
    cvxif_req.x_issue_valid
  ) else $fatal(1, "Ilegal deassertion of x_issue_valid.");

  assert property (
    @(posedge clk_i)
    cvxif_resp.x_result_valid && cvxif_req.x_result_ready |->
    !cvxif_resp.x_result.exc
  ) else $fatal(1, "Exception occurred.");

`endif

endmodule // ariane
