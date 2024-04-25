#include <string.h>

#include <list>
#include <mutex>
#include <thread>

#include <verilated.h>
#include <verilated_vcd_c.h>

#include "types.hpp"
#include "format.hpp"

using namespace std;

static list<dec_req_t> dec_req_list;
static list<dec_rsp_t> dec_rsp_list;
static list<exe_req_t> exe_req_list;
static list<exe_rsp_t> exe_rsp_list;
static list<axi_aw_t>  axi_aw_list;
static list<axi_w_t>   axi_w_list;
static list<axi_b_t>   axi_b_list;
static list<axi_ar_t>  axi_ar_list;
static list<axi_r_t>   axi_r_list;

static list<dec_req_t> in_flight_dec;
static list<exe_req_t> in_flight_exe;

typedef struct {
    logic_t   rstn;
    dec_req_t dec_req;
    logic_t   dec_req_valid;
    logic_t   dec_rsp_ready;
    exe_req_t exe_req;
    logic_t   exe_req_valid;
    logic_t   exe_rsp_ready;
    logic_t   axi_ar_ready;
    axi_r_t   axi_r;
    logic_t   axi_r_valid;
} next_t;

double sc_time_stamp() { return 0; }

void init_signals(dut_t *dut)
{
    dut->clk  = 0;
    dut->rstn = 0;

    dut->dec_req_id = 0;
    dut->dec_req_instr = 0;
    dut->dec_req_valid = 0;

    dut->dec_rsp_ready = 1;

    dut->exe_req_id = 0;
    dut->exe_req_instr = 0;
    dut->exe_req_rs_addr_0 = 0;
    dut->exe_req_rs_addr_1 = 0;
    dut->exe_req_rs_data_0 = 0;
    dut->exe_req_rs_data_1 = 0;
    dut->exe_req_vs_addr_0 = 0;
    dut->exe_req_vs_addr_1 = 0;
    dut->exe_req_vs_addr_2 = 0;
    dut->exe_req_vs_data_0 = {0};
    dut->exe_req_vs_data_1 = {0};
    dut->exe_req_vs_data_2 = {0};
    dut->exe_req_valid = 0;

    dut->exe_rsp_ready = 0;

    dut->axi_aw_ready = 0;

    dut->axi_w_ready = 0;

    dut->axi_b_id = 0;
    dut->axi_b_valid = 0;

    dut->axi_ar_ready = 0;

    dut->axi_r_id = 0;
    dut->axi_r_data = {0};
    dut->axi_r_valid = 0;
}

void set_dec_req(dut_t *dut, dec_req_t dec_req)
{
    dut->dec_req_id    = dec_req.id;
    dut->dec_req_instr = dec_req.instr;
}

dec_req_t get_dec_req(dut_t *dut)
{
    dec_req_t dec_req;
    dec_req.id    = dut->dec_req_id;
    dec_req.instr = dut->dec_req_instr;
    return dec_req;
}

dec_rsp_t get_dec_rsp(dut_t *dut)
{
    dec_rsp_t dec_rsp;
    dec_rsp.id         = dut->dec_rsp_id;
    dec_rsp.rd_clobber = dut->dec_rsp_rd_clobber;
    dec_rsp.vd_clobber = dut->dec_rsp_vd_clobber;
    dec_rsp.rs_read[0] = dut->dec_rsp_rs_read_0;
    dec_rsp.rs_read[1] = dut->dec_rsp_rs_read_1;
    dec_rsp.vs_read[0] = dut->dec_rsp_vs_read_0;
    dec_rsp.vs_read[1] = dut->dec_rsp_vs_read_1;
    dec_rsp.vs_read[2] = dut->dec_rsp_vs_read_2;
    dec_rsp.accept     = dut->dec_rsp_accept;
    return dec_rsp;
}

void set_exe_req(dut_t *dut, exe_req_t exe_req)
{
    dut->exe_req_id        = exe_req.id;
    dut->exe_req_instr     = exe_req.instr;
    dut->exe_req_rs_addr_0 = exe_req.rs_addr[0];
    dut->exe_req_rs_addr_1 = exe_req.rs_addr[1];
    dut->exe_req_rs_data_0 = exe_req.rs_data[0];
    dut->exe_req_rs_data_1 = exe_req.rs_data[1];
    dut->exe_req_vs_addr_0 = exe_req.vs_addr[0];
    dut->exe_req_vs_addr_1 = exe_req.vs_addr[1];
    dut->exe_req_vs_addr_2 = exe_req.vs_addr[2];
    memcpy(dut->exe_req_vs_data_0.data(), &exe_req.vs_data[0],
        sizeof(vec_data_t));
    memcpy(dut->exe_req_vs_data_1.data(), &exe_req.vs_data[1],
        sizeof(vec_data_t));
    memcpy(dut->exe_req_vs_data_2.data(), &exe_req.vs_data[2],
        sizeof(vec_data_t));
}

