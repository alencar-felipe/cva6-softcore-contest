package xadac_pkg;

    localparam int unsigned AddrWidth   = 32;
    localparam int unsigned XlenWidth   = 32;
    localparam int unsigned ElemWidth   = 8;
    localparam int unsigned IdWidth     = 4;
    localparam int unsigned InstrWidth  = 32;
    localparam int unsigned LenWidth    = 7;
    localparam int unsigned SumWidth    = 32;
    localparam int unsigned VectorWidth = 128;

    localparam int unsigned BeWidth = AddrWidth/8;

    localparam type AddrT   = logic [AddrWidth-1:0];
    localparam type XlenT   = logic [XlenWidth-1:0];
    localparam type ElemT   = logic [ElemWidth-1:0];
    localparam type IdT     = logic [IdWidth-1:0];
    localparam type InstrT  = logic [InstrT-1:0];
    localparam type LenT    = logic [LenWidth-1:0];
    localparam type SumT    = logic [SumWidth-1:0];
    localparam type VectorT = logic [VectorWidth-1:0];

    localparam type OpT = enum {
        Null=0,
        Vactv,
        Vbias,
        Vload,
        Vmacc
    };

endpackage
