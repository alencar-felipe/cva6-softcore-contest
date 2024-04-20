interface xadac_ex_if
    import xadac_pkg::*;
#();

    IdT     req_id;
    InstrT  req_instr;
    XlenT   req_rs1;
    XlenT   req_rs2;
    VectorT req_vs1;
    VectorT req_vs2;
    VectorT req_vs3;
    logic   req_rs1_read;
    logic   req_rs2_read;
    logic   req_vs1_read;
    logic   req_vs2_read;
    logic   req_vs3_read;
    logic   req_rd_clobber;
    logic   req_vd_clobber;
    logic   req_accept;
    logic   req_valid;
    logic   req_ready;

    IdT     resp_id;
    XlenT   resp_rd;
    VectorT resp_vd;
    logic   resp_rd_write;
    logic   resp_vd_write;
    logic   resp_valid;
    logic   resp_ready;

    modport mst (
        output req_id,
        output req_instr,
        output req_rs1,
        output req_rs2,
        output req_vs1,
        output req_vs2,
        output req_vs3,
        input  req_rs1_read,
        input  req_rs2_read,
        input  req_vs1_read,
        input  req_vs2_read,
        input  req_vs3_read,
        input  req_rd_clobber,
        input  req_vd_clobber,
        output req_valid,
        input  req_ready,

        input  resp_id,
        input  resp_rd,
        input  resp_vd,
        input  resp_rd_write,
        input  resp_vd_write,
        input  resp_valid,
        output resp_ready
    );

    modport slv (
        input  req_id,
        input  req_instr,
        input  req_rs1,
        input  req_rs2,
        input  req_vs1,
        input  req_vs2,
        input  req_vs3,
        output req_rs1_read,
        output req_rs2_read,
        output req_vs1_read,
        output req_vs2_read,
        output req_vs3_read,
        output req_rd_clobber,
        output req_vd_clobber,
        input  req_valid,
        output req_ready,

        output resp_id,
        output resp_rd,
        output resp_vd,
        output resp_rd_write,
        output resp_vd_write,
        output resp_valid,
        input  resp_ready
    );

endinterface
