# Directories =================================================================

APP_DIR   := ./app
BSP_DIR   := ./bsp
BUILD_DIR := ./build

OBJ_DIR := $(BUILD_DIR)/obj
DEP_DIR := $(BUILD_DIR)/dep
TGT_DIR := $(BUILD_DIR)/tgt

# Apps ========================================================================

APPS := \
	coremark \
	dhrystone \
	helloworld \
	helloworld_printf \
	median \
	mnist \
	multiply \
	pmp \
	qsort \
	rsort \
	spmv \
	towers \
	vvadd

# Toolchain ==================================================================

AS      := riscv32-unknown-elf-as
CC      := riscv32-unknown-elf-gcc
CXX     := riscv32-unknown-elf-g++
LD      := riscv32-unknown-elf-g++
OBJCOPY := riscv32-unknown-elf-objcopy
OBJDUMP := riscv32-unknown-elf-objdump

# Flags =======================================================================

GFLAGS := -march=rv32imc -mabi=ilp32

CFLAGS := -g -O2 -Wall -Wextra -static -mcmodel=medany -ffunction-sections \
	-fdata-sections  -fstrict-volatile-bitfields

CXXFLAGS := -g -O2 -Wall -Wextra -static -mcmodel=medany -ffunction-sections \
	-fdata-sections -fstrict-volatile-bitfields

LDFLAGS := -Wl,--gc-sections -nostdlib -nostartfiles

# Rules =======================================================================

.PHONY: all
all: $(APPS)

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)

define make_app_rules
L_APP      := $(1)
L_APP_DIR  := $$(APP_DIR)/$$(L_APP)
L_TGT_ELF  := $$(TGT_DIR)/$$(L_APP).elf
L_TGT_DUMP := $$(TGT_DIR)/$$(L_APP).dump

