interface xadac_dec_if;

    import xadac_pkg::*;

    typedef struct {
        IdT    id;
        InstrT instr;
    } req_t;

    typedef struct {
        IdT   id;
        logic rd_clobber;
        logic vd_clobber;
        logic [NoRs-1:0] rs_read;
        logic [NoVs-1:0] vs_read;
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
        RegIdT  [NoRs-1:0] rs_addr;
        XlenT   [NoRs-1:0] rs_data;
        RegIdT  [NoVs-1:0] vs_addr;
        VectorT [NoVs-1:0] vs_data;
    } req_t;

    typedef struct {
        IdT     id;
        RegIdT  rd_addr;
        XlenT   rd_data;
        logic   rd_write;
        XlenT   vd_addr;
        VectorT vd_data;
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
