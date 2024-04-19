interface obi_if #(
    parameter int unsigned AddrWidth = 0,
    parameter int unsigned DataWidth = 0,
    parameter int unsigned IdWidth = 0
);

    localparam type AddrT = logic [AddrWidth-1:0];
    localparam type DataT = logic [DataWidth-1:0];
    localparam type BeT   = logic [DataWidth/8-1:0];
    localparam type IdT   = logic [IdT-1:0];

    logic req;
    logic gnt;
    AddrT addr;
    logic we;
    BeT   be;
    DataT wdata;
    IdT   aid;

    logic rvalid;
    logic rready;
    DataT rdata;
    IdT   rid;

    modport Master (
        output req,
        input  gnt,
        output addr,
        output we,
        output be,
        output wdata,
        output aid,

        input  rvalid,
        output rready,
        input  rdata,
        input  rid
    );

    modport Slave (
        input  req,
        output gnt,
        input  addr,
        input  we,
        input  be,
        input  wdata,
        input  aid,

        output rvalid,
        input  rready,
        output rdata,
        output rid
    );

    modport Monitor (
        output req,
        output gnt,
        output addr,
        output we,
        output be,
        output wdata,
        output aid,

        output rvalid,
        output rready,
        output rdata,
        output rid
    );

endinterface
