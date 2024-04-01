#include "network.h"

#define C1_ON    ( 16) // Number of Outputs               
#define C1_OY    ( 11) // Output Height
#define C1_OX    ( 11) // Output Width
#define C1_IN    (  1) // Number of Inputs
#define C1_IY    ( 24) // Input Height
#define C1_IX    ( 24) // Input Width
#define C1_WY    (  4) // Weight Height (Kernel Height)
#define C1_WX    (  4) // Weight Width (Kernel Width)
#define C1_SY    (  2) // Input Stride Y
#define C1_SX    (  2) // Input Stride X
#define C1_BIAS  (128) // Bias Value
#define C1_SHIFT (  8) // Output Left Shift

#define C1_O_SIZE (C1_ON*C1_OY*C1_OX)
#define C1_I_SIZE (C1_IN*C1_IY*C1_IX)
#define C1_W_SIZE (C1_ON*C1_WX*C1_WY*C1_IN)

#define C2_ON    ( 24) // Number of Outputs               
#define C2_OY    (  4) // Output Height
#define C2_OX    (  4) // Output Width
#define C2_IN    ( 16) // Number of Inputs
#define C2_IX    ( 11) // Input Height
#define C2_IY    ( 11) // Input Width
#define C2_WY    (  5) // Weight Height (Kernel Height)
#define C2_WX    (  5) // Weight Width (Kernel Width)
#define C2_SY    (  2) // Input Stride Y
#define C2_SX    (  2) // Input Stride X
#define C2_BIAS  (128) // Bias Value
#define C2_SHIFT (  8) // Output Left Shift

#define C2_O_SIZE (C2_ON*C2_OY*C2_OX)
#define C2_I_SIZE (C2_IN*C2_IY*C2_IX)
#define C2_W_SIZE (C2_ON*C2_WX*C2_WY*C2_IN)

#define FC1_O    (150) // Number of Outputs
#define FC1_I    (384) // Number of Inputs
#define FC1_BIAS (128) // Bias Value

#define FC2_O    (  10) // Number of Outputs
#define FC2_I    ( 150) // Number of Inputs
#define FC2_BIAS (1024) // Bias Value

static uint8_t conv1_output[C1_O_SIZE];
static uint8_t conv2_output[C2_O_SIZE];
static uint8_t fc1_output[FC1_O];
static uint8_t fc2_output[FC2_O];

static void conv1(
    uint8_t *output,
    const uint8_t *input,
    const int8_t *weight
)
{
    size_t avl;
    size_t vl;

    for (size_t oy = 0; oy < C1_OX; oy++) {
        for (size_t ox = 0; ox < C1_OY; ox++) {
            for (size_t on = 0; on < C1_ON; on += vl) {
                avl = C1_ON - on;
                vl = (avl <= MAXVL) ? avl : MAXVL; 
            
                vec_t sum_v = vmv_vx(C1_BIAS, vl);

                for (size_t wy = 0; wy < C1_WY; wy++) {
                    for (size_t wx = 0; wx < C1_WX; wx++) {
                        for (size_t in = 0; in < C1_IN; in++) {   
                            size_t iy = C1_SY*oy + wy;
                            size_t ix = C1_SX*ox + wx;
                            size_t i = (iy*C1_IX + ix)*C1_IN + in;

                            size_t w = ((in*C1_WY + wy)*C1_WX + wx)*C1_ON + on;                
                            vec_t weight_v = vlei8(&weight[w], vl);
                            
                            sum_v = vmacc_vx(sum_v, input[i], weight_v, vl);
                        }
                    }
                }

                sum_v = vmax_vx(sum_v, 0, vl);

                size_t o = (oy*C1_OX + ox)*C1_ON + on;
                vnsrl_vse8(&output[o], sum_v, C1_SHIFT, vl);        
            }  
        }
    }
}