exe_req_t get_exe_req(const dut_t *dut)
{
    exe_req_t exe_req;
    exe_req.id         = dut->exe_req_id;
    exe_req.instr      = dut->exe_req_instr;
    exe_req.rs_addr[0] = dut->exe_req_rs_addr_0;
    exe_req.rs_addr[1] = dut->exe_req_rs_addr_1;
    exe_req.rs_data[0] = dut->exe_req_rs_data_0;
    exe_req.rs_data[1] = dut->exe_req_rs_data_1;
    exe_req.vs_addr[0] = dut->exe_req_vs_addr_0;
    exe_req.vs_addr[1] = dut->exe_req_vs_addr_1;
    exe_req.vs_addr[2] = dut->exe_req_vs_addr_2;
    memcpy(&exe_req.vs_data[0], dut->exe_req_vs_data_0.data(),
        sizeof(vec_data_t));
    memcpy(&exe_req.vs_data[1], dut->exe_req_vs_data_1.data(),
        sizeof(vec_data_t));
    memcpy(&exe_req.vs_data[2], dut->exe_req_vs_data_2.data(),
        sizeof(vec_data_t));
    return exe_req;
}

exe_rsp_t get_exe_rsp(dut_t *dut)
{
    exe_rsp_t exe_rsp;
    exe_rsp.id       = dut->exe_rsp_id;
    exe_rsp.rd_addr  = dut->exe_rsp_rd_addr;
    exe_rsp.rd_data  = dut->exe_rsp_rd_data;
    exe_rsp.rd_write = dut->exe_rsp_rd_write;
    exe_rsp.vd_addr  = dut->exe_rsp_vd_addr;
    memcpy(&exe_rsp.vd_data, dut->exe_rsp_vd_data.data(),
        sizeof(vec_data_t));
    exe_rsp.vd_write = dut->exe_rsp_vd_write;
    return exe_rsp;
}

axi_aw_t get_axi_aw(dut_t *dut)
{
    axi_aw_t axi_aw;
    axi_aw.id   = dut->axi_aw_id;
    axi_aw.addr = dut->axi_aw_addr;
    return axi_aw;
}

axi_w_t get_axi_w(dut_t *dut)
{
    axi_w_t axi_w;
    memcpy(&axi_w.data, dut->axi_w_data.data(),
        sizeof(vec_data_t));
    axi_w.strb = dut->axi_w_strb;
    return axi_w;
}

void set_axi_b(dut_t *dut, axi_b_t axi_b)
{
    dut->axi_b_id = axi_b.id;
}

axi_ar_t get_axi_ar(dut_t *dut)
{
    axi_ar_t axi_ar;
    axi_ar.id   = dut->axi_ar_id;
    axi_ar.addr = dut->axi_ar_addr;
    return axi_ar;
}

void set_axi_r(dut_t *dut, axi_r_t axi_r)
{
    dut->axi_r_id   = axi_r.id;
    memcpy(dut->axi_r_data.data(), &axi_r.data,
        sizeof(vec_data_t));
}

axi_r_t get_axi_r(dut_t *dut)
{
    axi_r_t axi_r;
    axi_r.id = dut->axi_r_id;
    memcpy(&axi_r.data, dut->axi_r_data.data(),
        sizeof(vec_data_t));
    return axi_r;
}

