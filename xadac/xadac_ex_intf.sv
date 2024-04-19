interface xadac_ex_if
    import xadac_pkg::*;
#();

    IdT     req_id;
    XlenT   req_rs1;
    XlenT   req_rs2;
    VectorT req_vs1;
    VectorT req_vs2;
    VectorT req_vs3;
    ImmT    req_imm;
    logic   req_valid;
    logic   req_ready;

    IdT     resp_id;
    XlenT   resp_rd;
    VectorT resp_vd;
    logic   resp_valid;
    logic   resp_ready;

    modport Master (
        output req_id,
        output req_rs1,
        output req_rs2,
        output req_vs1,
        output req_vs2,
        output req_vs3,
        output req_imm,
        output req_valid,
        input  req_ready,

        input  resp_id,
        input  resp_rd,
        input  resp_vd,
        input  resp_valid,
        output resp_ready
    );

    modport Slave (
        input  req_id,
        input  req_rs1,
        input  req_rs2,
        input  req_vs1,
        input  req_vs2,
        input  req_vs3,
        input  req_imm,
        input  req_valid,
        output req_ready,

        output resp_id,
        output resp_rd,
        output resp_vd,
        output resp_valid,
        input  resp_ready
    );

    modport Monitor (
        input req_id,
        input req_rs1,
        input req_rs2,
        input req_vs1,
        input req_vs2,
        input req_vs3,
        input req_imm,
        input req_valid,
        input req_ready,

        input resp_id,
        input resp_rd,
        input resp_vd,
        input resp_valid,
        input resp_ready
    );

endinterface
