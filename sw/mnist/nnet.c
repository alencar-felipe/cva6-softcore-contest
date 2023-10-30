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

static void nnet_mac(const uint8_t* __restrict inputs,
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

                    // Apply the multiplication and accumulation (nnet_mac) operations
                    if (!wrapInRange && (input_len == INPUT_MEM_STRIDE && ((PADDING_X == 0 && OUTPUTS_WIDTH == OUTPUTS_WIDTH_NOPAD) || kxMax - kxMin == KERNEL_WIDTH))) {
                        nnet_mac(input + iOffset, weights + wOffset, &sum, KERNEL_WIDTH * input_len);
                    } else {
                        for (int kx = 0; kx < KERNEL_WIDTH; kx++) {
                            if ((PADDING_X != 0 || OUTPUTS_WIDTH != OUTPUTS_WIDTH_NOPAD) && kx >= kxMax - kxMin) {
                                break;
                            }

                            int iOffsetInRange = iOffset + kx * INPUT_MEM_STRIDE;

                            if (wrapInRange && iOffsetInRange >= INPUT_MEM_CONT_SIZE) {
                                iOffsetInRange += INPUT_MEM_WRAP_OFFSET - INPUT_MEM_CONT_OFFSET - INPUT_MEM_CONT_SIZE;
                            }

                            nnet_mac(input + iOffsetInRange, weights + wOffset + kx * input_len, &sum, input_len);
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

static void fccellPropagateuint8_t(
    const uint8_t* __restrict inputs,
    uint8_t* __restrict outputs,
    const int32_t* __restrict biasses,
    const int8_t* __restrict weights,
    const int rescaling,
    int NB_CHANNELS, 
    int CHANNELS_HEIGHT, int CHANNELS_WIDTH,
    int NB_OUTPUTS,
    int OUTPUTS_HEIGHT, int OUTPUTS_WIDTH,
    ActivationFunction_T ACTIVATION,
    // Memory mapping: inputs
    int INPUT_MEM_CONT_OFFSET,
    int INPUT_MEM_CONT_SIZE,
    int INPUT_MEM_WRAP_OFFSET,
    int INPUT_MEM_WRAP_SIZE,
    int INPUT_MEM_STRIDE,
    // Memory mapping: outputs
    int OUTPUT_MEM_CONT_OFFSET,
    int OUTPUT_MEM_CONT_SIZE,
    int OUTPUT_MEM_WRAP_OFFSET,
    int OUTPUT_MEM_WRAP_SIZE,
    int OUTPUT_MEM_STRIDE)
{
    // static_assert(OUTPUTS_HEIGHT == 1, "Outputs height should be 1");
    // static_assert(OUTPUTS_WIDTH == 1, "Outputs width should be 1");
    // static_assert(OUTPUT_MEM_WRAP_SIZE == 0, "Output wrapping not supported");

    for (int och = 0; och < NB_OUTPUTS; och++) {
        int32_t weightedSum = biasses[och];

        for (int iy = 0; iy < CHANNELS_HEIGHT; ++iy) {
            const int iPos = (CHANNELS_WIDTH * iy);
            int iOffset = INPUT_MEM_STRIDE * iPos;

            // Wrapping cannot occur in the middle of a line, except if
            // there is only one line (1D)!
            bool wrapInRange = false;

            if (INPUT_MEM_WRAP_SIZE > 0 && iOffset >= INPUT_MEM_CONT_SIZE) {
                iOffset += INPUT_MEM_WRAP_OFFSET - INPUT_MEM_CONT_OFFSET
                            - INPUT_MEM_CONT_SIZE;
            }
            else if (INPUT_MEM_WRAP_SIZE > 0 && CHANNELS_WIDTH > 1
                && CHANNELS_HEIGHT == 1 // single line (1D)!
                && iOffset + CHANNELS_WIDTH * NB_CHANNELS
                    > INPUT_MEM_CONT_SIZE)
            {
                wrapInRange = true;
            }

            const int wOffset = NB_CHANNELS * CHANNELS_WIDTH
                                    * (iy + CHANNELS_HEIGHT * och);

            if (!wrapInRange && INPUT_MEM_STRIDE == NB_CHANNELS) {
                nnet_mac(
                    inputs + iOffset, 
                    weights + wOffset, 
                    &weightedSum, NB_CHANNELS * CHANNELS_WIDTH);
            }
            else {
                for (int ix = 0; ix < CHANNELS_WIDTH; ++ix) {
                    int iOffsetInRange = iOffset + ix * INPUT_MEM_STRIDE;

                    if (wrapInRange
                        && iOffsetInRange >= INPUT_MEM_CONT_SIZE)
                    {
                        iOffsetInRange += INPUT_MEM_WRAP_OFFSET
                                    - INPUT_MEM_CONT_OFFSET
                                    - INPUT_MEM_CONT_SIZE;
                    }

                    nnet_mac(
                        inputs + iOffsetInRange, 
                        weights + wOffset + ix * NB_CHANNELS, 
                        &weightedSum, NB_CHANNELS);
                }
            }
        }

        outputs[och] = sat(weightedSum, ACTIVATION, rescaling);
    }
}

static void nnet_fc_i8(
    const uint8_t *input,
    const int32_t *bias,
    const int8_t *weight,
    int8_t *output,
    const int rescaling,
    int NB_CHANNELS,     // 150
    int NB_OUTPUTS,      // 10
    ActivationFunction_T ACTIVATION // Linear
)
{
    for (int och = 0; och < NB_OUTPUTS; och++) {
        int32_t sum = bias[och];

        const int wOffset = NB_CHANNELS * och;

        nnet_mac(input, weight + wOffset, &sum, NB_CHANNELS);

        output[och] = sat(sum, ACTIVATION, rescaling);
    }
}

static void nnet_argmax(
    const int8_t *input,
    const uint32_t input_len,
    uint32_t *arg,
    int8_t *max
) 
{
    uint32_t i;

    *arg = 0;
    *max = input[*arg];

    for(i = 1; i < input_len; i++) {
        if(input[i] > *max) {
            *arg = i;
            *max = input[i];
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

    fccellPropagateuint8_t(
        conv2_output,
        fc1_output,
        fc1_biases,
        fc1_weights,
        8,
        
        FC1_NB_CHANNELS,   
        FC1_CHANNELS_HEIGHT, 
        FC1_CHANNELS_WIDTH,
        
        FC1_NB_OUTPUTS, 
        FC1_OUTPUTS_HEIGHT,
        FC1_OUTPUTS_WIDTH,
        
        FC1_ACTIVATION, // ReLU
        
        CONV2_MEM_CONT_OFFSET, // 352
        CONV2_MEM_CONT_SIZE,   // 384
        CONV2_MEM_WRAP_OFFSET, // 0
        CONV2_MEM_WRAP_SIZE,   // 0
        CONV2_MEM_STRIDE,      // 24

        FC1_MEM_CONT_OFFSET, // 736
        FC1_MEM_CONT_SIZE,   // 150
        FC1_MEM_WRAP_OFFSET, // 0
        FC1_MEM_WRAP_SIZE,   // 0
        FC1_MEM_STRIDE       // 150
    );

    // fc2
    int8_t* fc2_output = (int8_t*) mem + FC2_MEM_CONT_OFFSET;

    nnet_fc_i8(
        fc1_output,
        fc2_biases,
        fc2_weights,
        fc2_output,
        11,
        FC2_NB_CHANNELS, 
        FC2_NB_OUTPUTS, 
        
        FC2_ACTIVATION // Linear
    );

    nnet_argmax(fc2_output, 10, outputs, credence);
}


