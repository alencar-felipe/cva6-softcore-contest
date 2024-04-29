module dcache_axi_adapter
    import ariane_pkg::*;
    import riscv::*;
(
    input logic clk_i,
    input logic rst_ni,

    AXI_BUS.slave axi,

    output dcache_req_t write_dcache_req_o,
    input  dcache_rsp_t write_dcache_rsp_i,

    output dcache_req_t read_dcache_req_o,
    input  dcache_rsp_t read_dcache_rsp_i
);

    localparam int unsigned IdWidth    = TRANS_ID_BITS;
    localparam int unsigned TagWidth   = DCACHE_TAG_WIDTH;
    localparam int unsigned IndexWidth = DCACHE_INDEX_WIDTH;
    localparam int unsigned AddrWidth  = TagWidth + IndexWidth;
    localparam int unsigned DataWidth  = XLEN;
    localparam int unsigned StrbWidth  = DataWidth/8;

    typedef logic [IdWidth-1:0]    id_t;
    typedef logic [TagWidth-1:0]   tag_t;
    typedef logic [IndexWidth-1:0] index_t;
    typedef logic [AddrWidth-1:0]  addr_t;
    typedef logic [DataWidth-1:0]  data_t;
    typedef logic [StrbWidth-1:0]  strb_t;

    function automatic logic [1:0] get_data_size(input logic [3:0] strb);
        case (strb)
            4'b0001, 4'b0010, 4'b0100, 4'b1000: begin
                return 2'b00;
            end
            4'b0011, 4'b0110, 4'b1100: begin
                return 2'b01;
            end
            4'b1111: begin
                return 2'b10;
            end
            default: begin
                return 2'b10;
            end
        endcase
    endfunction

    AXI_BUS #(
        .AXI_ADDR_WIDTH (AddrWidth),
        .AXI_DATA_WIDTH (DataWidth),
        .AXI_ID_WIDTH   (TRANS_ID_BITS),
        .AXI_USER_WIDTH (1)
    ) axi_skid ();

    // write ==================================================================

    id_t   id_d,       id_q;
    addr_t addr_d,     addr_q;
    data_t data_d,     data_q;
    strb_t strb_d,     strb_q;
    logic  aw_done_d,  aw_done_q;
    logic  w_done_d,   w_done_q;
    logic  b_done_d,   b_done_q;
    logic  req_done_d, req_done_q;

    always_comb begin: write_comb_req
        id_d       = id_q;
        addr_d     = addr_q;
        data_d     = data_q;
        strb_d     = strb_q;
        aw_done_d  = aw_done_q;
        w_done_d   = w_done_q;
        b_done_d   = b_done_q;
        req_done_d = req_done_q;

        // aw =================================================================

        axi_skid.aw_ready = !aw_done_d;

        if (axi_skid.aw_valid && axi_skid.aw_ready) begin
            id_d   = axi_skid.aw_id;
            addr_d = axi_skid.aw_addr;
            aw_done_d = '1;
        end

        // w ==================================================================

        axi_skid.w_ready = !w_done_d;

        if (axi_skid.w_valid && axi_skid.w_ready) begin
            data_d = axi_skid.w_data;
            strb_d = axi_skid.w_strb;
            w_done_d = '1;
        end

        // b ==================================================================

        axi_skid.b_valid = (aw_done_q && w_done_q && !b_done_d);

        if (axi_skid.b_valid && axi_skid.b_ready) begin
            data_d = axi_skid.w_data;
            strb_d = axi_skid.w_strb;
            w_done_d = '1;
        end

        // end ================================================================

        write_dcache_req_o.data_req = (aw_done_d && w_done_d && !req_done_d);

        if (write_dcache_req_o.data_req && write_dcache_rsp_i.data_gnt) begin
            write_dcache_req_o.address_index  = addr_d[IndexWidth-1:0];
            write_dcache_req_o.address_tag    = addr_d[AddrWidth-1:IndexWidth];
            write_dcache_req_o.data_wdata     = data_d;
            write_dcache_req_o.data_wuser     = '0;
            write_dcache_req_o.data_we        = '1;
            write_dcache_req_o.data_be        = strb_d;
            write_dcache_req_o.data_size      = get_data_size(strb_d);
            write_dcache_req_o.data_id        = id_d;
            write_dcache_req_o.kill_req       = '0;
            req_done_d = '1;
        end

        // end ================================================================

        if (aw_done_d && w_done_d && b_done_d && req_done_d) begin
            id_d       = '0;
            addr_d     = '0;
            data_d     = '0;
            strb_d     = '0;
            aw_done_d  = '0;
            w_done_d   = '0;
            b_done_d   = '0;
            req_done_d = '0;
        end
    end

    always_ff @(posedge clk_i, negedge rst_ni) begin
        if (~rst_ni) begin
            id_q       <= '0;
            addr_q     <= '0;
            data_q     <= '0;
            strb_q     <= '0;
            aw_done_q  <= '0;
            w_done_q   <= '0;
            b_done_q   <= '0;
            req_done_q <= '0;
        end else begin
            id_q       <= id_d;
            addr_q     <= addr_d;
            data_q     <= data_d;
            strb_q     <= strb_d;
            aw_done_q  <= aw_done_d;
            w_done_q   <= w_done_d;
            b_done_q   <= b_done_d;
            req_done_d <= req_done_d;
        end
    end

    // read ===================================================================

    tag_t tag_d;
    logic tag_valid_d, tag_valid_q;

    always_comb begin : read_comb_req
        read_dcache_req_o.address_index = axi_skid.ar_addr[IndexWidth-1:0];
        read_dcahce_req_o.data_wdata    = '0;
        read_dcache_req_o.data_wuser    = '0;
        read_dcache_req_o.data_req      = axi_skid.ar_valid;
        read_dcache_req_o.data_we       = '1;
        read_dcache_req_o.data_be       = '1;
        read_dcache_req_o.data_size     = 2'b10;
        read_dcache_req_o.data_id       = axi_skid.ar_id;
        read_dcache_req_o.kill_req      = '0;
        read_dcache_req_o.tag_valid     = tag_valid_q;

        axi_skid.ar_ready = read_dcache_rsp_i.data.gnt;

        tag_valid_d = axi_skid.ar_addr[AddrWidth-1:IndexWidth];
    end

    always_ff @(posedge clk_i, negedge rst_ni) begin : read_seq_req
        if (~rst_ni) begin
            tag_valid_q <= '0;
        end
        else begin
            tag_valid_q <= tag_valid_d;
        end
    end

    always_comb begin : read_comb_rsp
        axi_skid.r_id    = read_dcache_rsp_i.data_rid;
        axi_skid.r_data  = read_dcache_rsp_i.data_rdata;
        axi_skid.r_resp  = '0;
        axi_skid.r_last  = '1;
        axi_skid.r_user  = '0;
        axi_skid.r_valid = read_dcache_rsp_i.data_rdata;
    end

`ifndef VERILATOR
    assert property (
        @(posedge clk_i) axi_skid.r_valid -> axi_skid.r_ready
    ) else $error("R stream error.");
`endif

endmodule
