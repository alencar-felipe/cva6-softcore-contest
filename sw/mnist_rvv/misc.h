#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "config.h"

#define STR_(x) #x
#define STR(x) STR_(x)

// config =====================================================================

#define TAG STR([IMAGE]) " "

// assert =====================================================================

#define ASSERT(condition) \
    do { \
        if (!(condition)) { \
            printf(TAG " Assertion failed: %s:%d\n", \
                __FILE__, __LINE__); \
        } \
    } while (0)

// csr ========================================================================

#define READ_CSR(reg) ({ \
    unsigned long __tmp; \
    asm volatile ("csrr %0, " #reg : "=r"(__tmp)); \
  __tmp; \
})

#define WRITE_CSR(reg, val) ({ \
  asm volatile ("csrw " #reg ", %0" :: "rK"(val)); \
})

// crc32 ======================================================================

#define CRC32_POLY (0xEDB88320)

void crc32_table_init(void);
void crc32(uint32_t *crc, const void *buf, size_t len);

// hexdump ====================================================================

void hexdump(const void *buf, size_t len);

// decode_img =================================================================

void decode_img(const void *buf, void **image, int32_t *label);