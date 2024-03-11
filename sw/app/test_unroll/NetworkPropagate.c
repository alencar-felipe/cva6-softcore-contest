#include <stdlib.h>
#include <stdio.h>

#include "env.h"
#include "mem_info.h"

#include "conv1.h"
#include "conv2.h"
#include "fc1.h"
#include "fc2.h"

const uint32_t CRC_POLY = 0xEDB88320;
uint32_t conv[256];

static int8_t mem[MEMORY_SIZE];

static int clamp(int v, int lo, int hi) {
    int mask_lo = v - lo >> (sizeof(int) * CHAR_BIT - 1);
    int mask_hi = hi - v >> (sizeof(int) * CHAR_BIT - 1);

    return (lo & mask_lo) | (hi & mask_hi) | (v & ~(mask_lo | mask_hi));
}

void mac(const uint8_t* __restrict inputs,
                        const int8_t* __restrict weights,
                        int32_t* __restrict weightedSum,
                        int nb_iterations)
{
    int iter = 0;
    int32_t sum = *weightedSum;
    //vint32m1_t sum_vec = vle32_v_i32m1(weightedSum); // load initial sum into vector register

    for (; iter <= nb_iterations - 4; iter += 4) {
        sum += inputs[iter+0]*weights[iter+0]; 
        sum += inputs[iter+1]*weights[iter+1]; 
        sum += inputs[iter+2]*weights[iter+2]; 
        sum += inputs[iter+3]*weights[iter+3]; 
    }

    for (; iter < nb_iterations; iter++) {
        sum += inputs[iter]*weights[iter]; 
    }

    *weightedSum = sum;
}

static inline void convcellPropagate1(const uint8_t *input, uint8_t *outputs, const int32_t *__restrict biasses, const int8_t *__restrict weights, ActivationFunction_T ACTIVATION){
    for (int oy = 0; oy < 11; oy++){        //OUTPUTS_HEIGHT 
        int iy = oy << 1;
        for (int ox = 0; ox < 11; ox++){   // OUTPUTS_WIDTH
            int ix = ox << 1;
            int oOffset = ox + 11 * oy << 4;
            oOffset += (oOffset >= 1584) ? -2160 : 0;       // 1584 = OUTPUT_MEM_CONT_SIZE    _|&&|_  2160 = UTPUT_MEM_WRAP_OFFSET - OUTPUT_MEM_CONT_OFFSET - OUTPUT_MEM_CONT_SIZE;
            for (int output = 0; output < 16; ++output){
                int32_t sum = biasses[output];
                for (int ky = 0; ky < 4; ky++){
                    int iPos = ix + (24 * (iy + ky));
                    int wOffset = ((ky + (output << 2)) << 2);
                    for (int i=0; i<4 ; i++){
                        sum += (((input + iPos)[i]) * ((weights + wOffset)[i]));
                    }
                }
                //outputs[oOffset + output] = (ACTIVATION == ReLU && sum <= 0) ? clamp(0, 0,8) : clamp(sum >> 8,0, 8);
                outputs[oOffset + output] = clamp(sum >> 8, 0, 255);
            }
        }
    }
}

static inline void convcellPropagate2(const uint8_t *input, uint8_t *outputs, const int32_t *__restrict biasses, const int8_t *__restrict weights, ActivationFunction_T ACTIVATION){
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
                    iOffset += (iOffset >= 1584) ? -2160 : 0;       // 1584 = OUTPUT_MEM_CONT_SIZE    _|&&|_  2160 = UTPUT_MEM_WRAP_OFFSET - OUTPUT_MEM_CONT_OFFSET - OUTPUT_MEM_CONT_SIZE;
                    int wOffset = (5 * (ky + 5 * output)) << 4;
                    for (int kx = 0; kx < 5; kx++){
                        int iOffsetInRange = iOffset + (kx << 4);
                        mac(input + iOffsetInRange, weights + wOffset + kx * 16, &sum, 16);
                    }
                }
                //outputs[oOffset + output] = (ACTIVATION == ReLU && sum <= 0) ? clamp(0, 0,8) : clamp(sum >> 8, 0,8);
                outputs[oOffset + output] = clamp(sum >> 8, 0, 255);
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
        output[i] = (ACTIVATION == CONV1_ACTIVATION && sum <= 0) ? 0 : clamp(sum >> rescaling, 0, 255);
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

void buildCrc32Table() {
    uint32_t i, ch, crc, b;
    size_t j;

    for(i = 0; i < 256; i++) {
        ch = i;
        crc = 0;

        for(j = 0; j < 8; j++) {
            b = (ch ^ crc) & 1;
            crc >>= 1;
            if(b) crc = crc ^ CRC_POLY;
            ch >>= 1;
        }

        conv[i] = crc;
    }
}

void crc32(uint32_t *crc, const uint8_t *data, size_t len) {
    uint32_t t;
    uint8_t *ptr;

    (*crc) = ~(*crc);

    ptr = (uint8_t *) data;

    while(len--) {
        t = (*(ptr++) ^ *crc) & 0xFF;
        *crc = (*crc >> 8) ^ conv[t];
    }

    (*crc) = ~(*crc);
}


