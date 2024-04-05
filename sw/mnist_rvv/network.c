#include "network.h"

#define L0_ON    ( 16) // Number of Outputs
#define L0_OY    ( 11) // Output Height
#define L0_OX    ( 11) // Output Width
#define L0_IN    (  1) // Number of Inputs
#define L0_IY    ( 24) // Input Height
#define L0_IX    ( 24) // Input Width
#define L0_WY    (  4) // Weight Height (Kernel Height)
#define L0_WX    (  4) // Weight Width (Kernel Width)
#define L0_SY    (  2) // Input Stride Y
#define L0_SX    (  2) // Input Stride X
#define L0_BIAS  (128) // Bias Value
#define L0_SHIFT (  8) // Output Left Shift

#define L0_O_SIZE (L0_ON*L0_OY*L0_OX)
#define L0_I_SIZE (L0_IN*L0_IY*L0_IX)
#define L0_W_SIZE (L0_ON*L0_WX*L0_WY*L0_IN)

#define L1_ON    ( 24) // Number of Outputs
#define L1_OY    (  4) // Output Height
#define L1_OX    (  4) // Output Width
#define L1_IN    ( 16) // Number of Inputs
#define L1_IX    ( 11) // Input Height
#define L1_IY    ( 11) // Input Width
#define L1_WY    (  5) // Weight Height (Kernel Height)
#define L1_WX    (  5) // Weight Width (Kernel Width)
#define L1_SY    (  2) // Input Stride Y
#define L1_SX    (  2) // Input Stride X
#define L1_BIAS  (128) // Bias Value
#define L1_SHIFT (  8) // Output Left Shift

#define L1_O_SIZE (L1_ON*L1_OY*L1_OX)
#define L1_I_SIZE (L1_IN*L1_IY*L1_IX)
#define L1_W_SIZE (L1_ON*L1_WX*L1_WY*L1_IN)

#define L2_ON    (150) // Number of Outputs
#define L2_OY    (  1) // Output Height
#define L2_OX    (  1) // Output Width
#define L2_IN    (384) // Number of Inputs
#define L2_IX    (  1) // Input Height
#define L2_IY    (  1) // Input Width
#define L2_WY    (  1) // Weight Height (Kernel Height)
#define L2_WX    (  1) // Weight Width (Kernel Width)
#define L2_SY    (  1) // Input Stride Y
#define L2_SX    (  1) // Input Stride X
#define L2_BIAS  (128) // Bias Value
#define L2_SHIFT (  8) // Output Left Shift

#define L2_O_SIZE (L2_ON*L2_OY*L2_OX)
#define L2_I_SIZE (L2_IN*L2_IY*L2_IX)
#define L2_W_SIZE (L2_ON*L2_WX*L2_WY*L2_IN)

#define L3_ON    (  10) // Number of Outputs
#define L3_OY    (   1) // Output Height
#define L3_OX    (   1) // Output Width
#define L3_IN    ( 150) // Number of Inputs
#define L3_IX    (   1) // Input Height
#define L3_IY    (   1) // Input Width
#define L3_WY    (   1) // Weight Height (Kernel Height)
#define L3_WX    (   1) // Weight Width (Kernel Width)
#define L3_SY    (   1) // Input Stride Y
#define L3_SX    (   1) // Input Stride X
#define L3_BIAS  (1024) // Bias Value
#define L3_SHIFT (  11) // Output Left Shift

#define L3_O_SIZE (L3_ON*L3_OY*L3_OX)
#define L3_I_SIZE (L3_IN*L3_IY*L3_IX)
#define L3_W_SIZE (L3_ON*L3_WX*L3_WY*L3_IN)

static uint8_t l0_out[L0_O_SIZE];
static uint8_t l1_out[L1_O_SIZE];
static uint8_t l2_out[L2_O_SIZE];
static uint8_t l3_out[L3_O_SIZE];

#define CONV(L, O, I, W) \
do { \
    size_t avl; \
    size_t vl; \
    \
    for (size_t oy = 0; oy < L##_OX; oy++) { \
        for (size_t ox = 0; ox < L##_OY; ox++) { \
            for (size_t on = 0; on < L##_ON; on += vl) { \
                avl = L##_ON - on; \
                vl = (avl <= MAXVL) ? avl : MAXVL; \
                \
                vec_t sum_v = vmv_vx(L##_BIAS, vl); \
                \
                for (size_t wy = 0; wy < L##_WY; wy++) { \
                    for (size_t wx = 0; wx < L##_WX; wx++) { \
                        for (size_t in = 0; in < L##_IN; in++) { \
                            size_t iy = L##_SY * oy + wy; \
                            size_t ix = L##_SX * ox + wx; \
                            size_t i = (iy * L##_IX + ix) * L##_IN + in; \
                            \
                            size_t w = ((in * L##_WY + wy) * L##_WX + wx) * L##_ON + on; \
                            vec_t weight_v = vlei8(&W[w], vl); \
                            \
                            sum_v = vmacc_vx(sum_v, I[i], weight_v, vl); \
                        } \
                    } \
                } \
                \
                sum_v = vmax_vx(sum_v, 0, vl); \
                \
                size_t o = (oy * L##_OX + ox) * L##_ON + on; \
                vnclip_vse8(&O[o], sum_v, L##_SHIFT, vl); \
            } \
        } \
    } \
} while(0)

static void argmax1(
    int32_t *output,
    uint8_t *max,
    const uint8_t *input
)
{
    *output = 0;
    *max = input[*output];

    for(size_t i = 1; i < L3_O_SIZE; i++) {
        if(input[i] > *max) {
            *output = i;
            *max = input[i];
        }
    }
}

void inference(const uint8_t* input, int32_t* output, uint8_t* credence)
{

    // Set RVV rounding mode to round-down (truncate).
    asm volatile ("csrw vxrm, %0" :: "rK"(0b10));

#ifdef VALIDATION_RUN
    ASSERT(MAXVL < vsetvlmax());

    uint32_t crc;
    crc32_table_init();

    crc = 0;
    crc32(&crc, input, L0_I_SIZE);
    ASSERT(crc == 0x4dfde263);
#endif

    // conv1
    CONV(L0, l0_out, input, l0_weight);

#ifdef VALIDATION_RUN
    crc = 0;
    crc32(&crc, l0_out, L0_O_SIZE);
    ASSERT(crc == 0xa6062dba);
#endif

    // conv2
    CONV(L1, l1_out, l0_out, l1_weight);

#ifdef VALIDATION_RUN
    crc = 0;
    crc32(&crc, l1_out, L1_O_SIZE);
    ASSERT(crc == 0x0aa1524f);
#endif

    // fc1
    CONV(L2, l2_out, l1_out, l2_weight);

#ifdef VALIDATION_RUN
    crc = 0;
    crc32(&crc, l2_out, L2_O_SIZE);
    ASSERT(crc == 0x7e1d772e);
#endif

    // fc2
    CONV(L3, l3_out, l2_out, l3_weight);

#ifdef VALIDATION_RUN
    crc = 0;
    crc32(&crc, l3_out, L3_O_SIZE);
    ASSERT(crc == 0x6d779679);
#endif

    argmax1(output, credence, l3_out);

#ifdef VALIDATION_RUN
    crc = 0;
    crc32(&crc, output, 1);
    ASSERT(crc == 0xd56f2b94);
#endif

}
