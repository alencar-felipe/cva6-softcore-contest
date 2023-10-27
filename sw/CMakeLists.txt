cmake_minimum_required(VERSION 3.16)
project(RISCV_APPS)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER riscv32-unknown-elf-gcc)
set(CMAKE_CXX_COMPILER riscv32-unknown-elf-g++)
set(CMAKE_OBJDUMP riscv32-unknown-elf-objdump)
set(PK /opt/riscv/riscv32-unknown-elf/bin/pk)

set(COMMON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/common")
set(LDSCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/link.ld")

set(ARCH "rv32imc_zicsr_zifencei")
add_compile_definitions(ARCH=${ARCH})
add_compile_options(-march=${ARCH} -mabi=ilp32 -g -O2 -flto)

include_directories(${COMMON_DIR})

file(GLOB_RECURSE COMMON_SRCS "${COMMON_DIR}/*.s" "${COMMON_DIR}/*.c")

set(APPS dhrystone helloworld helloworld_printf median mnist multiply pmp
    qsort rsort spmv towers vvadd)

foreach(APP ${APPS})
    file(GLOB_RECURSE APP_SRCS "${APP}/*.s" "${APP}/*.c")
    
    add_executable(${APP} ${COMMON_SRCS} ${APP_SRCS})
    target_include_directories(${APP} PRIVATE ${APP})
    
    # target_link_options(${APP} PRIVATE -T ${LDSCRIPT})
    
    add_custom_command(TARGET ${APP} POST_BUILD
        COMMAND ${CMAKE_OBJDUMP} -S -D $<TARGET_FILE:${APP}> > ${APP}.dump
        COMMENT "Generating ${APP}.dump"
    )

    add_custom_target(${APP}_spike DEPENDS ${APP}_spike.phony)
    add_custom_command(
        OUTPUT ${APP}_spike.phony
        COMMAND spike --isa=${ARCH} ${PK} $<TARGET_FILE:${APP}>
        DEPENDS ${APP}
        COMMENT "Launching ${APP} with Spike"
    )
    
endforeach()