static void conv2(
    uint8_t *output,
    const uint8_t *input,
    const int8_t *weight
)
{
    size_t avl;
    size_t vl;

    for (size_t oy = 0; oy < C2_OX; oy++) {
        for (size_t ox = 0; ox < C2_OY; ox++) {
            for (size_t on = 0; on < C2_ON; on += vl) {
                avl = C2_ON - on;
                vl = (avl <= MAXVL) ? avl : MAXVL; 
            
                vec_t sum_v = vmv_vx(C2_BIAS, vl);

                for (size_t wy = 0; wy < C2_WY; wy++) {
                    for (size_t wx = 0; wx < C2_WX; wx++) {
                        for (size_t in = 0; in < C2_IN; in++) {   
                            size_t iy = C2_SY*oy + wy;
                            size_t ix = C2_SX*ox + wx;
                            size_t i = (iy*C2_IX + ix)*C2_IN + in;

                            size_t w = ((in*C2_WY + wy)*C2_WX + wx)*C2_ON + on;                
                            vec_t weight_v = vlei8(&weight[w], vl);
                            
                            sum_v = vmacc_vx(sum_v, input[i], weight_v, vl);
                        }
                    }
                }

                sum_v = vmax_vx(sum_v, 0, vl);

                size_t o = (oy*C2_OX + ox)*C2_ON + on;
                vnsrl_vse8(&output[o], sum_v, C2_SHIFT, vl);        
            }  
        }
    }
}

static void fc1(
    const uint8_t *input,
          uint8_t *output,
    const int8_t  *weight
)
{
    for (size_t o = 0; o < FC1_O; o++) {
        int32_t sum = FC1_BIAS;

        for (size_t i = 0; i < FC1_I; i++) {
            size_t w = o*FC1_I + i;
            sum += input[i] * weight[w];
        }

        // activation
        if (sum > 0) {
            output[o] = (sum >> 8) & 0xFF;
        } else {
            output[o] = 0;
        }
    }    
}

static void fc2(
    const uint8_t *input,
          uint8_t *output,
    const int8_t  *weight
)
{
    for (size_t o = 0; o < FC2_O; o++) {
        int32_t sum = FC2_BIAS;

        for (size_t i = 0; i < FC2_I; i++) {
            size_t w = o*FC2_I + i;
            sum += input[i] * weight[w];
        }

        // activation
        sum = (sum >> 11);
        if (sum < 0) {
            output[o] =  0;
        } else if (sum > 255) { // 2047
            output[o] = 255; // 2047
        } else {
            output[o] = sum;
        }
    }    
}

static void argmax1(
    const uint8_t *input,
          int32_t *output,
          uint8_t *max
) 
{
    *output = 0;
    *max = input[*output];

    for(size_t i = 1; i < FC2_O; i++) {
        if(input[i] > *max) {
            *output = i;
            *max = input[i];
        }
    }
}

void inference(const uint8_t* input, int32_t* output, uint8_t* credence)
{

#ifdef VALIDATION_RUN
    ASSERT(MAXVL >= vsetvlmax());
    
    uint32_t crc;
    crc32_table_init();

    crc = 0;
    crc32(&crc, input, 576);
    ASSERT(crc == 0x4dfde263);
#endif

    conv1(conv1_output, input, conv1_weight);

#ifdef VALIDATION_RUN
    crc = 0;
    crc32(&crc, conv1_output, C1_O_SIZE);
    ASSERT(crc == 0xa6062dba);
#endif

    conv2(conv2_output, conv1_output, conv2_weight);

#ifdef VALIDATION_RUN    
    crc = 0;
    crc32(&crc, conv2_output, C2_O_SIZE);
    ASSERT(crc == 0x0aa1524f);
#endif

    fc1(conv2_output, fc1_output, fc1_weight);

#ifdef VALIDATION_RUN 
    crc = 0;
    crc32(&crc, fc1_output, FC1_O);
    ASSERT(crc == 0x7e1d772e);
#endif

    fc2(fc1_output, fc2_output, fc2_weight);

#ifdef VALIDATION_RUN
    crc = 0;
    crc32(&crc, fc2_output, FC2_O);
    ASSERT(crc == 0x6d779679);
#endif

    argmax1(fc2_output, output, credence);

#ifdef VALIDATION_RUN
    crc = 0;
    crc32(&crc, output, 1);
    ASSERT(crc == 0xd56f2b94);
#endif

}





