module cvxif_dcache_adapter
    import ariane_pkg::*;
    import cvxif_pkg::*;
    import riscv::*;
#(
    parameter config_pkg::cva6_cfg_t CVA6Cfg = config_pkg::cva6_cfg_empty
) (
    input logic clk_i,
    input logic rst_ni,

    input  x_mem_req_t  x_mem_req_i,
    output x_mem_resp_t x_mem_resp_o,
    input  logic        x_mem_valid_i,
    output logic        x_mem_ready_o,

    output x_mem_result_t x_mem_result_o,
    output logic          x_mem_result_valid_o,

    output dcache_req_t dcache_req_o,
    input  dcache_rsp_t dcache_rsp_i
);

  typedef logic [DCACHE_TAG_WIDTH-1:0] address_tag_t;

  logic         tag_valid_d,   tag_valid_q;
  x_mem_req_t   req_buffer_d,  req_buffer_q;
  logic         outstanding_d, outstanding_q;

  always_comb begin
    x_mem_resp_o         = '0;
    x_mem_ready_o        = '0;
    x_mem_result_o       = '0;
    x_mem_result_valid_o = '0;
    dcache_req_o         = '0;

    tag_valid_d   = tag_valid_q;
    req_buffer_d  = req_buffer_q;
    outstanding_d = outstanding_q;

    // Result =================================================================

    if (outstanding_d) begin
      if (req_buffer_d.we) begin
        x_mem_result_o = '{
          id:    req_buffer_d.id,
          rdata: '0,
          err:   '0
        };

        x_mem_result_valid_o = '1;
        outstanding_d        = '0;
      end
      else if (dcache_rsp_i.data_rvalid) begin
        x_mem_result_o = '{
          id:    req_buffer_d.id,
          rdata: dcache_rsp_i.data_rdata,
          err:   '0
        };

        x_mem_result_valid_o = '1;
        outstanding_d        = '0;
      end
    end

    // Tag ====================================================================

    if (tag_valid_q) begin
      dcache_req_o.address_tag = req_buffer_d.addr[VLEN-1:DCACHE_INDEX_WIDTH];
      dcache_req_o.tag_valid = 1;
      tag_valid_d = '0;
    end

    // Req / Resp =============================================================

    if (
      (x_mem_valid_i && !outstanding_d) &&
      (!tag_valid_q || !x_mem_req_i.we)
    ) begin
      dcache_req_o.address_index = x_mem_req_i.addr[DCACHE_INDEX_WIDTH-1:0];
      dcache_req_o.data_wdata    = x_mem_req_i.wdata;
      dcache_req_o.data_wuser    = '0;
      dcache_req_o.data_req      = x_mem_valid_i;
      dcache_req_o.data_we       = x_mem_req_i.we;
      dcache_req_o.data_be       = x_mem_req_i.be;
      dcache_req_o.data_id       = x_mem_req_i.id;
      dcache_req_o.kill_req      = '0;

      case (x_mem_req_i.be)
        4'b0001, 4'b0010, 4'b0100, 4'b1000: dcache_req_o.data_size = 2'b01;
        4'b0011, 4'b0110, 4'b1100:          dcache_req_o.data_size = 2'b10;
        4'b1111:                            dcache_req_o.data_size = 2'b11;
        default:                            dcache_req_o.data_size = 2'b11;
      endcase

      if (x_mem_req_i.we) begin
        dcache_req_o.address_tag = req_buffer_d.addr[VLEN-1:DCACHE_INDEX_WIDTH];
      end

      x_mem_resp_o.exc     = '0;
      x_mem_resp_o.exccode = '0;

      if (dcache_rsp_i.data_gnt) begin
        req_buffer_d  = x_mem_req_i;
        tag_valid_d   = !x_mem_req_i.we;
        outstanding_d = '1;
        x_mem_ready_o = '1;
      end
    end
  end

  always_ff @(posedge clk_i, negedge rst_ni) begin
    if (~rst_ni) begin
      tag_valid_q   <= '0;
      req_buffer_q  <= '0;
      outstanding_q <= '0;
    end else begin
      tag_valid_q   <= tag_valid_d;
      req_buffer_q  <= req_buffer_d;
      outstanding_q <= outstanding_d;
    end
  end

endmodule
