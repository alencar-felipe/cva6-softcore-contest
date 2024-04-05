#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <riscv_vector.h>

#include "network.h"
#include "resources/all.h"

int main() {

    // Set RVV rounding mode to round-down (truncate).
    asm volatile ("csrw vxrm, %0" :: "rK"(0b10));

#ifdef PERFORMANCE_RUN
    printf(TAG "Performance Run\n");
#endif

#ifdef VALIDATION_RUN
    printf(TAG "Validation Run\n");
#endif

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
