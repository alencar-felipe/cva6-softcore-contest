interface xadac_dec_if;

    import xadac_pkg::*;

    typedef struct {
        IdT    id;
        InstrT instr;
    } req_t;

    typedef struct {
        IdT   id;
        logic rd_clobber;
        logic rs1_read;
        logic rs2_read;
        logic vd_clobber;
        logic vs1_read;
        logic vs2_read;
        logic vs3_read;
        logic accept;
    } rsp_t;

    req_t req;
    logic req_valid;
    logic req_ready;

    rsp_t rsp;
    logic rsp_valid;
    logic rsp_ready;

    modport mst (
        output req,
        output rep_valid,
        input  req_ready,

        input  rsp,
        input  rsp_valid,
        output rsp_ready
    );

    modport slv (
        input  req,
        input  req_valid,
        output req_ready,

        output rsp,
        output rsp_valid,
        input  rsp_ready
    );

endinterface

interface xadac_exe_if;

    import xadac_pkg::*;

    typedef struct {
        IdT     id;
        InstrT  instr;
        XlenT   rs1;
        XlenT   rs2;
        VectorT vs1;
        VectorT vs2;
        VectorT vs3;
    } req_t;

    typedef struct {
        IdT     id;
        XlenT   rd;
        VectorT vd;
        logic   rd_write;
        logic   vd_write;
    } rsp_t;

    req_t req;
    logic req_valid;
    logic req_ready;

    rsp_t rsp;
    logic rsp_valid;
    logic rsp_ready;

    modport mst (
        output req,
        output req_valid,
        input  req_ready,

        input  rsp,
        input  rsp_valid,
        output rsp_ready
    );

    modport slv (
        input  req,
        input  req_valid,
        output req_ready,

        output rsp,
        input  rsp_ready
    );

endinterface
