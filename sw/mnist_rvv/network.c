#include "network.h"

#define C0_ON    ( 16) // Number of Outputs               
#define C0_OY    ( 11) // Output Height
#define C0_OX    ( 11) // Output Width
#define C0_IN    (  1) // Number of Inputs
#define C0_IY    ( 24) // Input Height
#define C0_IX    ( 24) // Input Width
#define C0_WY    (  4) // Weight Height (Kernel Height)
#define C0_WX    (  4) // Weight Width (Kernel Width)
#define C0_SY    (  2) // Input Stride Y
#define C0_SX    (  2) // Input Stride X
#define C0_BIAS  (128) // Bias Value
#define C0_SHIFT (  8) // Output Left Shift

#define C0_O_SIZE (C0_ON*C0_OY*C0_OX)
#define C0_I_SIZE (C0_IN*C0_IY*C0_IX)
#define C0_W_SIZE (C0_ON*C0_WX*C0_WY*C0_IN)

#define C1_ON    ( 24) // Number of Outputs               
#define C1_OY    (  4) // Output Height
#define C1_OX    (  4) // Output Width
#define C1_IN    ( 16) // Number of Inputs
#define C1_IX    ( 11) // Input Height
#define C1_IY    ( 11) // Input Width
#define C1_WY    (  5) // Weight Height (Kernel Height)
#define C1_WX    (  5) // Weight Width (Kernel Width)
#define C1_SY    (  2) // Input Stride Y
#define C1_SX    (  2) // Input Stride X
#define C1_BIAS  (128) // Bias Value
#define C1_SHIFT (  8) // Output Left Shift

#define C1_O_SIZE (C1_ON*C1_OY*C1_OX)
#define C1_I_SIZE (C1_IN*C1_IY*C1_IX)
#define C1_W_SIZE (C1_ON*C1_WX*C1_WY*C1_IN)

#define C2_ON    (150) // Number of Outputs               
#define C2_OY    (  1) // Output Height
#define C2_OX    (  1) // Output Width
#define C2_IN    (384) // Number of Inputs
#define C2_IX    (  1) // Input Height
#define C2_IY    (  1) // Input Width
#define C2_WY    (  1) // Weight Height (Kernel Height)
#define C2_WX    (  1) // Weight Width (Kernel Width)
#define C2_SY    (  1) // Input Stride Y
#define C2_SX    (  1) // Input Stride X
#define C2_BIAS  (128) // Bias Value
#define C2_SHIFT (  8) // Output Left Shift

#define C2_O_SIZE (C2_ON*C2_OY*C2_OX)
#define C2_I_SIZE (C2_IN*C2_IY*C2_IX)
#define C2_W_SIZE (C2_ON*C2_WX*C2_WY*C2_IN)

#define C3_ON    (  10) // Number of Outputs               
#define C3_OY    (   1) // Output Height
#define C3_OX    (   1) // Output Width
#define C3_IN    ( 150) // Number of Inputs
#define C3_IX    (   1) // Input Height
#define C3_IY    (   1) // Input Width
#define C3_WY    (   1) // Weight Height (Kernel Height)
#define C3_WX    (   1) // Weight Width (Kernel Width)
#define C3_SY    (   1) // Input Stride Y
#define C3_SX    (   1) // Input Stride X
#define C3_BIAS  (1024) // Bias Value
#define C3_SHIFT (  11) // Output Left Shift

#define C3_O_SIZE (C3_ON*C3_OY*C3_OX)
#define C3_I_SIZE (C3_IN*C3_IY*C3_IX)
#define C3_W_SIZE (C3_ON*C3_WX*C3_WY*C3_IN)

static uint8_t c0_out[C0_O_SIZE];
static uint8_t c1_out[C1_O_SIZE];
static uint8_t c2_out[C2_O_SIZE];
static uint8_t c3_out[C3_O_SIZE];

