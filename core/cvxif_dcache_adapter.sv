module cvxif_dcache_adapter
    import ariane_pkg::*;
    import cvxif_pkg::*;
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
    output dcache_rsp_t dcache_rsp_i
);

  always_comb begin
    // req/resp
    dcache_req_o.data_req   = '0; // x_mem_valid_i;
    dcache_req_o.kill_req   = '0; // '0;
    dcache_req_o.tag_valid  = '0; // x_mem_valid_i && !x_mem_req_i.we;
    x_mem_ready_o           = '0; // dcache_rsp_i.data_gnt;

    dcache_req_o.address_index = '0; // x_mem_req_i.addr[DCACHE_INDEX_WIDTH-1:0];
    dcache_req_o.address_tag   = '0; // x_mem_req_i.addr[VLEN-1:DCACHE_INDEX_WIDTH];
    dcache_req_o.data_wdata    = '0; // x_mem_req_i.wdata;
    dcache_req_o.data_wuser    = '0; // '0;
    dcache_req_o.data_we       = '0; // x_mem_req_i.we;
    dcache_req_o.data_be       = '0; // x_mem_req_i.be;
    dcache_req_o.data_id       = '0; // x_mem_req_i.id;

    case (x_mem_req_i.be)
      4'b0001, 4'b0010, 4'b0100, 4'b1000: dcache_req_o.data_size = 2'b01;
      4'b0011, 4'b0110, 4'b1100:          dcache_req_o.data_size = 2'b10;
      4'b1111:                            dcache_req_o.data_size = 2'b11;
      default:                            dcache_req_o.data_size = 2'b11;
    endcase

    x_mem_resp_o.exc     = '0; // '0;
    x_mem_resp_o.exccode = '0; // '0;

    // result
    //if (x_mem_valid_i && x_mem_ready_o && x_mem_req_i.we)

    x_mem_result_valid_o = '0; // dcache_rsp_i.data_rvalid;

    x_mem_result_o.id    = '0; // dcache_rsp_i.data_rid;
    x_mem_result_o.rdata = '0; // dcache_rsp_i.data_rdata;
    x_mem_result_o.err   = '0; // '0;
  end

  always_ff @(posedge clk_i, negedge rst_ni) begin
    if (~rst_ni) begin

    end else begin

    end
  end

endmodule