void handle_streams(dut_t *dut)
{

    /* axi aw */

    if (dut->axi_aw_valid && dut->axi_aw_ready) {
        axi_aw_t axi_aw = get_axi_aw(dut);
        axi_aw_list.push_back(axi_aw);

        VL_PRINTF("axi aw %s\n", format_axi_aw(axi_aw).c_str());
    }

    dut->axi_aw_ready = (axi_aw_list.size() < 10);

    /* axi w */

    if (dut->axi_w_valid && dut->axi_w_ready) {
        axi_w_t axi_w = get_axi_w(dut);
        axi_w_list.push_back(axi_w);

        VL_PRINTF("axi w %s\n", format_axi_w(axi_w).c_str());
    }

    dut->axi_w_ready = (axi_w_list.size() < 10);

    /* axi b */

    static axi_b_t axi_b;

    if (dut->axi_b_valid && dut->axi_b_ready) {
        dut->axi_b_valid = 0;

        VL_PRINTF("axi b %s\n", format_axi_b(axi_b).c_str());
    }

    if (!dut->axi_b_valid && !axi_b_list.empty()) {
        axi_b = axi_b_list.front();
        axi_b_list.pop_front();
        set_axi_b(dut, axi_b);
        dut->axi_b_valid = 1;
    }

    /* axi ar */

    if (dut->axi_ar_valid && dut->axi_ar_ready) {
        axi_ar_t axi_ar = get_axi_ar(dut);
        axi_ar_list.push_back(axi_ar);

        VL_PRINTF("axi ar %s\n", format_axi_ar(axi_ar).c_str());
    }

    dut->axi_ar_ready = (axi_ar_list.size() < 10);

    /* axi r */

    static axi_r_t axi_r;

    if (dut->axi_r_valid && dut->axi_r_ready) {
        dut->axi_r_valid = 0;

        VL_PRINTF("axi r %s\n", format_axi_r(axi_r).c_str());
    }

    if (!dut->axi_r_valid && !axi_r_list.empty()) {
        axi_r = axi_r_list.front();
        axi_r_list.pop_front();
        set_axi_r(dut, axi_r);
        dut->axi_r_valid = 1;
    }
}

next_t read_phase(dut_t *dut)
{
    next_t next = {0};

    if (Verilated::time() < 10) return next;

    /* rstn */

    next.rstn = 1;

    /* dec req */

    next.dec_req = get_dec_req(dut);
    next.dec_req_valid = dut->dec_req_valid;

    if (dut->dec_req_valid && dut->dec_req_ready) {
        VL_PRINTF("dec req %s\n", format_dec_req(next.dec_req).c_str());
        memset(&next.dec_req, 0, sizeof(dec_req_t));
        next.dec_req_valid = 0;
    }

    if (dec_req_list.size() > 0 && next.dec_req_valid == 0) {
        next.dec_req = dec_req_list.front();
        next.dec_req_valid = 1;
        dec_req_list.pop_front();
    }

    /* deq rsp */

    if (dut->dec_rsp_valid && dut->dec_rsp_ready) {
        dec_rsp_t dec_rsp = get_dec_rsp(dut);
        dec_rsp_list.push_back(dec_rsp);
        VL_PRINTF("dec rsp %s\n", format_dec_rsp(dec_rsp).c_str());
    }

    next.dec_rsp_ready = (dec_rsp_list.size() < 10);

    /* exe req */

    next.exe_req = get_exe_req(dut);
    next.exe_req_valid = dut->exe_req_valid;

    if (dut->exe_req_valid && dut->exe_req_ready) {
        VL_PRINTF("exe req %s\n", format_exe_req(next.exe_req).c_str());
        memset(&next.exe_req, 0, sizeof(exe_req_t));
        next.exe_req_valid = 0;
    }

    if (exe_req_list.size() > 0 && next.exe_req_valid == 0) {
        next.exe_req = exe_req_list.front();
        next.exe_req_valid = 1;
        exe_req_list.pop_front();
    }

    /* exe rsp */

    if (dut->exe_rsp_valid && dut->exe_rsp_ready) {
        exe_rsp_t exe_rsp = get_exe_rsp(dut);
        exe_rsp_list.push_back(exe_rsp);
        VL_PRINTF("exe rsp %s\n", format_exe_rsp(exe_rsp).c_str());
    };

    next.exe_rsp_ready = (exe_rsp_list.size() < 10);

    /* axi ar */

    if (dut->axi_ar_valid && dut->axi_ar_ready) {
        axi_ar_t axi_ar = get_axi_ar(dut);
        axi_ar_list.push_back(axi_ar);
        VL_PRINTF("axi ar %s\n", format_axi_ar(axi_ar).c_str());
    }

    next.axi_ar_ready = (axi_ar_list.size() < 10);

    /* axi r */

    next.axi_r = get_axi_r(dut);
    next.axi_r_valid = dut->axi_r_valid;

    if (dut->axi_r_valid && dut->axi_r_ready) {
        VL_PRINTF("axi r %s\n", format_axi_r(next.axi_r).c_str());
        memset(&next.axi_r, 0, sizeof(axi_r_t));
        next.axi_r_valid = 0;
    }

    if (axi_r_list.size() > 0 && next.axi_r_valid == 0 && Verilated::time() > 1000) {
        next.axi_r = axi_r_list.front();
        next.axi_r_valid = 1;
        axi_r_list.pop_front();
    }

    return next;
}

