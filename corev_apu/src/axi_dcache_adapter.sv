module axi_dcache_adapter
    import ariane_pkg::*;
    import riscv::*;
(
    input logic clk_i,
    input logic rst_ni,

    AXI_BUS.Slave axi_skid,

    output dcache_req_t dcache_req_o,
    input  dcache_rsp_t dcache_rsp_i
);

    localparam int unsigned IdWidth    = TRANS_ID_BITS;
    localparam int unsigned TagWidth   = DCACHE_TAG_WIDTH;
    localparam int unsigned IndexWidth = DCACHE_INDEX_WIDTH;
    localparam int unsigned AddrWidth  = VLEN;
    localparam int unsigned DataWidth  = 128;
    localparam int unsigned TransWidth = 32;
    localparam int unsigned StrbWidth  = DataWidth/8;
    localparam int unsigned SizeWidth  = 8;

    typedef logic [IdWidth-1:0]    id_t;
    typedef logic [TagWidth-1:0]   tag_t;
    typedef logic [IndexWidth-1:0] index_t;
    typedef logic [AddrWidth-1:0]  addr_t;
    typedef logic [DataWidth-1:0]  data_t;
    typedef logic [StrbWidth-1:0]  strb_t;
    typedef logic [SizeWidth-1:0]  size_t;

    function automatic logic [1:0] dsize(input logic [3:0] strb);
        case (strb)
            4'b0001, 4'b0010, 4'b0100, 4'b1000: return 2'b00;
            4'b0011, 4'b0110, 4'b1100:          return 2'b01;
            4'b1111:                            return 2'b10;
            default:                            return 2'b10;
        endcase
    endfunction

    // AXI_BUS #(
    //     .AXI_ADDR_WIDTH (AddrWidth),
    //     .AXI_DATA_WIDTH (DataWidth),
    //     .AXI_ID_WIDTH   (TRANS_ID_BITS),
    //     .AXI_USER_WIDTH (1)
    // ) axi_skid ();

    // write ==================================================================

    typedef struct packed {
        id_t   id;
        addr_t addr;
        data_t data;
        strb_t strb;

        logic tag_valid;

        size_t req_count;
        size_t rsp_count;

        logic aw_done;
        logic w_done;
        logic b_done;

        logic ar_done;
        logic r_done;
    } state_t;

    state_t state_d, state_q;

    always_comb begin
        state_d = state_q;

        dcache_req_o = '0;

        axi_skid.b_id   = '0;
        axi_skid.b_resp = '0;
        axi_skid.b_user = '0;

        // aw =================================================================

        axi_skid.aw_ready = (!state_d.ar_done && !state_d.aw_done);

        if (axi_skid.aw_valid && axi_skid.aw_ready) begin
            state_d.id      = axi_skid.aw_id;
            state_d.addr    = axi_skid.aw_addr;
            state_d.aw_done = '1;
        end

        // w ==================================================================

        axi_skid.w_ready = (state_d.aw_done && !state_d.w_done);

        if (axi_skid.w_valid && axi_skid.w_ready) begin
            state_d.data   = axi_skid.w_data;
            state_d.strb   = axi_skid.w_strb;
            state_d.w_done = '1;
        end

        // ar =================================================================

        axi_skid.ar_ready = (!state_d.ar_done && !state_d.aw_done);

        if (axi_skid.ar_valid && axi_skid.ar_ready) begin
            state_d.id   = axi_skid.ar_id;
            state_d.addr = axi_skid.ar_addr;
            state_d.ar_done = '1;
        end

        // tag ================================================================

        dcache_req_o.address_tag = '0;
        dcache_req_o.tag_valid   = '0;

        if (state_d.tag_valid) begin
            dcache_req_o.address_tag = state_d.addr[AddrWidth-1:IndexWidth];
            dcache_req_o.tag_valid   = '1;
            state_d.tag_valid = '0;
        end

        // req ================================================================

        dcache_req_o.address_index = '0;
        dcache_req_o.data_wdata    = '0;
        dcache_req_o.data_wuser    = '0;
        dcache_req_o.data_we       = '0;
        dcache_req_o.data_be       = '0;
        dcache_req_o.data_size     = '0;
        dcache_req_o.data_id       = '0;
        dcache_req_o.kill_req      = '0;

        dcache_req_o.data_req = ((
            (state_d.ar_done) ||
            (state_d.aw_done && state_d.w_done && !state_q.tag_valid)
        ) && (
            (state_d.req_count < 4)
        ));

        if (dcache_req_o.data_req && dcache_rsp_i.data_gnt) begin
            dcache_req_o.address_index = state_d.addr[IndexWidth-1:0];
            dcache_req_o.data_wdata    = state_d.data[TransWidth-1:0];
            dcache_req_o.data_wuser    = '0;
            dcache_req_o.data_we       = state_d.aw_done;
            dcache_req_o.data_be       = state_d.strb[TransWidth/8-1:0];
            dcache_req_o.data_size     = dsize(state_d.strb[TransWidth/8-1:0]);
            dcache_req_o.data_id       = state_d.id;
            dcache_req_o.kill_req      = '0;

            state_d.tag_valid = state_d.ar_done;

            state_d.req_count += 1;

            if(state_d.aw_done) begin
                dcache_req_o.address_tag = state_d.addr[AddrWidth-1:IndexWidth];

                state_d.data = data_t'(state_d.data >> TransWidth);
                state_d.strb = strb_t'(state_d.strb >> TransWidth/8);

                state_d.rsp_count += 1;
            end
        end

        // rsp ================================================================

        if (dcache_rsp_i.data_rvalid) begin
            state_d.data <<= TransWidth;
            state_d.data[TransWidth-1:0] = dcache_rsp_i.data_rdata;

            state_d.rsp_count += 1;
        end

        // b ==================================================================

        axi_skid.b_id   = '0;
        axi_skid.b_resp = '0;
        axi_skid.b_user = '0;

        axi_skid.b_valid = (state_d.aw_done && state_d.rsp_count >= 4);

        if (axi_skid.b_valid && axi_skid.b_ready) begin
            axi_skid.b_id   = state_d.id;
            axi_skid.b_resp = '0;
            axi_skid.b_user = '0;
            state_d.b_done = '1;
        end

        // r ==================================================================

        axi_skid.r_id   = '0;
        axi_skid.r_data = '0;
        axi_skid.r_resp = '0;
        axi_skid.r_last = '0;
        axi_skid.r_user = '0;

        axi_skid.r_valid = (state_d.ar_done && state_d.rsp_count >= 4);

        if (axi_skid.r_valid && axi_skid.r_ready) begin
            axi_skid.r_id   = state_d.id;
            axi_skid.r_data = state_d.data;
            axi_skid.r_resp = '0;
            axi_skid.r_last = '1;
            axi_skid.r_user = '0;
            state_d.r_done = '1;
        end

        // end ================================================================

        if (state_d.r_done || state_d.b_done) begin
            state_d = '0;
        end
    end

    always_ff @(posedge clk_i, negedge rst_ni) begin
        if (~rst_ni) begin
            state_q <= '0;
        end else begin
            state_q <= state_d;
        end
    end

endmodule
