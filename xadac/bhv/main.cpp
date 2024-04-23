#include <string.h>

#include <list>
#include <mutex>
#include <thread>

#include <verilated.h>
#include <verilated_vcd_c.h>

#include "xadac.h"

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

    if (dut->dec_req_valid && dut->dec_req_ready) {
        VL_PRINTF("dec req\n");
        dut->dec_req_valid = 0;
    }

    if (!dut->dec_req_valid && !dec_req_list.empty()) {
        set_dec_req(dut, dec_req_list.front());
        dec_req_list.pop_front();
        dut->dec_req_valid = 1;
    }

    /* dec rsp */

    if (dut->dec_rsp_valid && dut->dec_rsp_ready) {
        VL_PRINTF("dec rsp\n");
        dec_rsp_list.push_back(get_dec_rsp(dut));
    }

    dut->dec_rsp_ready = (dec_req_list.size() < 10);

    /* exe req */

    if (dut->exe_req_valid && dut->exe_req_ready) {
        VL_PRINTF("exe req\n");
        dut->exe_req_valid = 0;
    }

    if (!dut->exe_req_valid && !exe_req_list.empty()) {
        set_exe_req(dut, exe_req_list.front());
        exe_req_list.pop_front();
        dut->exe_req_valid = 1;
    }

    /* exe rsp */

    if (dut->exe_rsp_valid && dut->exe_rsp_ready) {
        VL_PRINTF("exe rsp\n");
        exe_rsp_list.push_back(get_exe_rsp(dut));
    }

    dut->exe_rsp_ready = (exe_rsp_list.size() < 10);

    /* axi aw */

    if (dut->axi_aw_valid && dut->axi_aw_ready) {
        axi_aw_list.push_back(get_axi_aw(dut));
    }

    dut->axi_aw_ready = (axi_aw_list.size() < 10);

    /* axi w */

    if (dut->axi_w_valid && dut->axi_w_ready) {
        VL_PRINTF("axi w\n");
        axi_w_list.push_back(get_axi_w(dut));
    }

    dut->axi_w_ready = (axi_w_list.size() < 10);

    /* axi b */

    if (dut->axi_b_valid && dut->axi_b_ready) {
        VL_PRINTF("axi b\n");
        dut->axi_b_valid = 0;
    }

    if (!dut->axi_b_valid && !axi_b_list.empty()) {
        set_axi_b(dut, axi_b_list.front());
        axi_b_list.pop_front();
        dut->axi_b_valid = 1;
    }

    /* axi ar */

    if (dut->axi_ar_valid && dut->axi_ar_ready) {
        VL_PRINTF("axi ar\n");
        axi_ar_list.push_back(get_axi_ar(dut));
    }

    dut->axi_ar_ready = (axi_ar_list.size() < 10);

    /* axi r */

    if (dut->axi_r_valid && dut->axi_r_ready) {
        VL_PRINTF("axi r\n");
        dut->axi_r_valid = 0;
    }

    if (!dut->axi_r_valid && !axi_r_list.empty()) {
        set_axi_r(dut, axi_r_list.front());
        axi_r_list.pop_front();
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
        .instr = 0
    });

    while (!Verilated::gotFinish()) {
        Verilated::timeInc(1);

        dut->clk = !dut->clk;

        if (Verilated::time() < 10) {
            dut->rstn = 0;
        } else {
            dut->rstn = 1;
            handle_streams(dut);
        }

        dut->eval();
        vcd->dump(Verilated::time());
    }

    dut->final();

    vcd->close();
    delete vcd;
    delete dut;
    dut = nullptr;

    VL_PRINTF("Simulation end\n");
    exit(0);
}