L_SRCS := $$(wildcard \
 	$$(L_APP_DIR)/*.s \
 	$$(L_APP_DIR)/*.c \
 	$$(L_APP_DIR)/*.cpp \
)

L_OBJS := \
	$$(patsubst %.s,$$(OBJ_DIR)/%.o,$$(filter %.s,$$(L_SRCS))) \
	$$(patsubst %.c,$$(OBJ_DIR)/%.o,$$(filter %.c,$$(L_SRCS))) \
    $$(patsubst %.cpp,$$(OBJ_DIR)/%.o,$$(filter %.cpp,$$(L_SRCS)))

L_DEPS := \
	$$(patsubst %.c,$$(DEP_DIR)/%.d,$$(filter %.c,$$(L_SRCS))) \
	$$(patsubst %.cpp,$$(DEP_DIR)/%.d,$$(filter %.cpp,$$(L_SRCS)))

.PHONY: $$(L_APP)
$$(L_APP): $$(L_TGT_ELF) $$(L_TGT_DUMP)

$$(info $$(L_OBJS))

$$(L_OBJS): %.o: %
	@mkdir -p $(dir $$@)
	@if [ "$$(echo $$< | grep '.s$$')" != "" ]; then \
		$(AS) $$< -o $$@ -c $$(GFLAGS) $$(APP_FLAGS); \
	elif [ "$$(echo $$< | grep '.c$$')" != "" ]; then \
		$(CC) $$< -o $$@ -c $$(GFLAGS) $(CFLAGS) $(APP_FLAGS); \
	else \
		$(CXX) $$< -o $$@ -c $(GFLAGS) $(CXXFLAGS) $(APP_FLAGS); \
	fi






endef

$(foreach APP,$(APPS),$(eval $(call make_app_rules,$(APP))))

# $(TGT_DIR)/%.elf: $(OBJ_DIR)/%/*.o $(LDSCRIPT) # Note: This assumes each app has its objects in its subdirectory under $(OBJ_DIR)
# 	@mkdir -p $(dir $@)
# 	$(LD) $^ -o $@ $(GFLAGS) $(LDFLAGS) -T $(LDSCRIPT)

# $(TGT_DIR)/%.dump: $(TGT_DIR)/%.elf
# 	@mkdir -p $(dir $@)
# 	$(OBJDUMP) -S -D $^ > $@

# #--------------------------------------------------------------------
# # Build rules
# #--------------------------------------------------------------------

# RISCV_PREFIX ?= riscv$(XLEN)-unknown-elf-
# RISCV_GCC ?= $(RISCV_PREFIX)gcc
# RISCV_OBJCOPY ?= $(RISCV_PREFIX)objcopy
# RISCV_AR ?= $(RISCV_PREFIX)ar
# RISCV_OBJDUMP ?= $(RISCV_PREFIX)objdump


# SRC_BSP_C=$(wildcard $(bsp_dir)/hal/*.c) $(wildcard $(bsp_dir)/drivers/uart/*.c) 
# SRC_BSP_S=$(wildcard $(bsp_dir)/hal/*.S)


# OBJ_BSP_C=$(SRC_BSP_C:.c=.o)
# OBJ_BSP_S=$(SRC_BSP_S:.S=.o)
# OBJ_BSP= $(OBJ_BSP_S) $(OBJ_BSP_C)


# OBJ_APP=$(wildcard $(src_dir)/*.o) 

# INCS += -I$(src_dir)/../bsp/config \
# 	-I$(src_dir)/../bsp/drivers/uart \
# 	-I$(src_dir)/../bsp/hal \
# 	-I$(src_dir)/coremark/simple \
# 	-I$(src_dir)/common \
# 	$(addprefix -I$(src_dir)/, $(bmarks))
	
# FLAGS_STR:="$(RISCV_CFLAGS)  $(RISCV_LDFLAGS) "

# CFLAGS ?=-DPREALLOCATE=1 -fvisibility=hidden -DSTDIO_THRU_UART -O3 -mcmodel=medany   -static  -Wall -pedantic 

# RISCV_CFLAGS :=-DPERFORMANCE_RUN=1 \
# 		-DITERATIONS=3 \
# 		-DFLAGS_STR=\"$(FLAGS_STR)\" \
# 		-DPREALLOCATE=1 -fvisibility=hidden -DSTDIO_THRU_UART -O3 -mcmodel=medany   -static  -Wall -pedantic 

# RISCV_LDFLAGS=-L./ -lcva6 -static -nostartfiles -T $(src_dir)/../bsp/config/link.ld



# incs  += -I$(src_dir)/../env -I$(src_dir)/common $(addprefix -I$(src_dir)/, $(bmarks))
# objs  :=


# define generate_rules =
# $(1)_SRC_DIR = ./$(1)/
# $(1)_SRC_FILES = $$(shell find $$($(1)_SRC_DIR) -name "*.c")
# $(1)_OBJ_FILES = $$($(1)_SRC_FILES:.c=.o)
# ALL_OBJ += $$($(1)_OBJ_FILES)
# $(1): $(1).D $(1).riscv
# $(1): $(1).hex $(1).riscv
# $(1): $(1).bin $(1).riscv
# $(1): $(1).mem $(1).bin
# $(1): $(1).coe $(1).mem
# $(1).riscv: libcva6.a $$($(1)_OBJ_FILES)
# 	# LINKING
# 	$$(RISCV_GCC) $$(RISCV_CFLAGS) -o $$@ $$^ $$(RISCV_LDFLAGS)
# endef

# $(foreach bmark,$(bmarks),$(eval $(call generate_rules,$(bmark))))

# %.D: %.riscv 
# 	$(RISCV_OBJDUMP) -D $< > $@

# %.hex: %.riscv
# 	$(RISCV_OBJCOPY) -O ihex $< $@

# %.bin: %.riscv
# 	$(RISCV_OBJCOPY) -O binary $< $@

# libcva6.a: $(OBJ_BSP)
# 	$(RISCV_AR) rcs libcva6.a $(OBJ_BSP)

# %.o: %.c
# 	$(RISCV_GCC) $(INCS) -o $@ -c $< $(RISCV_CFLAGS)

# %.o: %.S
# 	$(RISCV_GCC) -o $@ -c $< $(CFLAGS)

# %.mem: %.bin
# 	$(utils_dir)/bin2mem.py $(src_dir)/$<

# %.coe: %.mem
# 	$(utils_dir)/mem2coe.py $(src_dir)/$<


# all: $(bmarks)


# clean:
# 	rm -rf $(OBJ_BSP) $(ALL_OBJ) libcva6.a *.riscv *.o *.D *.mem *.bin *.coe


