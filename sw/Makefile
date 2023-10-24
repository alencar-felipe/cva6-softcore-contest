# Paths =======================================================================

APP_DIR    := app
BSP_DIR    := bsp
COMMON_DIR := common
BUILD_DIR  := build

BSP_CONFIG_DIR := $(BSP_DIR)/config
BSP_DRIVERS_DIR := $(BSP_DIR)/drivers
BSP_HAL_DIR := $(BSP_DIR)/hal

OBJ_DIR := $(BUILD_DIR)/obj
DEP_DIR := $(BUILD_DIR)/dep
TGT_DIR := $(BUILD_DIR)/tgt

LDSCRIPT := link.ld

# Apps ========================================================================

# APPS := \
# 	coremark \
# 	dhrystone \
# 	helloworld \
# 	helloworld_printf \
# 	median \
# 	mnist \
# 	multiply \
# 	pmp \
# 	qsort \
# 	rsort \
# 	spmv \
# 	towers \
# 	vvadd

APPS := mnist

# Toolchain ==================================================================

AS      := riscv32-unknown-elf-as
CC      := riscv32-unknown-elf-gcc
CXX     := riscv32-unknown-elf-g++
LD      := riscv32-unknown-elf-g++
OBJCOPY := riscv32-unknown-elf-objcopy
OBJDUMP := riscv32-unknown-elf-objdump

# Flags =======================================================================

GFLAGS := -march=rv32imc_zicsr_zifencei -mabi=ilp32

CFLAGS := -g -O2 -Wall -Wextra -static -mcmodel=medany -ffunction-sections \
	-fdata-sections  -fstrict-volatile-bitfields

CXXFLAGS := -g -O2 -Wall -Wextra -static -mcmodel=medany -ffunction-sections \
	-fdata-sections -fstrict-volatile-bitfields

LDFLAGS := -Wl,--gc-sections -nostdlib -nostartfiles

# Gloabal Rules ===============================================================

.PHONY: all
all: $(APPS)

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)

# App Macro ===================================================================

define app_macro
$(1)_APP_DIR  := $$(APP_DIR)/$(1)
$(1)_TGT_ELF  := $$(TGT_DIR)/$(1).elf
$(1)_TGT_DUMP := $$(TGT_DIR)/$(1).dump

$(1)_INCS := \
	$$(BSP_CONFIG_DIR) \
	$$(BSP_DRIVERS_DIR) \
	$$(BSP_HAL_DIR) \
	$$(COMMON_DIR) \
	$$($(1)_APP_DIR)

$(1)_SRCS_S   := $$(foreach dir, $$($(1)_INCS), $$(wildcard $$(dir)/*.s))
$(1)_SRCS_C   := $$(foreach dir, $$($(1)_INCS), $$(wildcard $$(dir)/*.c))
$(1)_SRCS_CXX := $$(foreach dir, $$($(1)_INCS), $$(wildcard $$(dir)/*.cpp))

$(1)_OBJS_S   := $$(addprefix $$(OBJ_DIR)/, $$($(1)_SRCS_S:.s=.o))
$(1)_OBJS_C   := $$(addprefix $$(OBJ_DIR)/, $$($(1)_SRCS_C:.c=.o))
$(1)_OBJS_CXX := $$(addprefix $$(OBJ_DIR)/, $$($(1)_SRCS_CXX:.cpp=.o))

$(1)_OBJS := $$($(1)_OBJS_C) $$($(1)_OBJS_CXX) $$($(1)_OBJS_S)

$(1)_DEPS_C   := $$(addprefix $$(OBJ_DIR)/, $$($(1)_SRCS_C:.c=.d))
$(1)_DEPS_CXX := $$(addprefix $$(OBJ_DIR)/, $$($(1)_SRCS_CXX:.cpp=.d))

$(1)_DEPS := $$($(1)_DEPS_C) $$($(1)_DEPS_CXX)

.PHONY: $(1)
$(1): $$($(1)_TGT_ELF) $$($(1)_TGT_DUMP)
	# EMPTY

$$($(1)_OBJS_C): $$(OBJ_DIR)/%.o: %.c
	@mkdir -p $$(dir $$@)
	$$(CC) $$< -o $$@ -c $$(GFLAGS) $$(CFLAGS) $$(addprefix -I, $$($(1)_INCS))

$$($(1)_TGT_ELF): $$($(1)_OBJS) $(LDSCRIPT)
	@mkdir -p $$(dir $$@)
	$$(LD) $$($(1)_OBJS) -o $$@ $$(GFLAGS) $$(LDFLAGS) -T $$(LDSCRIPT)

$$($(1)_TGT_DUMP): $$($(1)_TGT_ELF)
	@mkdir -p $$(dir $$@)
	$$(OBJDUMP) -S -D $$^ > $$@

$$(info $$($(1)_OBJS))

endef

$(foreach APP,$(APPS),$(eval $(call app_macro,$(APP))))
