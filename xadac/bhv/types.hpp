#pragma once

#include <string>

#include <stdint.h>
#include <stdlib.h>

#define NO_RS (2)
#define NO_VS (3)

using namespace std;

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

#define FORMAT_HEX(data) format_hex(&(data), sizeof(data))

inline std::string format_hex(const void* ptr, size_t size) {
    const uint8_t* byte_ptr = static_cast<const uint8_t*>(ptr);
    std::string result = "0x";

    const std::string hex_digits = "0123456789abcdef";

    for (size_t i = size - 1; i < size; i++) {
        uint8_t byte = byte_ptr[i];
        result += hex_digits[(byte >> 4) & 0x0F];
        result += hex_digits[byte & 0x0F];
    }

    return result;
}

inline std::string format_dec_req(const dec_req_t& req) {
    return
        "{ 'id': "    + FORMAT_HEX(req.id) +
        ", 'instr': " + FORMAT_HEX(req.instr) +
        " }";
}

inline string format_dec_rsp(const dec_rsp_t& rsp) {
    string result =
        "{ 'id': "         + FORMAT_HEX(rsp.id) +
        ", 'rd_clobber': " + FORMAT_HEX(rsp.rd_clobber) +
        ", 'vd_clobber': " + FORMAT_HEX(rsp.vd_clobber) +
        ", 'rs_read': [";

    for (int i = 0; i < NO_RS; ++i) {
        result += FORMAT_HEX(rsp.rs_read[i]);
        if (i < NO_RS - 1) {
            result += ", ";
        }
    }

    result += "], 'vs_read': [";

    for (int i = 0; i < NO_VS; ++i) {
        result += FORMAT_HEX(rsp.vs_read[i]);
        if (i < NO_VS - 1) {
            result += ", ";
        }
    }

    result += "], 'accept': " + FORMAT_HEX(rsp.accept) + " }";

    return result;
}

inline string format_exe_req(const exe_req_t& req) {
    string result =
        "{ 'id': "    + FORMAT_HEX(req.id) +
        ", 'instr': " + FORMAT_HEX(req.instr) +
        ", 'rs_addr': [";

    for (int i = 0; i < NO_RS; ++i) {
        result += FORMAT_HEX(req.rs_addr[i]);
        if (i < NO_RS - 1) {
            result += ", ";
        }
    }

    result += "], 'rs_data': [";
    for (int i = 0; i < NO_RS; ++i) {
        result += FORMAT_HEX(req.rs_data[i]);
        if (i < NO_RS - 1) {
            result += ", ";
        }
    }

    result += "], 'vs_addr': [";
    for (int i = 0; i < NO_VS; ++i) {
        result += FORMAT_HEX(req.vs_addr[i]);
        if (i < NO_VS - 1) {
            result += ", ";
        }
    }

    result += "], 'vs_data': [";
    for (int i = 0; i < NO_VS; ++i) {
        result += FORMAT_HEX(req.vs_data[i]);
        if (i < NO_VS - 1) {
            result += ", ";
        }
    }

    result += "] }";

    return result;
}

inline string format_exe_rsp(const exe_rsp_t& rsp) {
    return
        "{ 'id': "       + FORMAT_HEX(rsp.id) +
        ", 'rd_addr': "  + FORMAT_HEX(rsp.rd_addr) +
        ", 'rd_data': "  + FORMAT_HEX(rsp.rd_data) +
        ", 'rd_write': " + FORMAT_HEX(rsp.rd_write) +
        ", 'vd_addr': "  + FORMAT_HEX(rsp.vd_addr) +
        ", 'vd_data': "  + FORMAT_HEX(rsp.vd_data) +
        ", 'vd_write': " + FORMAT_HEX(rsp.vd_write) +
        " }";
}

inline string format_axi_aw(const axi_aw_t& aw) {
    return
        "{ 'id': "   + FORMAT_HEX(aw.id) +
        ", 'addr': " + FORMAT_HEX(aw.addr) +
        " }";
}

inline string format_axi_w(const axi_w_t& w) {
    return
        "{ 'data': '" + FORMAT_HEX(w.data) +
        "', 'strb': " + FORMAT_HEX(w.strb) +
        "' }";
}

inline string format_axi_b(const axi_b_t& b) {
    return
    "{ 'id': " + FORMAT_HEX(b.id) +
    " }";
}

inline string format_axi_ar(const axi_ar_t& ar) {
    return
        "{ 'id': "   + FORMAT_HEX(ar.id) +
        ", 'addr': " + FORMAT_HEX(ar.addr) +
        " }";
}

inline string format_axi_r(const axi_r_t& r) {
    return
        "{ 'id': "    + FORMAT_HEX(r.id) +
        ", 'data': '" + FORMAT_HEX(r.data) +
        "' }";
}
