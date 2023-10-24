cmake_minimum_required(VERSION 3.16)
project(RISCV_APPS)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER riscv32-unknown-elf-gcc)
set(CMAKE_CXX_COMPILER riscv32-unknown-elf-g++)
set(CMAKE_OBJDUMP riscv32-unknown-elf-objdump)
set(PK /opt/riscv/riscv32-unknown-elf/bin/pk)

# Set the main paths
set(COMMON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/common")
set(LDSCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/link.ld")

# Set compiler flags
set(ARCH "rv32imc_zicsr_zifencei")
add_compile_definitions(ARCH=${ARCH})
add_compile_options(-march=${ARCH} -mabi=ilp32 -g -O2)

# Include common directory
include_directories(${COMMON_DIR})

# Common sources
file(GLOB COMMON_SRCS "${COMMON_DIR}/*.s" "${COMMON_DIR}/*.c")

# Define APPS and iterate over them # coremark 
set(APPS dhrystone helloworld helloworld_printf median mnist multiply pmp qsort rsort spmv towers vvadd)

foreach(APP ${APPS})
    # Gather app sources
    file(GLOB APP_SRCS "${APP}/*.s" "${APP}/*.c")
    
    # Define the executable
    add_executable(${APP} ${COMMON_SRCS} ${APP_SRCS})
    target_include_directories(${APP} PRIVATE ${APP})
    
    # If there are specific linking requirements, like a custom linker script, add them here
    # target_link_options(${APP} PRIVATE -T ${LDSCRIPT})
    
    # If you need the dump file
    add_custom_command(TARGET ${APP} POST_BUILD
        COMMAND ${CMAKE_OBJDUMP} -S -D $<TARGET_FILE:${APP}> > ${APP}.dump
        COMMENT "Generating ${APP}.dump"
    )

    add_custom_target(${APP}_spike DEPENDS ${APP}_spike.phony)
    add_custom_command(
        OUTPUT ${APP}_spike.phony
        COMMAND spike --isa=${ARCH} ${PK} $<TARGET_FILE:${APP}>
        DEPENDS ${APP}
        COMMENT "Simulating ${APP} with Spike"
    )
    
endforeach()
