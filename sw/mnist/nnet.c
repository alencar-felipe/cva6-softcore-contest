#include <stdlib.h>
#include <stdio.h>

#include "env.h"
#include "mem_info.h"

#include "conv1.h"
#include "conv2.h"
#include "fc1.h"
#include "fc2.h"

static int8_t mem[MEMORY_SIZE];

static int max(int lhs, int rhs) {
    return (lhs >= rhs) ? lhs : rhs;
}

static int clamp(int v, int lo, int hi) {
    if(v < lo) {
        return lo;
    }
    else if(v > hi) {
        return hi;
    }
    else {
        return v;
    }
}

static void mac(const uint8_t* __restrict inputs,
                        const int8_t* __restrict weights,
                        int32_t* __restrict weightedSum,
                        int nb_iterations)
{
    for (int iter = 0; iter < nb_iterations; ++iter) {
        *weightedSum += inputs[iter] * weights[iter];
    }
}

static uint8_t saturate(int32_t value, uint32_t sat) {
    return clamp(value, (int32_t)(0), ((int32_t)(1) << sat) - 1);
}

static uint8_t sat(int32_t weightedSum,
                                           ActivationFunction_T func,
                                           /* const Rescaling_T& __restrict rescaling */
                                           int shift)
{
    switch(func) {
        case Linear:
        case Saturation: {
            break;
        }
        case ReLU: {
            if(weightedSum <= 0) weightedSum = 0;
            break;
        }
        default:
            printf("Unsupported activation function.\n");
            break;
    }

    return saturate(weightedSum>>shift, NB_BITS);
}

static void computeKernelBounds(int outputPos, int padding, int stride, int channelSize, int kernelSize, int* minBound, int* maxBound) {
    *minBound = (padding == 0) ? 0 : max(padding - (outputPos * stride), 0);
    *maxBound = (padding == 0) ? kernelSize : clamp(channelSize + padding - (outputPos * stride), 0, kernelSize);
}

