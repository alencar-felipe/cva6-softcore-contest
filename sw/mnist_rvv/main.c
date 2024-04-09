#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <riscv_vector.h>

#include "misc.h"
#include "network.h"
#include "resources/all.h"

int main() {

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
    instret = -READ_CSR(minstret);
    cycles = -READ_CSR(mcycle);
#endif

    int32_t output;
    uint8_t credence;
    inference(input, &output, &credence);

#ifdef PERFORMANCE_RUN
    instret += READ_CSR(minstret);
    cycles += READ_CSR(mcycle);
#endif

    printf(TAG "expected: %ld\n", expected);
    printf(TAG "output: %ld\n", output);
    printf(TAG "credence: %d\n", credence);

#ifdef PERFORMANCE_RUN
    printf(TAG "instrutions: %u\n", instret);
    printf(TAG "cycles: %u\n", cycles);
#endif

}
