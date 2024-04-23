VERILATOR  = verilator
CXX        = g++
CXXFLAGS   = -Wall -O3
LDFLAGS    = -lverilated -lverilated_vcd_c

AXI_DIR = ../vendor/pulp-platform/axi
OBJ_DIR = ./obj

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
	xadac_verilator.sv \
	xadac_vload.sv \
	xadac_vmacc.sv \
	xadac_vrf_phy.sv \
	xadac_vrf.sv \
	xadac.sv

INC_FLAGS = $(foreach dir, $(INC), -I$(dir))

CPP_OUTPUT = $(OBJ_DIR)/$(TOP)_verilated.cpp
OBJ_OUTPUT = $(OBJ_DIR)/$(TOP)_verilated.o
EXE_OUTPUT = $(OBJ_DIR)/$(TOP)_sim

$(shell mkdir -p $(OBJ_DIR))

verilate: $(SRC)
	$(VERILATOR) --cc $(SRC) $(INC_FLAGS) --exe -o $(CPP_OUTPUT)

compile: verilate
	$(CXX) $(CXXFLAGS) -c $(CPP_OUTPUT) -o $(OBJ_OUTPUT)
	$(CXX) $(CXXFLAGS) $(OBJ_DIR)/$(TOP).o -o $(EXE_OUTPUT) $(LDFLAGS)

run: compile
	./$(EXE_OUTPUT)

clean:
	rm -rf $(OBJ_DIR)

.PHONY: verilate compile run clean
