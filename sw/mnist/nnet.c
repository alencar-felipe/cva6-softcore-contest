#include <stdlib.h>
#include <stdio.h>

#include "env.h"
#include "mem_info.h"

#include "conv1.h"
#include "conv2.h"
#include "fc1.h"
#include "fc2.h"

static int8_t mem[MEMORY_SIZE];

static inline int clamp(int v, int hi)
{
    // int mask = v >> (sizeof(int) * 8 - 1);
    /*
        Do the displacement(Right Shift) in v
        And logic v and not v(shifted)
        Or logic between the two parts
    */
    return (v & ~(v >> 31)) | (hi & (v >> 31));
}

static void mac(const uint8_t *__restrict inputs,
                const int8_t *__restrict weights,
                int32_t *__restrict weightedSum,
                int nb_iterations)
{
    for (int iter = 0; iter < nb_iterations; ++iter)
    {
        *weightedSum += inputs[iter] * weights[iter];
    }
}

static inline void convcellPropagate1(const uint8_t *input, uint8_t *outputs, const int32_t *__restrict biasses, const int8_t *__restrict weights, ActivationFunction_T ACTIVATION){
    for (int oy = 0; oy < 11; oy++){
        int iy = oy << 1;
        for (int ox = 0; ox < 11; ox++){
            int ix = ox << 1;
            int oOffset = ox + 11 * oy << 4;
            oOffset += (oOffset >= 1584) ? -2160 : 0;
            for (int output = 0; output < 16; ++output){
                int32_t sum = biasses[output];
                for (int ky = 0; ky < 4; ky++){
                    int iPos = ix + (24 * (iy + ky));
                    int wOffset = ((ky + (output << 2)) << 2);
                    sum += (((input + iPos)[0]) * ((weights + wOffset)[0]));
                    sum += (((input + iPos)[1]) * ((weights + wOffset)[1]));
                    sum += (((input + iPos)[2]) * ((weights + wOffset)[2]));
                    sum += (((input + iPos)[3]) * ((weights + wOffset)[3]));
                }
                outputs[oOffset + output] = (ACTIVATION == ReLU && sum <= 0) ? clamp(0, 8) : clamp(sum >> 8, 8);
            }
        }
    }
}

static void convcellPropagate2(const uint8_t *input, uint8_t *outputs, const int32_t *__restrict biasses, const int8_t *__restrict weights, ActivationFunction_T ACTIVATION){
    for (int oy = 0; oy < 4; oy++){
        int iy = oy << 1;
        for (int ox = 0; ox < 4; ox++){
            int ix = ox << 1;
            int oPos = (ox + 4 * oy);
            int oOffset = 24 * oPos;
            oOffset += (oOffset >= 1584) ? -2160 : 0;
            for (int output = 0; output < 24; ++output){
                int32_t sum = biasses[output];
                for (int ky = 0; ky < 5; ky++){
                    int iOffset = ((ix) + 11 * (iy + ky)) << 4;
                    iOffset += (iOffset >= 1584) ? -2160 : 0;
                    int wOffset = (5 * (ky + 5 * output)) << 4;
                    for (int kx = 0; kx < 5; kx++){
                        int iOffsetInRange = iOffset + (kx << 4);
                        mac(input + iOffsetInRange, weights + wOffset + kx * 16, &sum, 16);
                    }
                }
                outputs[oOffset + output] = (ACTIVATION == ReLU && sum <= 0) ? clamp(0, 8) : clamp(sum >> 8, 8);
            }
        }
    }
}
// TODO: Do we need int8_t for the output of fc2?

static void fully_connected(
    const uint32_t input_len,
    const uint32_t output_len,
    const uint8_t *input,
    const int32_t *bias,
    const int8_t *weight,
    uint8_t *output,
    const int rescaling,
    ActivationFunction_T ACTIVATION)
{
    for (uint32_t i = 0; i < output_len; i++)
    {
        int32_t sum = bias[i];
        mac(input, weight, &sum, input_len);
        output[i] = (ACTIVATION == ReLU && sum <= 0) ? clamp(0, 8) : clamp(sum >> rescaling, 8);
        weight += input_len;
    }
}

static void argmax(
    const uint32_t n,
    const int8_t *arr,
    uint32_t *arg,
    int8_t *max)
{
    *arg = 0;
    *max = arr[*arg];

    for (uint32_t i = 1; i < n; i++)
    {
        if (arr[i] > *max)
        {
            *arg = i;
            *max = arr[i];
        }
    }
}

void propagate(const uint8_t *inputs, int32_t *outputs, uint8_t *credence)
{
    // conv1
    uint8_t *conv1_output = (uint8_t *)mem + CONV1_MEM_CONT_OFFSET;

    // printf("-1 => %d\n", conv1_output[0]);
    // exit(-1);

    convcellPropagate1(
        inputs,
        conv1_output,
        conv1_biases,
        conv1_weights,
        CONV1_ACTIVATION // ReLU
    );

    // printf("-1 => %d\n", conv1_output[0]);

    // printf("mem : 0x%08x\n", conv1_output);
    // exit(-1);

    // conv2
    uint8_t *conv2_output = (uint8_t *)mem + CONV2_MEM_CONT_OFFSET;

    convcellPropagate2(
        conv1_output,
        conv2_output,
        conv2_biases,
        conv2_weights,
        CONV2_ACTIVATION // ReLU

    );

    // fc1
    uint8_t *fc1_output = (uint8_t *)mem + FC1_MEM_CONT_OFFSET;

    fully_connected(
        FC1_CHANNELS_WIDTH * FC1_CHANNELS_HEIGHT * FC1_NB_CHANNELS,
        FC1_NB_OUTPUTS,
        conv2_output,
        fc1_biases,
        fc1_weights,
        fc1_output,
        8,
        FC1_ACTIVATION // Linear
    );

    // fc2
    int8_t *fc2_output = (int8_t *)mem + FC2_MEM_CONT_OFFSET;

    fully_connected(
        FC2_NB_CHANNELS,
        FC2_NB_OUTPUTS,
        fc1_output,
        fc2_biases,
        fc2_weights,
        fc2_output,
        11,
        FC2_ACTIVATION // Linear
    );

    argmax(10, fc2_output, outputs, credence);
}