static void convcellPropagate1(
    const uint8_t *input,
    uint8_t *outputs,
    const int32_t* __restrict biasses,
    const int8_t* __restrict weights,
    int rescaling,
    int input_len, 
    int input_height, int input_width,
    int output_len,
    int OUTPUTS_HEIGHT, int OUTPUTS_WIDTH,
    int PADDING_Y, int PADDING_X,
    int STRIDE_Y, int STRIDE_X,
    int KERNEL_HEIGHT, int KERNEL_WIDTH,
    ActivationFunction_T ACTIVATION,
    int INPUT_MEM_CONT_OFFSET,
    int INPUT_MEM_CONT_SIZE,
    int INPUT_MEM_WRAP_OFFSET,
    int INPUT_MEM_WRAP_SIZE,
    int INPUT_MEM_STRIDE,
    int OUTPUT_MEM_CONT_OFFSET,
    int OUTPUT_MEM_CONT_SIZE,
    int OUTPUT_MEM_WRAP_OFFSET,
    int OUTPUT_MEM_WRAP_SIZE,
    int OUTPUT_MEM_STRIDE)
{
    // Compute output sizes without padding
    int OUTPUTS_HEIGHT_NOPAD = (input_height - KERNEL_HEIGHT + STRIDE_Y) / STRIDE_Y;
    int OUTPUTS_WIDTH_NOPAD = (input_width - KERNEL_WIDTH + STRIDE_X) / STRIDE_X;

    for (int oy = 0; oy < OUTPUTS_HEIGHT; oy++) {
        int kyMin, kyMax;
        computeKernelBounds(oy, PADDING_Y, STRIDE_Y, input_height, KERNEL_HEIGHT, &kyMin, &kyMax);
        int iy = (oy * STRIDE_Y) - PADDING_Y;

        for (int ox = 0; ox < OUTPUTS_WIDTH; ox++) {
            int kxMin, kxMax;
            computeKernelBounds(ox, PADDING_X, STRIDE_X, input_width, KERNEL_WIDTH, &kxMin, &kxMax);
            int ix = (ox * STRIDE_X) - PADDING_X;

            // Compute the output's memory offset
            int oPos = (ox + OUTPUTS_WIDTH * oy);
            int oOffset = OUTPUT_MEM_STRIDE * oPos;
            if (OUTPUT_MEM_WRAP_SIZE > 0 && oOffset >= OUTPUT_MEM_CONT_SIZE) {
                oOffset += OUTPUT_MEM_WRAP_OFFSET - OUTPUT_MEM_CONT_OFFSET - OUTPUT_MEM_CONT_SIZE;
            }

            //printf("[%02d %02d %04d] ", ix, iy, oOffset);

            // Iterate over each output channel
            for (int output = 0; output < output_len; ++output) {
                int32_t sum = biasses[output];

                // Iterate over the kernel's height
                for (int ky = 0; ky < KERNEL_HEIGHT; ky++) {
                    if ((PADDING_Y != 0 || OUTPUTS_HEIGHT != OUTPUTS_HEIGHT_NOPAD) && ky >= kyMax - kyMin) {
                        break;
                    }

                    // Compute the input's memory offset
                    int iPos = ((kxMin + ix) + input_width * (iy + kyMin + ky));
                    int iOffset = INPUT_MEM_STRIDE * iPos;
                    bool wrapInRange = false;

                    // Handle input memory wrapping
                    if (INPUT_MEM_WRAP_SIZE > 0 && iOffset >= INPUT_MEM_CONT_SIZE) {
                        iOffset += INPUT_MEM_WRAP_OFFSET - INPUT_MEM_CONT_OFFSET - INPUT_MEM_CONT_SIZE;
                    } else if (INPUT_MEM_WRAP_SIZE > 0 && KERNEL_WIDTH > 1 && input_height == 1 && iOffset + KERNEL_WIDTH * input_len > INPUT_MEM_CONT_SIZE) {
                        wrapInRange = true;
                    }

                    // Compute the weight's offset
                    int wOffset = input_len * (kxMin + KERNEL_WIDTH * (kyMin + ky + KERNEL_HEIGHT * output));

                    // Apply the multiplication and accumulation (mac) operations
                    if (!wrapInRange && (input_len == INPUT_MEM_STRIDE && ((PADDING_X == 0 && OUTPUTS_WIDTH == OUTPUTS_WIDTH_NOPAD) || kxMax - kxMin == KERNEL_WIDTH))) {
                        mac(input + iOffset, weights + wOffset, &sum, KERNEL_WIDTH * input_len);
                    } else {
                        for (int kx = 0; kx < KERNEL_WIDTH; kx++) {
                            if ((PADDING_X != 0 || OUTPUTS_WIDTH != OUTPUTS_WIDTH_NOPAD) && kx >= kxMax - kxMin) {
                                break;
                            }

                            int iOffsetInRange = iOffset + kx * INPUT_MEM_STRIDE;

                            if (wrapInRange && iOffsetInRange >= INPUT_MEM_CONT_SIZE) {
                                iOffsetInRange += INPUT_MEM_WRAP_OFFSET - INPUT_MEM_CONT_OFFSET - INPUT_MEM_CONT_SIZE;
                            }

                            mac(input + iOffsetInRange, weights + wOffset + kx * input_len, &sum, input_len);
                        }
                    }
                }

                //printf("0x%08x %04d\n", &outputs[oOffset + output], oOffset);
                // Store the final result after applying the activation function and saturation
                outputs[oOffset + output] = sat(sum, ACTIVATION, rescaling);
            }
        }

        //printf("\n");
    }
}

static void fc1(
    const uint8_t *inputs,
    uint8_t *outputs,
    const int32_t *bias,
    const int8_t *weights,
    const int rescaling,
    int NB_CHANNELS, 
    int CHANNELS_HEIGHT, int CHANNELS_WIDTH,
    int NB_OUTPUTS,
    ActivationFunction_T ACTIVATION
)
{
    uint32_t K = CHANNELS_WIDTH*CHANNELS_HEIGHT*NB_CHANNELS;
    uint32_t offset = 0;
    for (int i = 0; i < NB_OUTPUTS; i++) {
        int32_t sum = bias[i];
        mac(inputs, weights + offset, &sum, K);
        outputs[i] = sat(sum, ACTIVATION, rescaling);
        offset += K;
    }
}

static void fc2(
    const uint32_t input_len,
    const uint32_t output_len,
    const uint8_t *input,
    const int32_t *bias,
    const int8_t *weight,
    int8_t *output,
    const int rescaling,
    ActivationFunction_T ACTIVATION // Linear
)
{
    for (uint32_t i = 0; i < output_len; i++) {
        int32_t sum = bias[i];
        mac(input, weight, &sum, input_len);
        output[i] = sat(sum, ACTIVATION, rescaling);
        weight += input_len;
    }
}

