#!/bin/bash

set -e

riscv32-unknown-elf-gcc -Og -g \
    -march=rv32im -mabi=ilp32 --specs=nano.specs \
    -fno-inline-small-functions \
    -I ../common/ -I ../../../tb/riscv-isa-sim/riscv/ \
    *.c -o mnist

riscv32-unknown-elf-nm mnist > nm.txt

spike -g --isa=rv32imc /opt/riscv/riscv32-unknown-elf/bin/pk \
    mnist 2>histogram.txt
