#include "mnist.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "env.h"
#include "nnet.h"
#include "util.h"

static void decode_stimulus(
    const uint8_t *raw,
    uint8_t **image,
    int32_t *label
) {
    *image = (int8_t *) (raw += 13);
    *label = *(int32_t *) (raw += IMAGE_W*IMAGE_H);
}

int main() {
    size_t instret, cycles;

    uint8_t *image;

    int32_t expected;
    int32_t predicted;
    uint8_t credence;

    printf("IM ALIVE\n");

    decode_stimulus(stimulus_0003, &image, &expected);

    // instret = -read_csr(minstret);
    // cycles = -read_csr(mcycle);

    propagate(image, &predicted, &credence);

    // instret += read_csr(minstret);
    // cycles += read_csr(mcycle);
    
    printf("expected  = %d\n", expected);
    printf("predicted = %d\n", predicted);
    printf("credence  = %d\n", credence);
    // printf("image %s: %d instructions\n", stringify(MNIST_INPUT_IMAGE), (int)(instret));
    // printf("image %s: %d cycles\n", stringify(MNIST_INPUT_IMAGE), (int)(cycles));
}