void write_phase(dut_t *dut, next_t next)
{
    dut->rstn = next.rstn;
    set_dec_req(dut, next.dec_req);
    dut->dec_req_valid = next.dec_req_valid;
    dut->dec_rsp_ready = next.dec_rsp_ready;
    set_exe_req(dut, next.exe_req);
    dut->exe_req_valid = next.exe_req_valid;
    dut->exe_rsp_ready = next.exe_rsp_ready;
    dut->axi_ar_ready = next.axi_ar_ready;
    set_axi_r(dut, next.axi_r);
    dut->axi_r_valid = next.axi_r_valid;
}

void verilator_step(dut_t *dut, VerilatedVcdC* vcd)
{
    dut->eval();

    next_t next = read_phase(dut);

    Verilated::timeInc(1);
    dut->clk = 1;
    dut->eval();
    vcd->dump(Verilated::time());

    write_phase(dut, next);

    Verilated::timeInc(1);
    dut->clk = 0;
    dut->eval();
    vcd->dump(Verilated::time());
}

static int counter = 0;

void issue(instr_t instr)
{
    dec_req_t dec_req = {
        .id = counter,
        .instr = instr
    };

    exe_req_t exe_req = {
        .id = counter,
        .instr = instr,
        .rs_addr = {0},
        .rs_data = {1,0},
        .vs_addr = {0},
        .vs_data = {0}
    };

    dec_req_list.push_back(dec_req);
    in_flight_dec.push_back(dec_req);
    exe_req_list.push_back(exe_req);
    in_flight_exe.push_back(exe_req);

    counter = (counter + 1) % 4;
}

void retire()
{
    while (!dec_rsp_list.empty()) {
        dec_rsp_t rsp = dec_rsp_list.front();
        dec_rsp_list.pop_front();

        auto it = find_if(in_flight_dec.begin(), in_flight_dec.end(),
            [rsp](const dec_req_t& req) { return req.id == rsp.id; });

        if (it != in_flight_dec.end()) {
            in_flight_dec.erase(it);
        } else {
            printf("panic!\n");
            exit(-1);
        }
    }

    while (!exe_rsp_list.empty()) {
        exe_rsp_t rsp = exe_rsp_list.front();
        exe_rsp_list.pop_front();

        auto it = find_if(in_flight_exe.begin(), in_flight_exe.end(),
            [rsp](const exe_req_t& req) { return req.id == rsp.id; });

        if (it != in_flight_exe.end()) {
            in_flight_exe.erase(it);
        } else {
            printf("panic!\n");
            exit(-1);
        }
    }

    if (
        dec_req_list.empty() &&
        dec_rsp_list.empty() &&
        in_flight_dec.empty() &&
        in_flight_exe.empty()
    ) {
        printf("All instructions retired.\n");
        Verilated::gotFinish(true);
    }
}


int main(int argc, char** argv, char** env) {
    Verilated::debug(0);
    Verilated::randReset(2);
    Verilated::traceEverOn(true);
    Verilated::commandArgs(argc, argv);

    dut_t* dut = new dut_t;

    VerilatedVcdC* vcd = new VerilatedVcdC;
    dut->trace(vcd, 99);
    vcd->open("output.vcd");

    // vbias
    issue(0x20000077); // vload
    issue(0x200000f7); // vload
    issue(0x20000177); // vload
    issue(0x200001f7); // vload
    issue(0x200010f7);
    issue(0x20001177);
    issue(0x200011f7);

    // vmacc
    issue(0x2021a0f7);

    VL_PRINTF("Simulation start\n");

    init_signals(dut);
    vcd->dump(Verilated::time());

    while (!Verilated::gotFinish()) {
        verilator_step(dut, vcd);

        if (axi_ar_list.size() > 0) {
            axi_ar_t axi_ar = axi_ar_list.front();
            axi_ar_list.pop_front();
            axi_r_list.push_back({
                .id   = axi_ar.id,
                .data = {0xDE, 0xAD, 0xBE, 0xEF, 0, 0, 0 ,0, 0, 0, 0, 0, 0, 0, 0, 0}
            });
        }

        retire();
    }

    VL_PRINTF("Simulation end\n");

    dut->final();
    vcd->close();
    delete dut;
    delete vcd;
    exit(0);
}
