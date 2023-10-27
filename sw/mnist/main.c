#include "mnist.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "env.h"
#include "Network.h"
#include "util.h"

static void decode_stimulus(uint8_t *raw, uint8_t **image, int32_t *label)
{
    *image = (int8_t *) (raw = raw + 13);
    *label = *(int32_t *) (raw = raw + IMAGE_W*IMAGE_H);
}

int processInput(        uint_t* inputBuffer,
                            long_t* expectedOutputBuffer,
                            long_t* predictedOutputBuffer,
			    uint_t* output_value)
{
    size_t nbPredictions = 0;
    size_t nbValidPredictions = 0;

    propagate(inputBuffer, predictedOutputBuffer, output_value);

    // assert(expectedOutputBuffer.size() == predictedOutputBuffer.size());
    for(size_t i = 0; i < OUTPUTS_SIZE[0]; i++) {
        if (expectedOutputBuffer[i] >= 0) {
            ++nbPredictions;

            if(predictedOutputBuffer[i] == expectedOutputBuffer[i]) {
                ++nbValidPredictions;
            }
        }
    }

    return (nbPredictions > 0)
        ? nbValidPredictions : 0;
}


int main(int argc, char* argv[]) {
    printf("IM ALIVE\n");

    // const N2D2::Network network{};
    size_t instret, cycles;

    uint8_t *inputBuffer;

    long_t expectedOutputBuffer[OUTPUTS_SIZE[0]];
    long_t predictedOutputBuffer[OUTPUTS_SIZE[0]];
    uint_t output_value;

    decode_stimulus(stimulus_0003, &inputBuffer, &expectedOutputBuffer[0]);

    printf("%d\n", expectedOutputBuffer[0]);

    // instret = -read_csr(minstret);
    // cycles = -read_csr(mcycle);
    const int success = processInput(inputBuffer, 
                                                        expectedOutputBuffer, 
                                                        predictedOutputBuffer,
							&output_value);
    // instret += read_csr(minstret);
    // cycles += read_csr(mcycle);
    
    printf("Expected  = %d\n", expectedOutputBuffer[0]);
    printf("Predicted = %d\n", predictedOutputBuffer[0]);
    printf("Result : %d/1\n", success);
    printf("credence: %d\n", output_value);
    printf("image %s: %d instructions\n", stringify(MNIST_INPUT_IMAGE), (int)(instret));
    printf("image %s: %d cycles\n", stringify(MNIST_INPUT_IMAGE), (int)(cycles));
}
