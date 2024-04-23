VERILATOR  = verilator
CXX        = g++
CXXFLAGS   = -Wall -O3
LDFLAGS    = -lverilated -lverilated_vcd_c

AXI_DIR   = ../vendor/pulp-platform/axi
BUILD_DIR = ./build

TOP = xadac_verilator

INC = \
	$(AXI_DIR)/include

SRC = \
	$(AXI_DIR)/src/axi_pkg.sv \
	$(AXI_DIR)/src/axi_intf.sv \
	xadac_pkg.sv \
	xadac_if.sv \
	xadac_mux.sv \
	xadac_vactv.sv \
	xadac_vbias.sv \
	xadac_vclobber.sv \
	xadac_vload.sv \
	xadac_vmacc.sv \
	xadac_vrf_phy.sv \
	xadac_vrf.sv \
	xadac.sv \
	xadac_verilator.sv \
	xadac_verilator.cpp

VFLAGS = \
	--exe --cc \
	--Wno-fatal \
	-Mdir $(BUILD_DIR) \
	$(foreach dir, $(INC), -I$(dir)) \
	--top $(TOP) \
	--trace

$(shell mkdir -p $(OBJ_DIR))

verilate: $(SRC)
	$(VERILATOR) $(SRC) $(VFLAGS)

compile:
	$(VERILATOR) $(SRC) $(VFLAGS) --build

run: compile
	$(BUILD_DIR)/V$(TOP)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: verilate compile run clean
