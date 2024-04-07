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

  typedef enum {
    IDLE,
    ISSUE,
    RESULT,
    ILLEGAL
  } state_t;

  state_t state;
  cvxif_pkg::x_issue_req_t req;

  assign x_ready_o = (!x_valid_i) && (state == IDLE);

  always_ff @(posedge clk_i, negedge rst_ni) begin

    x_trans_id_o  <= '0;
    x_exception_o <= '0;
    x_result_o    <= '0;
    x_valid_o     <= '0;
    x_we_o        <= '0;
    cvxif_req_o   <= '0;

    case (state)
      IDLE: begin
        if (x_valid_i) begin
          req.instr    <= x_off_instr_i;
          req.mode     <= priv_lvl_i;
          req.id       <= fu_data_i.trans_id;
          req.rs[0]    <= fu_data_i.operand_a;
          req.rs[1]    <= fu_data_i.operand_b;
          req.rs[2]    <= (NumRs == 3) ? fu_data_i.imm : '0;
          req.rs_valid <= (NumRs == 3) ? 3'b111 : 2'b11;

          state <= ISSUE;
          x_we_o <= '0;
        end
      end
      ISSUE: begin
        if (cvxif_req_o.x_issue_valid && cvxif_resp_i.x_issue_ready) begin
          if(cvxif_resp_i.x_issue_resp.accept) begin
            cvxif_req_o.x_commit.id            <= req.id;
            cvxif_req_o.x_commit.x_commit_kill <= '0;
            cvxif_req_o.x_commit_valid         <= '1;
          end
          state <= (cvxif_resp_i.x_issue_resp.accept) ? RESULT : ILLEGAL;
        end
        else begin
          cvxif_req_o.x_issue_valid <= 1;
          cvxif_req_o.x_issue_req <= req;
        end
      end
      RESULT: begin
        if (cvxif_resp_i.x_result_valid && cvxif_req_o.x_result_ready) begin
          x_trans_id_o        <= req.id;
          x_exception_o.cause <= riscv::ILLEGAL_INSTR;
          x_exception_o.valid <= '0; // cvxif_resp_i.x_result.exc;
          x_exception_o.tval  <= req.instr;
          x_result_o          <= cvxif_resp_i.x_result.rd;
          x_valid_o           <= '1;

          state <= IDLE;
          req <= '0;
        end
        else begin
          cvxif_req_o.x_result_ready <= '1;
        end
      end
      ILLEGAL: begin
        x_trans_id_o        <= req.id;
        x_exception_o.cause <= riscv::ILLEGAL_INSTR;
        x_exception_o.valid <= '0; // '1;
        x_exception_o.tval  <= req.instr;
        x_result_o          <= '0;
        x_valid_o           <= '1;

        state <= IDLE;
        req <= '0;
      end
      default: ;
    endcase

    if (~rst_ni) begin
      x_trans_id_o  <= '0;
      x_exception_o <= '0;
      x_result_o    <= '0;
      x_valid_o     <= '0;
      x_we_o        <= '0;
      cvxif_req_o   <= '0;

      state <= '0;
      req <= '0;
    end
  end

  cvxif_dcache_adapter #(
      .CVA6Cfg (CVA6Cfg)
  ) i_cvxif_dcache_adapter (
      .rst_ni,
      .clk_i,

      .x_mem_req_i  (cvxif_resp_i.x_mem_req),
      .x_mem_resp_o (cvxif_req_o.x_mem_resp),
      .x_mem_valid_i(cvxif_resp_i.x_mem_valid),
      .x_mem_ready_o(cvxif_req_o.x_mem_ready),

      .x_mem_result_o      (cvxif_req_o.x_mem_result),
      .x_mem_result_valid_o(cvxif_req_o.x_mem_result_valid),

      .dcache_req_o,
      .dcache_rsp_i
  );

endmodule
