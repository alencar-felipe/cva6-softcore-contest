// Copyright 2021 Thales DIS design services SAS
//
// Licensed under the Solderpad Hardware Licence, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// SPDX-License-Identifier: Apache-2.0 WITH SHL-2.0
// You may obtain a copy of the License at https://solderpad.org/licenses/
//
// Original Author: Guillaume CHAUVON (guillaume.chauvon@thalesgroup.com)

// Functional Unit for the logic of the CoreV-X-Interface


module cvxif_fu
  import ariane_pkg::*;
  import riscv::*;
#(
    parameter config_pkg::cva6_cfg_t CVA6Cfg = config_pkg::cva6_cfg_empty
) (
    input  logic             clk_i,
    input  logic             rst_ni,
    input  fu_data_t         fu_data_i,
    input  riscv::priv_lvl_t priv_lvl_i,
    //from issue
    input  logic        x_valid_i,
    output logic        x_ready_o,
    input  logic [31:0] x_off_instr_i,
    //to writeback
    output logic         [TRANS_ID_BITS-1:0] x_trans_id_o,
    output exception_t                       x_exception_o,
    output riscv::xlen_t                     x_result_o,
    output logic                             x_valid_o,
    output logic                             x_we_o,
    //to coprocessor
    output cvxif_pkg::cvxif_req_t  cvxif_req_o,
    input  cvxif_pkg::cvxif_resp_t cvxif_resp_i,
    //dcache
    output dcache_req_t dcache_req_o,
    input  dcache_rsp_t dcache_rsp_i
);

  localparam int unsigned NumRs = cvxif_pkg::X_NUM_RS;

  logic illegal_n, illegal_q;
  logic [TRANS_ID_BITS-1:0] illegal_id_n, illegal_id_q;
  logic [31:0] illegal_instr_n, illegal_instr_q;
  logic [NumRs-1:0] rs_valid;

  if (NumRs == 3) begin : gen_third_operand
    assign rs_valid = 3'b111;
  end else begin : gen_no_third_operand
    assign rs_valid = 2'b11;
  end

  always_comb begin
    automatic logic [1:0] data_size;

    cvxif_req_o = '0;
    cvxif_req_o.x_result_ready = 1'b1;
    x_ready_o = cvxif_resp_i.x_issue_ready;
    if (x_valid_i) begin
      cvxif_req_o.x_issue_valid     = x_valid_i;
      cvxif_req_o.x_issue_req.instr = x_off_instr_i;
      cvxif_req_o.x_issue_req.mode  = priv_lvl_i;
      cvxif_req_o.x_issue_req.id    = fu_data_i.trans_id;
      cvxif_req_o.x_issue_req.rs[0] = fu_data_i.operand_a;
      cvxif_req_o.x_issue_req.rs[1] = fu_data_i.operand_b;
      if (NumRs == 3) begin
        cvxif_req_o.x_issue_req.rs[2] = fu_data_i.imm;
      end
      cvxif_req_o.x_issue_req.rs_valid   = rs_valid;
      cvxif_req_o.x_commit_valid         = x_valid_i;
      cvxif_req_o.x_commit.id            = fu_data_i.trans_id;
      cvxif_req_o.x_commit.x_commit_kill = 1'b0;
    end

    // Memory
    case (cvxif_resp_i.x_mem_req.be)
      4'b0001, 4'b0010, 4'b0100, 4'b1000: data_size = 2'b01;
      4'b0011, 4'b0110, 4'b1100:          data_size = 2'b10;
      4'b1111:                            data_size = 2'b11;
      default:                            data_size = 2'b11;
    endcase

    // req / resp
    dcache_req_o.data_req   = cvxif_resp_i.x_mem_valid;
    dcache_req_o.kill_req   = '0;
    dcache_req_o.tag_valid  = cvxif_resp_i.x_mem_valid && !cvxif_resp_i.x_mem_req.we;
    cvxif_req_o.x_mem_ready = dcache_rsp_i.data_gnt;

    dcache_req_o.address_index = cvxif_resp_i.x_mem_req.addr[DCACHE_INDEX_WIDTH-1:0];
    dcache_req_o.address_tag   = cvxif_resp_i.x_mem_req.addr[VLEN-1:DCACHE_INDEX_WIDTH];
    dcache_req_o.data_wdata    = cvxif_resp_i.x_mem_req.wdata;
    dcache_req_o.data_wuser    = '0;
    dcache_req_o.data_we       = cvxif_resp_i.x_mem_req.we;
    dcache_req_o.data_be       = cvxif_resp_i.x_mem_req.be;
    dcache_req_o.data_size     = data_size;
    dcache_req_o.data_id       = cvxif_resp_i.x_mem_req.id;

    cvxif_req_o.x_mem_resp.exc     = '0;
    cvxif_req_o.x_mem_resp.exccode = '0;

    // result
    cvxif_req_o.x_mem_result_valid = dcache_rsp_i.data_rvalid;

    cvxif_req_o.x_mem_result.id    = dcache_rsp_i.data_rid;
    cvxif_req_o.x_mem_result.rdata = dcache_rsp_i.data_rdata;
    cvxif_req_o.x_mem_result.err   = '0;
  end

  always_comb begin
    illegal_n       = illegal_q;
    illegal_id_n    = illegal_id_q;
    illegal_instr_n = illegal_instr_q;
    if (~cvxif_resp_i.x_issue_resp.accept && cvxif_req_o.x_issue_valid && cvxif_resp_i.x_issue_ready && ~illegal_n) begin
      illegal_n       = 1'b1;
      illegal_id_n    = cvxif_req_o.x_issue_req.id;
      illegal_instr_n = cvxif_req_o.x_issue_req.instr;
    end
    x_valid_o = cvxif_resp_i.x_result_valid;  //Read result only when CVXIF is enabled
    x_trans_id_o = x_valid_o ? cvxif_resp_i.x_result.id : '0;
    x_result_o = x_valid_o ? cvxif_resp_i.x_result.data : '0;
    x_exception_o.cause   = x_valid_o ? {{(riscv::XLEN-6){1'b0}}, cvxif_resp_i.x_result.exccode} : '0;
    x_exception_o.valid = x_valid_o ? cvxif_resp_i.x_result.exc : '0;
    x_exception_o.tval = '0;
    x_we_o = x_valid_o ? cvxif_resp_i.x_result.we : '0;
    if (illegal_n) begin
      if (~x_valid_o) begin
        x_trans_id_o = illegal_id_n;
        x_result_o = '0;
        x_valid_o = 1'b1;
        x_exception_o.cause = riscv::ILLEGAL_INSTR;
        x_exception_o.valid = 1'b1;
        x_exception_o.tval = illegal_instr_n;
        x_we_o = '0;
        illegal_n             = '0; // Reset flag for illegal instr. illegal_id and illegal instr values are a don't care, no need to reset it.
      end
    end
  end

  always_ff @(posedge clk_i, negedge rst_ni) begin
    if (~rst_ni) begin
      illegal_q       <= 1'b0;
      illegal_id_q    <= '0;
      illegal_instr_q <= '0;
    end else begin
      illegal_q       <= illegal_n;
      illegal_id_q    <= illegal_id_n;
      illegal_instr_q <= illegal_instr_n;
    end
  end

endmodule
