#include <verilated.h>
#include <verilated_vcd_c.h>

#include "Vxadac_verilator.h"

vluint64_t main_time = 0;

double sc_time_stamp() {
    return main_time;
}

int main(int argc, char** argv, char** env) {

    Verilated::debug(0);
    Verilated::randReset(2);
    Verilated::traceEverOn(true);
    Verilated::commandArgs(argc, argv);

    Vxadac_verilator* dut = new Vxadac_verilator;

    VerilatedVcdC* vcd = new VerilatedVcdC;
    dut->trace(vcd, 99);
    vcd->open("output.vcd");

    VL_PRINTF("Simulation start\n");

    dut->clk = 0;
    while (!Verilated::gotFinish()) {
        ++main_time;
        dut->clk = !dut->clk;
        dut->rstn = (main_time < 10) ? 0 : 1;
        dut->eval();

        vcd->dump(main_time);
    }

    dut->final();

    vcd->close();
    delete vcd;
    delete dut;
    dut = nullptr;

    VL_PRINTF("Simulation end\n");
    exit(0);
}