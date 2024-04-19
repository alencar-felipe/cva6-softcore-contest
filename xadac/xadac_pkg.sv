package xadac_pkg;

    localparam int unsigned AddrWidth   = 32;
    localparam int unsigned XlenWidth   = 32;
    localparam int unsigned ElemWidth   = 8;
    localparam int unsigned IdWidth     = 4;
    localparam int unsigned ImmWidth    = 7;
    localparam int unsigned SumWidth    = 32;
    localparam int unsigned VectorWidth = 128;

    localparam int unsigned BeWidth = AddrWidth/8;

    localparam type AddrT   = logic [AddrWidth-1:0];
    localparam type XlenT   = logic [XlenWidth-1:0];
    localparam type ElemT   = logic [ElemWidth-1:0];
    localparam type IdT     = logic [IdWidth-1:0];
    localparam type ImmT    = logic [ImmWidth-1:0];
    localparam type SumT    = logic [SumWidth-1:0];
    localparam type VectorT = logic [VectorWidth-1:0];

endpackage
