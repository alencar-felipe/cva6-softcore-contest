#pragma once

#include <stdint.h>
#include <stdlib.h>

#include <sstream>
#include <iomanip>
#include <vector>
#include <string>

#include "Vxadac_verilator.h"

#define NO_RS (2)
#define NO_VS (3)

typedef Vxadac_verilator dut_t;

typedef struct { uint8_t x[16]; } uint128_t;

typedef uint8_t   logic_t;
typedef uint32_t  addr_t;
typedef uint8_t   _id_t;
typedef uint32_t  instr_t;
typedef uint8_t   reg_addr_t;
typedef uint32_t  reg_data_t;
typedef uint8_t   vec_addr_t;
typedef uint128_t vec_data_t;
typedef uint8_t   vec_strb_t;

typedef struct {
    _id_t   id;
    instr_t instr;
} dec_req_t;

typedef struct {
    _id_t   id;
    logic_t rd_clobber;
    logic_t vd_clobber;
    logic_t rs_read[NO_RS];
    logic_t vs_read[NO_VS];
    logic_t accept;
} dec_rsp_t;

typedef struct {
    _id_t      id;
    instr_t    instr;
    reg_addr_t rs_addr[NO_RS];
    reg_data_t rs_data[NO_RS];
    vec_addr_t vs_addr[NO_VS];
    vec_data_t vs_data[NO_VS];
} exe_req_t;

typedef struct {
    _id_t      id;
    reg_addr_t rd_addr;
    reg_data_t rd_data;
    logic_t    rd_write;
    vec_addr_t vd_addr;
    vec_data_t vd_data;
    logic_t    vd_write;
} exe_rsp_t;

typedef struct {
    _id_t  id;
    addr_t addr;
} axi_aw_t;

typedef struct {
    vec_data_t data;
    vec_strb_t strb;
} axi_w_t;

typedef struct {
    _id_t id;
} axi_b_t;

typedef struct {
    _id_t  id;
    addr_t addr;
} axi_ar_t;

typedef struct {
    _id_t      id;
    vec_data_t data;
} axi_r_t;