void propagate(const uint8_t *inputs, int32_t *outputs, uint8_t *credence)
{
    // conv1
    uint8_t *conv1_output = (uint8_t *)mem + CONV1_MEM_CONT_OFFSET;

    convcellPropagate1(
        inputs,
        conv1_output,
        conv1_biases,
        conv1_weights,
        CONV1_ACTIVATION // ReLU
    );


    #ifdef PRINT_OUT
        buildCrc32Table();
        size_t crc0 = 0x000000;
        crc32(&crc0, inputs, ENV_SIZE_Y*ENV_SIZE_X*ENV_NB_OUTPUTS);
        crc32(&crc0, conv1_output, 2160 + CONV1_MEM_CONT_OFFSET);
        crc32(&crc0, conv1_biases, CONV1_NB_OUTPUTS);
        crc32(&crc0, conv1_weights, CONV1_WEIGHTS_SIZE);
        printf("After First Convolution\n");
        printf("Inputs;         %10lu\n",inputs);
        printf("conv1_output;   %10lu\n",conv1_output);
        printf("conv1_biases;   %10lu\n",conv1_biases);
        printf("conv1_weights;  %10lu\n",conv1_weights);
        #ifdef PRINT_EACH_CRC
            printf("crc;            %x\n",crc0);
        #endif
    #endif


    // conv2
    uint8_t *conv2_output = (uint8_t *)mem + CONV2_MEM_CONT_OFFSET;

    convcellPropagate2(
        conv1_output,
        conv2_output,
        conv2_biases,
        conv2_weights,
        CONV2_ACTIVATION // ReLU

    );


    #ifdef PRINT_OUT
        size_t crc1 = 0x000000;
        crc32(&crc1, conv1_output, 2160 + CONV1_MEM_CONT_OFFSET);
        crc32(&crc1, conv2_output, 2160 + CONV2_MEM_CONT_OFFSET);
        crc32(&crc1, conv2_biases, CONV2_NB_OUTPUTS);
        crc32(&crc1, conv2_weights, CONV2_WEIGHTS_SIZE);
        printf("After Second Convolution\n");
        printf("Inputs;         %10lu\n",conv1_output);
        printf("conv2_output;   %10lu\n",conv2_output);
        printf("conv2_biases;   %10lu\n",conv2_biases);
        printf("conv2_weights;  %10lu\n",conv2_weights);
        #ifdef PRINT_EACH_CRC
            printf("crc;            %x\n",crc1);
        #endif
    #endif

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

    #ifdef PRINT_OUT
        size_t crc2 = 0x000000;
        crc32(&crc2, conv2_output, 2160 + CONV2_MEM_CONT_OFFSET);
        crc32(&crc2, fc1_biases, FC1_OUTPUTS_SIZE);
        crc32(&crc2, fc1_weights, FC1_WEIGHTS_SIZE);
        crc32(&crc2, fc1_output, 2160 + FC1_MEM_CONT_OFFSET);
        printf("After Fully Connected 1:\n");
        printf("conv2_output;  %10lu\n",conv2_output);
        printf("fc1_biases;    %10lu\n",fc1_biases);
        printf("fc1_weights;   %10lu\n",fc1_weights);
        printf("fc1_output;    %10lu\n",fc1_output);
        #ifdef PRINT_EACH_CRC
            printf("crc;            %x\n",crc2);
        #endif
    #endif

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
     
    #ifdef PRINT_OUT
        size_t crc3 = 0x000000;
        crc32(&crc3, fc1_output, 2160 + FC1_MEM_CONT_OFFSET);
        crc32(&crc3, fc2_biases, FC2_OUTPUTS_SIZE);
        crc32(&crc3, fc2_weights, FC2_WEIGHTS_SIZE);
        crc32(&crc3, fc2_output, 2160 + FC2_MEM_CONT_OFFSET);
        printf("After Fully Connected 2:\n");
        printf("fc1_output;  %10lu\n",fc1_output);
        printf("fc2_biases;  %10lu\n",fc2_biases);
        printf("fc2_weights; %10lu\n",fc2_weights);
        printf("fc2_output;  %10lu\n",fc2_output);
        #ifdef PRINT_EACH_CRC
            printf("crc;            %x\n",crc3);
        #endif
    #endif
    argmax(10, fc2_output, outputs, credence);
    
    #ifdef PRINT_OUT
        size_t crc4 =0x000000,crc5 = 0x000000;
        crc32(&crc4, fc2_output, 2160 + FC2_MEM_CONT_OFFSET);
        crc32(&crc4, outputs, 1);
        crc32(&crc4, credence, sizeof(credence));
        printf("After ARGMAX 2:\n");
        printf("fc2_output;  %10lu\n",fc2_output);
        printf("outputs;     %10lu\n",outputs);
        printf("credence;    %10lu\n",credence);
        #ifdef PRINT_EACH_CRC
            printf("crc;            %x\n",crc4);
        #endif
        #ifdef PRINT_LAST_CRC 
            crc5 = crc4 + crc3 + crc2 + crc1 + crc0;
            printf("crc_block %x\n",crc5);
        #endif
    #endif
}