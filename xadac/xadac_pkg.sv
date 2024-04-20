package xadac_pkg;

    import obi_pkg::*;

    localparam int unsigned NoRs = 2;
    localparam int unsigned NoVs = 3;

    localparam int unsigned AddrWidth   = 32;
    localparam int unsigned XlenWidth   = 32;
    localparam int unsigned ElemWidth   = 8;
    localparam int unsigned IdWidth     = 4;
    localparam int unsigned InstrWidth  = 32;
    localparam int unsigned VLenWidth   = 7;
    localparam int unsigned VrfIdWidth  = 5;
    localparam int unsigned SumWidth    = 32;
    localparam int unsigned VectorWidth = 128;

    localparam int unsigned BeWidth = VectorWidth/8;
    localparam int unsigned VrfLen  = 2**VrfIdWidth;
    localparam int unsigned SbLen   = 2**IdWidth;

    localparam type AddrT   = logic [AddrWidth-1:0];
    localparam type XlenT   = logic [XlenWidth-1:0];
    localparam type ElemT   = logic [ElemWidth-1:0];
    localparam type IdT     = logic [IdWidth-1:0];
    localparam type InstrT  = logic [InstrT-1:0];
    localparam type VLenT   = logic [VLenWidth-1:0];
    localparam type VrfIdT  = logic [VrfIdWidth-1:0];
    localparam type SumT    = logic [SumWidth-1:0];
    localparam type VectorT = logic [VectorWidth-1:0];

    localparam obi_cfg_t ObiCfg = obi_default_cfg(
        AddrWidth,
        VectorWidth,
        IdWidth,
        ObiMinimalOptionalConfig
    );

endpackage
