#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <riscv_vector.h>

#include "network.h"
#include "resources/all.h"

#define VL (4)

int32_t a_mem[VL] = {0, 1, 2, 3};
int32_t b_mem[VL] = {4, 5, 6, 7};
int32_t c_mem[VL] = {9, 9, 9, 9};

int main() {

#ifdef PERFORMANCE_RUN
    printf(TAG "Performance Run\n");
#endif

#ifdef VALIDATION_RUN
    printf(TAG "Validation Run\n");
#endif

    ASSERT(__riscv_vsetvl_e32m1(VL) == VL);

    vint32m1_t a, b, c;

    a = __riscv_vle32_v_i32m1(a_mem, VL);
    b = __riscv_vle32_v_i32m1(b_mem, VL);
    c = __riscv_vadd_vv_i32m1(a, b, VL);
    __riscv_vse32_v_i32m1(c_mem, c, VL);

    for (size_t i = 0; i < VL; i++) {
        printf("%d ", c_mem[i]);
    }
    printf("\n");

    uint8_t *input;
    int32_t expected;
    decode_img(img0003, (void **) &input, &expected);

#ifdef PERFORMANCE_RUN
    size_t instret, cycles;
    instret = -read_csr(minstret);
    cycles = -read_csr(mcycle);
#endif

    int32_t output;
    uint8_t credence;
    inference(input, &output, &credence);

#ifdef PERFORMANCE_RUN
    instret += read_csr(minstret);
    cycles += read_csr(mcycle);
#endif

    printf(TAG "expected: %d\n", expected);
    printf(TAG "output: %ld\n", output);
    printf(TAG "credence: %d\n", credence);

#ifdef PERFORMANCE_RUN
    printf(TAG "instrutions: %ld\n", instret);
    printf(TAG "cycles: %ld\n", cycles);
#endif

}
