#include <string.h>

#include <list>
#include <mutex>
#include <thread>

#include <verilated.h>
#include <verilated_vcd_c.h>

#include "types.hpp"
#include "format.hpp"

using namespace std;

list<dec_req_t> dec_req_list;
list<dec_rsp_t> dec_rsp_list;
list<exe_req_t> exe_req_list;
list<exe_rsp_t> exe_rsp_list;
list<axi_aw_t>  axi_aw_list;
list<axi_w_t>   axi_w_list;
list<axi_b_t>   axi_b_list;
list<axi_ar_t>  axi_ar_list;
list<axi_r_t>   axi_r_list;

double sc_time_stamp() { return 0; }

void init_io(dut_t *dut)
{
    dut->clk  = 0;
    dut->rstn = 0;

    dut->dec_req_id = 0;
    dut->dec_req_instr = 0;
    dut->dec_req_valid = 0;

    dut->dec_rsp_ready = 0;

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

void handle_streams(dut_t *dut)
{

    /* dec req */

    static dec_req_t dec_req;

    if (dut->dec_req_valid && dut->dec_req_ready) {
        dut->dec_req_valid = 0;

        VL_PRINTF("dec req %s\n", format_dec_req(dec_req).c_str());
    }

    if (!dut->dec_req_valid && !dec_req_list.empty()) {
        dec_req = dec_req_list.front();
        dec_req_list.pop_front();
        set_dec_req(dut, dec_req);
        dut->dec_req_valid = 1;
    }

    /* dec rsp */

    if (dut->dec_rsp_valid && dut->dec_rsp_ready) {
        dec_rsp_t dec_rsp = get_dec_rsp(dut);
        dec_rsp_list.push_back(dec_rsp);

        VL_PRINTF("dec rsp %s\n", format_dec_rsp(dec_rsp).c_str());
    }

    dut->dec_rsp_ready = (dec_req_list.size() < 10);

    /* exe req */

    static exe_req_t exe_req;

    if (dut->exe_req_valid && dut->exe_req_ready) {
        dut->exe_req_valid = 0;

        VL_PRINTF("exe rep %s\n", format_exe_req(exe_req).c_str());
    }

    if (!dut->exe_req_valid && !exe_req_list.empty()) {
        exe_req = exe_req_list.front();
        exe_req_list.pop_front();
        set_exe_req(dut, exe_req);
        dut->exe_req_valid = 1;
    }

    /* exe rsp */

    if (dut->exe_rsp_valid && dut->exe_rsp_ready) {
        exe_rsp_t exe_rsp = get_exe_rsp(dut);
        exe_rsp_list.push_back(exe_rsp);

        VL_PRINTF("exe rsp %s\n", format_exe_rsp(exe_rsp).c_str());
    }

    dut->exe_rsp_ready = (exe_rsp_list.size() < 10);

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

int main(int argc, char** argv, char** env) {
    Verilated::debug(0);
    Verilated::randReset(2);
    Verilated::traceEverOn(true);
    Verilated::commandArgs(argc, argv);

    dut_t* dut = new dut_t;

    VerilatedVcdC* vcd = new VerilatedVcdC;
    dut->trace(vcd, 99);
    vcd->open("output.vcd");

    VL_PRINTF("Simulation start\n");

    init_io(dut);

    dec_req_list.push_back({
        .id = 0,
        .instr = 0x0221a0f7
    });

    exe_req_list.push_back({
        .id = 0,
        .instr = 0x0221a0f7,
        .rs_addr = {0},
        .rs_data = {0},
        .vs_addr = {0},
        .vs_data = {0}
    });

    axi_r_list.push_back({
        .id = 0,
        .data = 0
    });

    while (!Verilated::gotFinish()) {
        dut->clk = !dut->clk;
        dut->eval();

        if (dut->clk && Verilated::time() > 20) {
            handle_streams(dut);
        }

        // for (int i = 0; i < 10; i++) dut->eval();

        vcd->dump(Verilated::time());
        Verilated::timeInc(1);
    }

    VL_PRINTF("Simulation end\n");

    dut->final();
    vcd->close();
    delete dut;
    delete vcd;
    exit(0);
}