static void c0(
    uint8_t *output,
    const uint8_t *input,
    const int8_t *weight
)
{
    size_t avl;
    size_t vl;

    for (size_t oy = 0; oy < C0_OX; oy++) {
        for (size_t ox = 0; ox < C0_OY; ox++) {
            for (size_t on = 0; on < C0_ON; on += vl) {
                avl = C0_ON - on;
                vl = (avl <= MAXVL) ? avl : MAXVL; 
            
                vec_t sum_v = vmv_vx(C0_BIAS, vl);

                for (size_t wy = 0; wy < C0_WY; wy++) {
                    for (size_t wx = 0; wx < C0_WX; wx++) {
                        for (size_t in = 0; in < C0_IN; in++) {   
                            size_t iy = C0_SY*oy + wy;
                            size_t ix = C0_SX*ox + wx;
                            size_t i = (iy*C0_IX + ix)*C0_IN + in;

                            size_t w = ((in*C0_WY + wy)*C0_WX + wx)*C0_ON + on;                
                            vec_t weight_v = vlei8(&weight[w], vl);
                            
                            sum_v = vmacc_vx(sum_v, input[i], weight_v, vl);
                        }
                    }
                }

                sum_v = vmax_vx(sum_v, 0, vl);

                size_t o = (oy*C0_OX + ox)*C0_ON + on;
                vnclip_vse8(&output[o], sum_v, C0_SHIFT, vl);        
            }  
        }
    }
}

static void c1(
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
                vnclip_vse8(&output[o], sum_v, C1_SHIFT, vl);        
            }  
        }
    }
}

static void c2(
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
                vnclip_vse8(&output[o], sum_v, C2_SHIFT, vl);        
            }  
        }
    }
}

static void c3(
    uint8_t *output,
    const uint8_t *input,
    const int8_t *weight
)
{
    size_t avl;
    size_t vl;

    for (size_t oy = 0; oy < C3_OX; oy++) {
        for (size_t ox = 0; ox < C3_OY; ox++) {
            for (size_t on = 0; on < C3_ON; on += vl) {
                avl = C3_ON - on;
                vl = (avl <= MAXVL) ? avl : MAXVL; 
            
                vec_t sum_v = vmv_vx(C3_BIAS, vl);

                for (size_t wy = 0; wy < C3_WY; wy++) {
                    for (size_t wx = 0; wx < C3_WX; wx++) {
                        for (size_t in = 0; in < C3_IN; in++) {   
                            size_t iy = C3_SY*oy + wy;
                            size_t ix = C3_SX*ox + wx;
                            size_t i = (iy*C3_IX + ix)*C3_IN + in;

                            size_t w = ((in*C3_WY + wy)*C3_WX + wx)*C3_ON + on;                
                            vec_t weight_v = vlei8(&weight[w], vl);
                            
                            sum_v = vmacc_vx(sum_v, input[i], weight_v, vl);
                        }
                    }
                }

                sum_v = vmax_vx(sum_v, 0, vl); // handles negative clipping

                size_t o = (oy*C3_OX + ox)*C3_ON + on;
                vnclip_vse8(&output[o], sum_v, C3_SHIFT, vl);        
            }  
        }
    }
}

static void argmax1(
    int32_t *output,
    uint8_t *max,
    const uint8_t *input
) 
{
    *output = 0;
    *max = input[*output];

    for(size_t i = 1; i < C3_O_SIZE; i++) {
        if(input[i] > *max) {
            *output = i;
            *max = input[i];
        }
    }
}

void inference(const uint8_t* input, int32_t* output, uint8_t* credence)
{

#ifdef VALIDATION_RUN
    ASSERT(MAXVL < vsetvlmax());
    
    uint32_t crc;
    crc32_table_init();

    crc = 0;
    crc32(&crc, input, C0_I_SIZE);
    ASSERT(crc == 0x4dfde263);
#endif

    c0(c0_out, input, c0_weight);

#ifdef VALIDATION_RUN
    crc = 0;
    crc32(&crc, c0_out, C0_O_SIZE);
    ASSERT(crc == 0xa6062dba);
#endif

    c1(c1_out, c0_out, c1_weight);

#ifdef VALIDATION_RUN    
    crc = 0;
    crc32(&crc, c1_out, C1_O_SIZE);
    ASSERT(crc == 0x0aa1524f);
#endif

    c2(c2_out, c1_out, c2_weight);

#ifdef VALIDATION_RUN 
    crc = 0;
    crc32(&crc, c2_out, C2_O_SIZE);
    ASSERT(crc == 0x7e1d772e);
#endif

    c3(c3_out, c2_out, c3_weight);

#ifdef VALIDATION_RUN
    crc = 0;
    crc32(&crc, c3_out, C3_O_SIZE);
    ASSERT(crc == 0x6d779679);
#endif

    argmax1(output, credence, c3_out);

#ifdef VALIDATION_RUN
    crc = 0;
    crc32(&crc, output, 1);
    ASSERT(crc == 0xd56f2b94);
#endif

}