static void argmax(
    const uint32_t n,
    const int8_t *arr,
    uint32_t *arg,
    int8_t *max
) 
{
    *arg = 0;
    *max = arr[*arg];

    for(uint32_t i = 1; i < n; i++) {
        if(arr[i] > *max) {
            *arg = i;
            *max = arr[i];
        }
    }
}

void propagate(const uint8_t* inputs, int32_t* outputs, uint8_t* credence)
{
    // conv1
    uint8_t* conv1_output = (uint8_t*) mem + CONV1_MEM_CONT_OFFSET;
    
    //printf("-1 => %d\n", conv1_output[0]);
    //exit(-1);

    convcellPropagate1(
        inputs,
        conv1_output,
        conv1_biases,
        conv1_weights,
        8,
        
        CONV1_NB_CHANNELS,
        CONV1_CHANNELS_HEIGHT,
        CONV1_CHANNELS_WIDTH,
        
        CONV1_NB_OUTPUTS,
        CONV1_OUTPUTS_HEIGHT, 
        CONV1_OUTPUTS_WIDTH,
        
        CONV1_PADDING_Y,
        CONV1_PADDING_X,
        CONV1_STRIDE_Y,
        CONV1_STRIDE_X,
        CONV1_KERNEL_HEIGHT,
        CONV1_KERNEL_WIDTH,

        CONV1_ACTIVATION, // ReLU

        ENV_MEM_CONT_OFFSET, // 0
        ENV_MEM_CONT_SIZE,   // 576
        ENV_MEM_WRAP_OFFSET, // 0
        ENV_MEM_WRAP_SIZE,   // 0
        ENV_MEM_STRIDE,      // 1
        
        CONV1_MEM_CONT_OFFSET, // 576
        CONV1_MEM_CONT_SIZE,   // 1584
        CONV1_MEM_WRAP_OFFSET, // 0
        CONV1_MEM_WRAP_SIZE,   // 352
        CONV1_MEM_STRIDE       // 16
    );

    //printf("-1 => %d\n", conv1_output[0]);

    //printf("mem : 0x%08x\n", conv1_output);
    //exit(-1);

    // conv2
    uint8_t* conv2_output = (uint8_t*) mem + CONV2_MEM_CONT_OFFSET;

    convcellPropagate1(
        conv1_output,
        conv2_output,
        conv2_biases,
        conv2_weights,
        8,

        CONV2_NB_CHANNELS,
        CONV2_CHANNELS_HEIGHT,
        CONV2_CHANNELS_WIDTH, 
        
        CONV2_NB_OUTPUTS,
        CONV2_OUTPUTS_HEIGHT,
        CONV2_OUTPUTS_WIDTH, 
        
        CONV2_PADDING_Y,
        CONV2_PADDING_X,
        CONV2_STRIDE_Y,
        CONV2_STRIDE_X,

        CONV2_KERNEL_HEIGHT,
        CONV2_KERNEL_WIDTH,

        CONV2_ACTIVATION, // ReLU

        CONV1_MEM_CONT_OFFSET, // 576
        CONV1_MEM_CONT_SIZE,   // 1584
        CONV1_MEM_WRAP_OFFSET, // 0
        CONV1_MEM_WRAP_SIZE,   // 352
        CONV1_MEM_STRIDE,      // 16
        
        CONV2_MEM_CONT_OFFSET, // 352
        CONV2_MEM_CONT_SIZE,   // 384
        CONV2_MEM_WRAP_OFFSET, // 0
        CONV2_MEM_WRAP_SIZE,   // 0
        CONV2_MEM_STRIDE       // 24
    );

    //convcellPropagate2(conv1_output , conv2_output, conv2_biases, conv2_weights, CONV2_SCALING);

    // fc1
    uint8_t* fc1_output = (uint8_t*) mem + FC1_MEM_CONT_OFFSET;

    fc1(
        conv2_output,
        fc1_output,
        fc1_biases,
        fc1_weights,
        8,
        
        FC1_NB_CHANNELS,   
        FC1_CHANNELS_HEIGHT, 
        FC1_CHANNELS_WIDTH,
        
        FC1_NB_OUTPUTS, 
        
        FC1_ACTIVATION // ReLU
    );

    // fc2
    int8_t* fc2_output = (int8_t*) mem + FC2_MEM_CONT_OFFSET;

    fc2(
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


