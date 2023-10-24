# Paths =======================================================================

BUILD_DIR  := build
COMMON_DIR := common

OBJ_DIR := $(BUILD_DIR)/obj
DEP_DIR := $(BUILD_DIR)/dep
TGT_DIR := $(BUILD_DIR)/tgt

LDSCRIPT := link.ld

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
LD      := riscv32-unknown-elf-gcc
OBJCOPY := riscv32-unknown-elf-objcopy
OBJDUMP := riscv32-unknown-elf-objdump

PK = /opt/riscv/riscv32-unknown-elf/bin/pk 

# Flags =======================================================================

GFLAGS  := -march=rv32imc_zicsr_zifencei -mabi=ilp32 
CFLAGS  := -g -O2
LDFLAGS := 

# Common Rules ================================================================

COMMON_INCS := $(COMMON_DIR)

COMMON_SRCS_S := $(wildcard $(COMMON_DIR)/*.s)
COMMON_SRCS_C := $(wildcard $(COMMON_DIR)/*.c)
COMMON_SRCS   := $(COMMON_SRCS_S) $(COMMON_SRCS_C)

COMMON_OBJS_S := $(addprefix $(OBJ_DIR)/, $(COMMON_SRCS_S:.s=.o))
COMMON_OBJS_C := $(addprefix $(OBJ_DIR)/, $(COMMON_SRCS_C:.c=.o))
COMMON_OBJS   := $(COMMON_OBJS) $(COMMON_OBJS_S) $(COMMON_OBJS_C) 

-include $(COMMON_DEPS)

.PHONY: all
all: $(APPS)

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)

$(COMMON_OBJS_S): $(OBJ_DIR)/%.o: %.s
	@mkdir -p $(dir $@)
	$(AS) $< -o $@ -c $(GFLAGS)

$(COMMON_OBJS_C): $(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $< -o $@ -c $(GFLAGS) $(CFLAGS) $(addprefix -I, $(COMMON_INCS))

# App Macro ===================================================================

define app_macro

$(1)_TGT_ELF  := $$(TGT_DIR)/$(1).elf
$(1)_TGT_DUMP := $$(TGT_DIR)/$(1).dump

$(1)_INCS := $(COMMON_INCS) $(1)

$(1)_SRCS_S := $$(wildcard $(1)/*.s)
$(1)_SRCS_C := $$(wildcard $(1)/*.c)
$(1)_SRCS   := $$($(1)_SRCS_S) $$($(1)_SRCS_C)

$(1)_OBJS_S := $$(addprefix $$(OBJ_DIR)/, $$($(1)_SRCS_S:.s=.o))
$(1)_OBJS_C := $$(addprefix $$(OBJ_DIR)/, $$($(1)_SRCS_C:.c=.o))
$(1)_OBJS   := $$(COMMON_OBJS) $$($(1)_OBJS_S) $$($(1)_OBJS_C) 

$(1)_DEPS_C := $$(addprefix $$(OBJ_DIR)/, $$($(1)_SRCS_C:.c=.d))
$(1)_DEPS   := $$(COMMON_DEPS) $$($(1)_DEPS_C)

-include $$($(1)_DEPS)

.PHONY: $(1)
$(1): $$($(1)_TGT_ELF) $$($(1)_TGT_DUMP)

.PHONY: $(1)_spike
$(1)_spike: $$($(1)_TGT_ELF)
	spike --isa=rv32imc_zicsr_zifencei $(PK) $$($(1)_TGT_ELF)

$$($(1)_OBJS_S): $$(OBJ_DIR)/%.o: %.s
	@mkdir -p $$(dir $$@)
	$(AS) $$< -o $$@ -c $$(GFLAGS)

$$($(1)_OBJS_C): $$(OBJ_DIR)/%.o: %.c
	@mkdir -p $$(dir $$@)
	$$(CC) $$< -o $$@ -c $$(GFLAGS) $$(CFLAGS) $$(addprefix -I, $$($(1)_INCS))

$$($(1)_TGT_ELF): $(LDSCRIPT) $$($(1)_OBJS) 
	@mkdir -p $$(dir $$@)
	$$(LD) $$($(1)_OBJS) -o $$@ $$(LDFLAGS)

$$($(1)_TGT_DUMP): $$($(1)_TGT_ELF)
	@mkdir -p $$(dir $$@)
	$$(OBJDUMP) -S -D $$^ > $$@

endef

$(foreach APP,$(APPS),$(eval $(call app_macro,$(APP))))
