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
static DATA_T mem[MEMORY_SIZE];

static int max(int lhs, int rhs) {
        return (lhs >= rhs)?lhs:rhs;
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

// static void macsOnRange(const uint8_t* __restrict inputs,
//                         const int8_t* __restrict weights,
//                         int32_t* __restrict weightedSum,
//                         int nb_iterations)
// {
//     for (int iter = 0; iter < nb_iterations; ++iter) {
//         *weightedSum += inputs[iter] * weights[iter];
//     }
// }

static void macsOnRange(const uint8_t* __restrict inputs,
                        const int8_t* __restrict weights,
                        int32_t* __restrict weightedSum,
                        int nb_iterations)
{
    int iter = 0;
    int32_t sum = *weightedSum;

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

static UDATA_T saturate(SUM_T value, uint32_t sat) {
    return clamp(value, (SUM_T)(0), ((SUM_T)(1) << sat) - 1);
}

static UDATA_T sat(SUM_T weightedSum, int output,
                                           ActivationFunction_T func,
                                           /* const Rescaling_T& __restrict rescaling */
                                           int shift)
{
    switch(func) {
        case Linear:
        case Saturation: {
            break;
        }
        case Rectifier: {
            if(weightedSum <= 0) weightedSum = 0;
            break;
        }
        default:
            printf("Unsupported activation function.\n");
            break;
    }

    return saturate(weightedSum>>shift, NB_BITS);
}


static void convcellPropagate1(
    const UDATA_T* __restrict inputs,
    UDATA_T* __restrict outputs,
    const BDATA_T* __restrict biasses,
    const WDATA_T* __restrict weights,
    int rescaling,
    int NB_CHANNELS, 
    int CHANNELS_HEIGHT, int CHANNELS_WIDTH,
    int NB_OUTPUTS,
    int OUTPUTS_HEIGHT, int OUTPUTS_WIDTH,
    int PADDING_Y, int PADDING_X,
    int STRIDE_Y, int STRIDE_X,
    int KERNEL_HEIGHT, int KERNEL_WIDTH,
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
    int OUTPUTS_HEIGHT_NOPAD
        = (CHANNELS_HEIGHT - KERNEL_HEIGHT + STRIDE_Y) / STRIDE_Y;
    int OUTPUTS_WIDTH_NOPAD
        = (CHANNELS_WIDTH - KERNEL_WIDTH + STRIDE_X) / STRIDE_X;

    for (int oy = 0; oy < OUTPUTS_HEIGHT; ++oy) {
        const int syMin = (PADDING_Y == 0) ? 0
            : max(PADDING_Y - (oy * STRIDE_Y), 0);
        const int syMax = (PADDING_Y == 0
                && OUTPUTS_HEIGHT == OUTPUTS_HEIGHT_NOPAD) ? KERNEL_HEIGHT
            : clamp(CHANNELS_HEIGHT + PADDING_Y - (oy * STRIDE_Y), 
                    0, KERNEL_HEIGHT);
        const int iy = (oy * STRIDE_Y) - PADDING_Y;

        for (int ox = 0; ox < OUTPUTS_WIDTH; ++ox) {
            for (int output = 0; output < NB_OUTPUTS; ++output) {
                // moved to inner loop for collapsing -->
                const int sxMin = (PADDING_X == 0) ? 0
                    : max(PADDING_X - (ox * STRIDE_X), 0);
                const int sxMax = (PADDING_X == 0
                        && OUTPUTS_WIDTH == OUTPUTS_WIDTH_NOPAD)
                            ? KERNEL_WIDTH
                    : clamp(CHANNELS_WIDTH + PADDING_X - (ox * STRIDE_X), 
                            0, KERNEL_WIDTH);
                const int ix = (ox * STRIDE_X) - PADDING_X;

                const int oPos = (ox + OUTPUTS_WIDTH * oy);
                int oOffset = OUTPUT_MEM_STRIDE * oPos;

                if (OUTPUT_MEM_WRAP_SIZE > 0 && oOffset >= OUTPUT_MEM_CONT_SIZE) {
                    oOffset += OUTPUT_MEM_WRAP_OFFSET - OUTPUT_MEM_CONT_OFFSET
                                - OUTPUT_MEM_CONT_SIZE;
                }
                // <--

                SUM_T weightedSum = biasses[output];

                for (int sy = 0; sy < KERNEL_HEIGHT; ++sy) {
                    if ((PADDING_Y != 0
                            || OUTPUTS_HEIGHT != OUTPUTS_HEIGHT_NOPAD)
                        && sy >= syMax - syMin)
                    {
                       break;
                    }

                    const int iPos = ((sxMin + ix)
                                        + CHANNELS_WIDTH * (iy + syMin + sy));
                    int iOffset = INPUT_MEM_STRIDE * iPos;

                    // Wrapping cannot occur in the middle of a line, except if
                    // there is only one line (1D)!
                    bool wrapInRange = false;

                    if (INPUT_MEM_WRAP_SIZE > 0
                        && iOffset >= INPUT_MEM_CONT_SIZE)
                    {
                        iOffset += INPUT_MEM_WRAP_OFFSET - INPUT_MEM_CONT_OFFSET
                                    - INPUT_MEM_CONT_SIZE;
                    }
                    else if (INPUT_MEM_WRAP_SIZE > 0 && KERNEL_WIDTH > 1
                        && CHANNELS_HEIGHT == 1 // single line (1D)!
                        && iOffset + KERNEL_WIDTH * NB_CHANNELS
                            > INPUT_MEM_CONT_SIZE)
                    {
                        wrapInRange = true;
                    }

                    const int wOffset = NB_CHANNELS * (sxMin
                        + KERNEL_WIDTH * (syMin + sy + KERNEL_HEIGHT * output));

                    if (!wrapInRange && (NB_CHANNELS == INPUT_MEM_STRIDE
                        && ((PADDING_X == 0
                            && OUTPUTS_WIDTH == OUTPUTS_WIDTH_NOPAD)
                                || sxMax - sxMin == KERNEL_WIDTH)))
                    {
                        macsOnRange(
                            inputs + iOffset, 
                            weights + wOffset, 
                            &weightedSum,KERNEL_WIDTH * NB_CHANNELS);
                    }
                    else {
                        for (int sx = 0; sx < KERNEL_WIDTH; ++sx) {
                            if ((PADDING_X != 0
                                    || OUTPUTS_WIDTH != OUTPUTS_WIDTH_NOPAD)
                                && sx >= sxMax - sxMin)
                            {
                                break;
                            }

                            int iOffsetInRange = iOffset
                                + sx * INPUT_MEM_STRIDE;

                            if (wrapInRange
                                && iOffsetInRange >= INPUT_MEM_CONT_SIZE)
                            {
                                iOffsetInRange += INPUT_MEM_WRAP_OFFSET
                                            - INPUT_MEM_CONT_OFFSET
                                            - INPUT_MEM_CONT_SIZE;
                            }

                            macsOnRange(
                                // same input line so no wrapping can occur
                                inputs + iOffsetInRange, 
                                weights + wOffset + sx * NB_CHANNELS, 
                                &weightedSum,NB_CHANNELS);
                        }
                    }
                }

                outputs[oOffset + output]
                    = sat(weightedSum, output, ACTIVATION, rescaling);
            }
        }
    }
}
/*static void convcellPropagate3(
    const UDATA_T* __restrict inputs,
    UDATA_T* __restrict outputs,
    const BDATA_T* __restrict biasses,
    const WDATA_T* __restrict weights,
    int rescaling,
    ActivationFunction_T ACTIVATION
)
{

    for (int oy = 0; oy < CONV1_OUTPUTS_HEIGHT; ++oy) {
        #if CONV1_PADDING_Y == 0              // on our code the padding = 0, because our neural network was trained to use = 0
            const int syMin = 0;
            #if CONV1_OUTPUTS_HEIGHT == CONV1_OUTPUTS_HEIGHT_NOPAD
                const int syMax = CONV1_KERNEL_HEIGHT;
            #else
                const int syMax = clamp(CONV1_CHANNELS_HEIGHT + CONV1_PADDING_Y - (oy * CONV1_STRIDE_Y),0, CONV1_KERNEL_HEIGHT);
            #endif
            const int iy = (oy * CONV1_STRIDE_Y);
        #else
            const int syMin = max(CONV1_PADDING_Y - (oy * CONV1_STRIDE_Y), 0);
            const int syMax = clamp(CONV1_CHANNELS_HEIGHT + CONV1_PADDING_Y - (oy * CONV1_STRIDE_Y),0, CONV1_KERNEL_HEIGHT);
            const int iy = (oy * CONV1_STRIDE_Y) - CONV1_PADDING_Y;
        #endif

        for (int ox = 0; ox < CONV1_OUTPUTS_WIDTH; ++ox) {
            for (int output = 0; output < CONV1_NB_OUTPUTS; ++output) {
                // moved to inner loop for collapsing -->
                #if CONV1_PADDING_X == 0
                    const int sxMin = 0;
                    #if CONV1_OUTPUTS_WIDTH == CONV1_OUTPUTS_WIDTH_NOPAD
                        const int sxMax = CONV1_KERNEL_WIDTH;
                    #else
                        const int sxMax =clamp(CONV1_CHANNELS_WIDTH + CONV1_PADDING_X - (ox * CONV1_STRIDE_X),  0, CONV1_KERNEL_WIDTH);
                    #endif
                    const ix = (ox * CONV1_STRIDE_X);
                #else
                    const int sxMin = (CONV1_PADDING_X == 0) ? 0 : max(CONV1_PADDING_X - (ox * CONV1_STRIDE_X), 0);
                    const int sxMax = (CONV1_PADDING_X == 0 && CONV1_OUTPUTS_WIDTH == CONV1_OUTPUTS_WIDTH_NOPAD) ? 
                    CONV1_KERNEL_WIDTH : clamp(CONV1_CHANNELS_WIDTH + CONV1_PADDING_X - (ox * CONV1_STRIDE_X), 0, CONV1_KERNEL_WIDTH);
                    const int ix = (ox * CONV1_STRIDE_X) - CONV1_PADDING_X;
                #endif

                int oOffset = CONV1_MEM_STRIDE *(ox + (CONV1_OUTPUTS_WIDTH * oy));
                // moved to inner loop for collapsing -->

                
                #if CONV1_MEM_WRAP_SIZE > 0
                    if( oOffset >= CONV1_MEM_CONT_SIZE){
                        oOffset += CONV1_MEM_WRAP_OFFSET - CONV1_MEM_CONT_OFFSET - CONV1_MEM_CONT_SIZE;
                    }
                #endif

                SUM_T weightedSum = biasses[output];

                for (int sy = 0; sy < CONV1_KERNEL_HEIGHT; ++sy) {
                    #if (CONV1_PADDING_Y != 0 || (CONV1_OUTPUTS_HEIGHT != CONV1_OUTPUTS_HEIGHT_NOPAD))
                        if(sy >= syMax - syMin) break;
                    #endif 


                    #if CONV1_PADDING_X == 0
                        #if ENV_MEM_STRIDE == 1
                            int iOffset = ix + (CONV1_CHANNELS_WIDTH * (iy + syMin + sy));
                        #else
                            int iOffset = ENV_MEM_STRIDE * (ix + (CONV1_CHANNELS_WIDTH * (iy+syMin +sy)));
                        #endif
                    #else 
                        #if ENV_MEM_STRIDE == 1
                            const int iOffset = ((sxMin + ix) + CONV1_CHANNELS_WIDTH * (iy + syMin + sy));
                        #else
                            int iOffset = ENV_MEM_STRIDE * (((sxMin + ix) + CONV1_CHANNELS_WIDTH * (iy + syMin + sy)));
                        #endif
                    #endif


                    // Wrapping cannot occur in the middle of a line, except if
                    // there is only one line (1D)!
                    bool wrapInRange = false;
                    #if ENV_MEM_WRAP_SIZE > 0 
                        if(iOffset >= ENV_MEM_CONT_SIZE){
                            iOffset += ENV_MEM_WRAP_OFFSET - ENV_MEM_CONT_OFFSET - ENV_MEM_CONT_SIZE;
                        }
                    #elif (ENV_MEM_CONT_SIZE > 0 && CONV1_KERNEL_WIDTH > 1 && CONV1_CHANNELS_HEIGHT == 1) 
                        if(iOffset + CONV1_KERNEL_WIDTH * CONV1_NB_CHANNELS > ENV_MEM_CONT_SIZE){
                            wrapInRange = true;
                        }
                    #endif

                    #if CONV1_PADDING_X == 0
                        #if CONV1_PADDING_Y == 0
                            const int wOffset = CONV1_NB_CHANNELS * (CONV1_KERNEL_WIDTH * (sy + (CONV1_KERNEL_HEIGHT * output)));
                        #else 
                            const int wOffset = CONV1_NB_CHANNELS * (CONV1_KERNEL_WIDTH * (syMin + sy + CONV1_KERNEL_HEIGHT * output));
                        #endif
                    #else
                        #if CONV1_PADDING_Y == 0
                            const int wOffset = CONV1_NB_CHANNELS * ( sxMin + CONV1_KERNEL_WIDTH * (sy + CONV1_KERNEL_HEIGHT * output));
                        #else 
                            const int wOffset = CONV1_NB_CHANNELS * ( sxMin + CONV1_KERNEL_WIDTH * (syMin + sy + CONV1_KERNEL_HEIGHT * output));
                        #endif
                    #endif

                    #if (CONV1_PADDING_X == 0 && CONV1_OUTPUTS_WIDTH == CONV1_OUTPUTS_WIDTH_NOPAD)  
                            #if (CONV1_NB_CHANNELS == ENV_MEM_STRIDE && (CONV1_PADDING_X == 0 && CONV1_OUTPUTS_WIDTH == CONV1_OUTPUTS_WIDTH_NOPAD))
                                if(!wrapInRange){               //sxMax - sxMin == KERNEL_WIDTH 
                                macsOnRange( inputs + iOffset,  weights + wOffset,  &weightedSum,(CONV1_KERNEL_WIDTH * CONV1_NB_CHANNELS));
                            }
                            else{
                                for (int sx = 0; sx < CONV1_KERNEL_WIDTH; ++sx) {
                                    #if (CONV1_PADDING_X != 0 || CONV1_OUTPUTS_WIDTH != CONV1_OUTPUTS_WIDTH_NOPAD)
                                        if(sx >= sxMax - sxMin) break;
                                    #endif

                                    int iOffsetInRange = iOffset + sx * ENV_MEM_STRIDE;

                                    if (wrapInRange && iOffsetInRange >= ENV_MEM_CONT_SIZE)
                                    {
                                        iOffsetInRange += ENV_MEM_WRAP_OFFSET - ENV_MEM_CONT_OFFSET - ENV_MEM_CONT_SIZE;
                                    }

                                    macsOnRange( inputs + iOffsetInRange,  weights + wOffset + sx * CONV1_NB_CHANNELS, &weightedSum,CONV1_NB_CHANNELS);
                                }           
                            }
                            #endif
                    #else
                        for (int sx = 0; sx < CONV1_KERNEL_WIDTH; ++sx) {
                                    #if (CONV1_PADDING_X != 0 || CONV1_OUTPUTS_WIDTH != CONV1_OUTPUTS_WIDTH_NOPAD)
                                        if(sx >= sxMax - sxMin) break;
                                    #endif

                                    int iOffsetInRange = iOffset + sx * ENV_MEM_STRIDE;

                                    if (wrapInRange && iOffsetInRange >= ENV_MEM_CONT_SIZE)
                                    {
                                        iOffsetInRange += ENV_MEM_WRAP_OFFSET - ENV_MEM_CONT_OFFSET - ENV_MEM_CONT_SIZE;
                                    }

                                    macsOnRange( inputs + iOffsetInRange,  weights + wOffset + sx * CONV1_NB_CHANNELS, &weightedSum,CONV1_NB_CHANNELS);
                                }     
                    #endif
                }
                outputs[oOffset + output] = sat(weightedSum, output, ACTIVATION, rescaling);
            }
        }
    }
}
*/
static void convcellPropagate2(
    const UDATA_T* __restrict inputs,
    UDATA_T* __restrict outputs,
    const BDATA_T* __restrict biasses,
    const WDATA_T* __restrict weights,
    int rescaling,
    ActivationFunction_T ACTIVATION)
{

    for (int oy = 0; oy < CONV2_OUTPUTS_HEIGHT; ++oy) {
        #if CONV2_PADDING_Y == 0              // on our code the padding = 0, because our neural network was trained to use = 0
            const int syMin = 0;
            #if CONV2_OUTPUTS_HEIGHT == CONV2_OUTPUTS_HEIGHT_NOPAD
                const int syMax = CONV2_KERNEL_HEIGHT;
            #else
                const int syMax = clamp(CONV2_CHANNELS_HEIGHT + CONV2_PADDING_Y - (oy * CONV2_STRIDE_Y),0, CONV2_KERNEL_HEIGHT);
            #endif
            const int iy = (oy * CONV2_STRIDE_Y);
        #else
            const int syMin = max(CONV2_PADDING_Y - (oy * CONV2_STRIDE_Y), 0);
            const int syMax = clamp(CONV2_CHANNELS_HEIGHT + CONV2_PADDING_Y - (oy * CONV2_STRIDE_Y),0, CONV2_KERNEL_HEIGHT);
            const int iy = (oy * CONV2_STRIDE_Y) - CONV2_PADDING_Y;
        #endif

        for (int ox = 0; ox < CONV2_OUTPUTS_WIDTH; ++ox) {
            for (int output = 0; output < CONV2_NB_OUTPUTS; ++output) {
                // moved to inner loop for collapsing -->
                #if CONV2_PADDING_X == 0
                    const int sxMin = 0;
                    #if CONV2_OUTPUTS_WIDTH == CONV2_OUTPUTS_WIDTH_NOPAD
                        const int sxMax = CONV2_KERNEL_WIDTH;
                    #else
                        const int sxMax =clamp(CONV2_CHANNELS_WIDTH + CONV2_PADDING_X - (ox * CONV2_STRIDE_X),  0, CONV2_KERNEL_WIDTH);
                    #endif
                    const ix = (ox * CONV2_STRIDE_X);
                #else
                    const int sxMin = (CONV2_PADDING_X == 0) ? 0 : max(CONV2_PADDING_X - (ox * CONV2_STRIDE_X), 0);
                    const int sxMax = (CONV2_PADDING_X == 0 && CONV2_OUTPUTS_WIDTH == CONV2_OUTPUTS_WIDTH_NOPAD) ?
                    CONV2_KERNEL_WIDTH : clamp(CONV2_CHANNELS_WIDTH + CONV2_PADDING_X - (ox * CONV2_STRIDE_X), 0, CONV2_KERNEL_WIDTH);
                    const int ix = (ox * CONV2_STRIDE_X) - CONV2_PADDING_X;
                #endif

                int oOffset = CONV2_MEM_STRIDE *(ox + (CONV2_OUTPUTS_WIDTH * oy));
                
                #if CONV2_MEM_WRAP_SIZE > 0
                    if( oOffset >= CONV2_MEM_CONT_SIZE){
                        oOffset += CONV2_MEM_WRAP_OFFSET - CONV2_MEM_CONT_OFFSET - CONV2_MEM_CONT_SIZE;
                    }
                #endif

                SUM_T weightedSum = biasses[output];

                for (int sy = 0; sy < CONV2_KERNEL_HEIGHT; ++sy) {
                    #if (CONV2_PADDING_Y != 0 || (CONV2_OUTPUTS_HEIGHT != CONV2_OUTPUTS_HEIGHT_NOPAD))
                        if(sy >= syMax - syMin) break;
                    #endif 


                    #if CONV2_PADDING_X == 0
                        #if CONV1_MEM_STRIDE == 1
                            int iOffset = ix + (CONV2_CHANNELS_WIDTH * (iy + syMin + sy));
                        #else
                            int iOffset = CONV1_MEM_STRIDE * (ix + (CONV2_CHANNELS_WIDTH * (iy+syMin +sy)));
                        #endif
                    #else 
                        #if CONV1_MEM_STRIDE == 1
                            const int iOffset = ((sxMin + ix) + CONV2_CHANNELS_WIDTH * (iy + syMin + sy));
                        #else
                            int iOffset = CONV1_MEM_STRIDE * (((sxMin + ix) + CONV2_CHANNELS_WIDTH * (iy + syMin + sy)));
                        #endif
                    #endif

                    // Wrapping cannot occur in the middle of a line, except if
                    // there is only one line (1D)!
                    bool wrapInRange = false;
                    #if CONV1_MEM_WRAP_SIZE > 0 
                        if(iOffset >= CONV1_MEM_CONT_SIZE){
                            iOffset += CONV1_MEM_WRAP_OFFSET - CONV1_MEM_CONT_OFFSET - CONV1_MEM_CONT_SIZE;
                        }
                    #elif (CONV1_MEM_CONT_SIZE > 0 && CONV2_KERNEL_WIDTH > 1 && CONV2_CHANNELS_HEIGHT == 1) 
                        if(iOffset + CONV2_KERNEL_WIDTH * CONV2_NB_CHANNELS > CONV1_MEM_CONT_SIZE){
                            wrapInRange = true;
                        }
                    #endif

                    #if CONV2_PADDING_X == 0
                        #if CONV2_PADDING_Y == 0
                            const int wOffset = CONV2_NB_CHANNELS * (CONV2_KERNEL_WIDTH * (sy + (CONV2_KERNEL_HEIGHT * output)));
                        #else 
                            const int wOffset = CONV2_NB_CHANNELS * (CONV2_KERNEL_WIDTH * (syMin + sy + CONV2_KERNEL_HEIGHT * output));
                        #endif
                    #else
                        #if CONV2_PADDING_Y == 0
                            const int wOffset = CONV2_NB_CHANNELS * ( sxMin + CONV2_KERNEL_WIDTH * (sy + CONV2_KERNEL_HEIGHT * output));
                        #else 
                            const int wOffset = CONV2_NB_CHANNELS * ( sxMin + CONV2_KERNEL_WIDTH * (syMin + sy + CONV2_KERNEL_HEIGHT * output));
                        #endif
                    #endif


                    #if (CONV2_PADDING_X == 0 && CONV2_OUTPUTS_WIDTH == CONV2_OUTPUTS_WIDTH_NOPAD) /*  sxMax == KERNEL_WIDTH  &&  sxMin = 0*/ 
                            #if (CONV2_NB_CHANNELS == CONV1_MEM_STRIDE && (CONV2_PADDING_X == 0 && CONV2_OUTPUTS_WIDTH == CONV2_OUTPUTS_WIDTH_NOPAD))
                                if(!wrapInRange){               //sxMax - sxMin == KERNEL_WIDTH                                                                                             //sxMax - sxMin == KERNEL_WIDTH
                                macsOnRange( inputs + iOffset,  weights + wOffset,  &weightedSum,(CONV2_KERNEL_WIDTH * CONV2_NB_CHANNELS));
                            }
                            else{
                                for (int sx = 0; sx < CONV2_KERNEL_WIDTH; ++sx) {
                                    #if (CONV2_PADDING_X != 0 || CONV2_OUTPUTS_WIDTH != CONV2_OUTPUTS_WIDTH_NOPAD)
                                        if(sx >= sxMax - sxMin) break;
                                    #endif

                                    int iOffsetInRange = iOffset + sx * CONV1_MEM_STRIDE;

                                    if (wrapInRange && iOffsetInRange >= CONV1_MEM_CONT_SIZE)
                                    {
                                        iOffsetInRange += CONV1_MEM_WRAP_OFFSET - CONV1_MEM_CONT_OFFSET - CONV1_MEM_CONT_SIZE;
                                    }

                                    macsOnRange( inputs + iOffsetInRange,  weights + wOffset + sx * CONV2_NB_CHANNELS, &weightedSum,CONV2_NB_CHANNELS);
                                }           
                            }
                            #endif
                    #else
                        for (int sx = 0; sx < CONV2_KERNEL_WIDTH; ++sx) {
                                    #if (CONV2_PADDING_X != 0 || CONV2_OUTPUTS_WIDTH != CONV2_OUTPUTS_WIDTH_NOPAD)
                                        if(sx >= sxMax - sxMin) break;
                                    #endif

                                    int iOffsetInRange = iOffset + sx * CONV1_MEM_STRIDE;

                                    if (wrapInRange && iOffsetInRange >= CONV1_MEM_CONT_SIZE)
                                    {
                                        iOffsetInRange += CONV1_MEM_WRAP_OFFSET - CONV1_MEM_CONT_OFFSET - CONV1_MEM_CONT_SIZE;
                                    }

                                    macsOnRange( inputs + iOffsetInRange,  weights + wOffset + sx * CONV2_NB_CHANNELS, &weightedSum,CONV2_NB_CHANNELS);
                                }     
                    #endif
                }
                outputs[oOffset + output] = sat(weightedSum, output, ACTIVATION, rescaling);
            }
        }
    }
}


static void fccellPropagateUDATA_T(
    const UDATA_T* __restrict inputs,
    UDATA_T* __restrict outputs,
    const BDATA_T* __restrict biasses,
    const WDATA_T* __restrict weights,
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
        SUM_T weightedSum = biasses[och];

        for (int iy = 0; iy < CHANNELS_HEIGHT; ++iy) {
            const int iPos = (CHANNELS_WIDTH * iy);
            int iOffset = INPUT_MEM_STRIDE * iPos;

            // Wrapping cannot occur in the middle of a line, except if
            // there is only one line (1D)!
            bool wrapInRange = false;

            if (INPUT_MEM_WRAP_SIZE > 0 && iOffset >= INPUT_MEM_CONT_SIZE) {
                iOffset += INPUT_MEM_WRAP_OFFSET - INPUT_MEM_CONT_OFFSET - INPUT_MEM_CONT_SIZE;
            }
            else if (INPUT_MEM_WRAP_SIZE > 0 && CHANNELS_WIDTH > 1 && CHANNELS_HEIGHT == 1 && iOffset + CHANNELS_WIDTH * NB_CHANNELS > INPUT_MEM_CONT_SIZE)
            {
                wrapInRange = true;
            }

            const int wOffset = NB_CHANNELS * CHANNELS_WIDTH * (iy + CHANNELS_HEIGHT * och);

            if (!wrapInRange && INPUT_MEM_STRIDE == NB_CHANNELS) {
                macsOnRange( inputs + iOffset,  weights + wOffset, &weightedSum, NB_CHANNELS * CHANNELS_WIDTH);
            }
            else {
                for (int ix = 0; ix < CHANNELS_WIDTH; ++ix) {
                    int iOffsetInRange = iOffset + ix * INPUT_MEM_STRIDE;

                    if (wrapInRange && iOffsetInRange >= INPUT_MEM_CONT_SIZE)
                    {
                        iOffsetInRange += INPUT_MEM_WRAP_OFFSET - INPUT_MEM_CONT_OFFSET - INPUT_MEM_CONT_SIZE;
                    }

                    macsOnRange( inputs + iOffsetInRange,  weights + wOffset + ix * NB_CHANNELS, &weightedSum, NB_CHANNELS);
                }
            }
        }

        outputs[och] = sat(weightedSum, och, ACTIVATION, rescaling);
    }
}

static void fccellPropagateDATA_T(
    const UDATA_T* __restrict inputs,
    DATA_T* __restrict outputs,
    const BDATA_T* __restrict biasses,
    const WDATA_T* __restrict weights,
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
        SUM_T weightedSum = biasses[och];

        for (int iy = 0; iy < CHANNELS_HEIGHT; ++iy) {
            const int iPos = (CHANNELS_WIDTH * iy);
            int iOffset = INPUT_MEM_STRIDE * iPos;

            // Wrapping cannot occur in the middle of a line, except if
            // there is only one line (1D)!
            bool wrapInRange = false;

            if (INPUT_MEM_WRAP_SIZE > 0 && iOffset >= INPUT_MEM_CONT_SIZE) {
                iOffset += INPUT_MEM_WRAP_OFFSET - INPUT_MEM_CONT_OFFSET - INPUT_MEM_CONT_SIZE;
            }
            else if (INPUT_MEM_WRAP_SIZE > 0 && CHANNELS_WIDTH > 1 && CHANNELS_HEIGHT == 1  && iOffset + CHANNELS_WIDTH * NB_CHANNELS > INPUT_MEM_CONT_SIZE)
            {
                wrapInRange = true;
            }

            const int wOffset = NB_CHANNELS * CHANNELS_WIDTH * (iy + CHANNELS_HEIGHT * och);

            if (!wrapInRange && INPUT_MEM_STRIDE == NB_CHANNELS) {
                macsOnRange( inputs + iOffset, weights + wOffset, &weightedSum, NB_CHANNELS * CHANNELS_WIDTH);
            }
            else {
                for (int ix = 0; ix < CHANNELS_WIDTH; ++ix) {
                    int iOffsetInRange = iOffset + ix * INPUT_MEM_STRIDE;

                    if (wrapInRange && iOffsetInRange >= INPUT_MEM_CONT_SIZE)
                    {
                        iOffsetInRange += INPUT_MEM_WRAP_OFFSET - INPUT_MEM_CONT_OFFSET - INPUT_MEM_CONT_SIZE;
                    }

                    macsOnRange(inputs + iOffsetInRange, weights + wOffset + ix * NB_CHANNELS, &weightedSum, NB_CHANNELS);
                }
            }
        }

        outputs[och] = sat(weightedSum, och, ACTIVATION, rescaling);
    }
}

static void maxPropagate1(
    const DATA_T* __restrict inputs,
    int32_t* __restrict outputs,
    DATA_T* output_value,
    int NB_CHANNELS,
    int INPUTS_HEIGHT, int INPUTS_WIDTH,
    // Memory mapping: outputs
    int INPUT_MEM_CONT_OFFSET,
    int INPUT_MEM_CONT_SIZE,
    int INPUT_MEM_WRAP_OFFSET,
    int INPUT_MEM_WRAP_SIZE,
    int INPUT_MEM_STRIDE)
{
    int iMaxInput = 0;
    DATA_T maxInput = SCHAR_MIN;

    for (int iy = 0; iy < INPUTS_HEIGHT; ++iy) {
        for (int ix = 0; ix < INPUTS_WIDTH; ++ix) {
            const int oPos = (ix + INPUTS_WIDTH * iy);
            int iOffset = INPUT_MEM_STRIDE * oPos;

            if (INPUT_MEM_WRAP_SIZE > 0 && iOffset >= INPUT_MEM_CONT_SIZE) {
                iOffset += INPUT_MEM_WRAP_OFFSET - INPUT_MEM_CONT_OFFSET - INPUT_MEM_CONT_SIZE;
            }

            if (NB_CHANNELS > 1) {
                for (int ch = 0; ch < NB_CHANNELS; ++ch) {
                    if (inputs[iOffset + ch] > maxInput) {
                        iMaxInput = ch;
                        maxInput = inputs[iOffset + ch];
                    }
                }

                outputs[oPos] = (int32_t)(iMaxInput);
		        *output_value = maxInput;
            }
            else {
                outputs[oPos] = (inputs[iOffset] > 0);
		        output_value = inputs[iOffset];
            }
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

void propagate(const UDATA_T* inputs, Target_T* outputs, UDATA_T* maxPropagate_val)
{
#ifdef SAVE_OUTPUTS
    FILE* env_stream = fopen("env_output.txt", "w");
    saveOutputs(ENV_NB_OUTPUTS, ENV_SIZE_Y, ENV_SIZE_X, ENV_MEM_CONT_OFFSET, ENV_MEM_CONT_SIZE, ENV_MEM_WRAP_OFFSET, 
    ENV_MEM_WRAP_SIZE, ENV_MEM_STRIDE, inputs, env_stream, Network::Format::CHW);
    fclose(env_stream);
#endif
    // conv1
    UDATA_T* conv1_output = (UDATA_T*) mem + CONV1_MEM_CONT_OFFSET;

#ifdef BENCHMARK
    const Tick_T start_conv1 = tick();
#endif
    printf("BEFORE OF FIRST CONVOLUTION!\n");
    printf("Inputs;         %10lu\n",inputs);
    printf("conv1_output;   %10lu\n",conv1_output);
    printf("conv1_biases;   %10lu\n",conv1_biases);
    printf("conv1_weights;  %10lu\n",conv1_weights);



     printf("\n\n\n----------------------------------------------------------------\n");
    
    //convcellPropagate1(inputs , conv1_output, conv1_biases, conv1_weights, 8, CONV1_ACTIVATION);
    convcellPropagate1(inputs , conv1_output, conv1_biases, conv1_weights, 8,
    CONV1_NB_CHANNELS, CONV1_CHANNELS_HEIGHT, CONV1_CHANNELS_WIDTH, CONV1_NB_OUTPUTS, CONV1_OUTPUTS_HEIGHT, 
    CONV1_OUTPUTS_WIDTH, CONV1_PADDING_Y, CONV1_PADDING_X, CONV1_STRIDE_Y, CONV1_STRIDE_X, CONV1_KERNEL_HEIGHT, 
    CONV1_KERNEL_WIDTH, CONV1_ACTIVATION, ENV_MEM_CONT_OFFSET, ENV_MEM_CONT_SIZE, ENV_MEM_WRAP_OFFSET, 
    ENV_MEM_WRAP_SIZE, ENV_MEM_STRIDE, CONV1_MEM_CONT_OFFSET, CONV1_MEM_CONT_SIZE, CONV1_MEM_WRAP_OFFSET, CONV1_MEM_WRAP_SIZE, CONV1_MEM_STRIDE);


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

    //convcellPropagate1(inputs , conv1_output, conv1_biases, conv1_weights, CONV1_SCALING);

#ifdef BENCHMARK
    const Tick_T end_conv1 = tick();
    static RunningMean_T conv1_timing = {0.0, 0};
    benchmark("conv1", start_conv1, end_conv1, conv1_timing);
#endif

#ifdef SAVE_OUTPUTS
    FILE* conv1_stream = fopen("conv1_output.txt", "w");
    saveOutputs(CONV1_NB_OUTPUTS, CONV1_OUTPUTS_HEIGHT, CONV1_OUTPUTS_WIDTH, CONV1_MEM_CONT_OFFSET, CONV1_MEM_CONT_SIZE, 
    CONV1_MEM_WRAP_OFFSET, CONV1_MEM_WRAP_SIZE, CONV1_MEM_STRIDE, conv1_output , conv1_stream, Network::Format::CHW);
    fclose(conv1_stream);
#endif




    // conv2
    UDATA_T* conv2_output = (UDATA_T*) mem + CONV2_MEM_CONT_OFFSET;

#ifdef BENCHMARK
    const Tick_T start_conv2 = tick();
#endif

    convcellPropagate2(conv1_output , conv2_output, conv2_biases, conv2_weights, 8, CONV2_ACTIVATION);
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
    //convcellPropagate2(conv1_output , conv2_output, conv2_biases, conv2_weights, CONV2_SCALING);

#ifdef BENCHMARK
    const Tick_T end_conv2 = tick();
    static RunningMean_T conv2_timing = {0.0, 0};
    benchmark("conv2", start_conv2, end_conv2, conv2_timing);
#endif

#ifdef SAVE_OUTPUTS
    FILE* conv2_stream = fopen("conv2_output.txt", "w");
    saveOutputs(CONV2_NB_OUTPUTS, CONV2_OUTPUTS_HEIGHT, CONV2_OUTPUTS_WIDTH, CONV2_MEM_CONT_OFFSET, CONV2_MEM_CONT_SIZE, 
    CONV2_MEM_WRAP_OFFSET, CONV2_MEM_WRAP_SIZE, CONV2_MEM_STRIDE, conv2_output , conv2_stream, Network::Format::CHW);
    fclose(conv2_stream);
#endif




    // fc1
    UDATA_T* fc1_output = (UDATA_T*) mem + FC1_MEM_CONT_OFFSET;

#ifdef BENCHMARK
    const Tick_T start_fc1 = tick();
#endif

    fccellPropagateUDATA_T(conv2_output , fc1_output, fc1_biases, fc1_weights, 8,
    FC1_NB_CHANNELS, FC1_CHANNELS_HEIGHT, 
    FC1_CHANNELS_WIDTH, FC1_NB_OUTPUTS, 
    FC1_OUTPUTS_HEIGHT, FC1_OUTPUTS_WIDTH, FC1_ACTIVATION, 
    CONV2_MEM_CONT_OFFSET, CONV2_MEM_CONT_SIZE, 
    CONV2_MEM_WRAP_OFFSET, CONV2_MEM_WRAP_SIZE, 
    CONV2_MEM_STRIDE, FC1_MEM_CONT_OFFSET, 
    FC1_MEM_CONT_SIZE, FC1_MEM_WRAP_OFFSET, FC1_MEM_WRAP_SIZE, FC1_MEM_STRIDE);

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

#ifdef BENCHMARK
    const Tick_T end_fc1 = tick();
    static RunningMean_T fc1_timing = {0.0, 0};
    benchmark("fc1", start_fc1, end_fc1, fc1_timing);
#endif

#ifdef SAVE_OUTPUTS
    FILE* fc1_stream = fopen("fc1_output.txt", "w");
    saveOutputs(FC1_NB_OUTPUTS, FC1_OUTPUTS_HEIGHT, FC1_OUTPUTS_WIDTH, FC1_MEM_CONT_OFFSET, FC1_MEM_CONT_SIZE, 
    FC1_MEM_WRAP_OFFSET, FC1_MEM_WRAP_SIZE, FC1_MEM_STRIDE, fc1_output , fc1_stream, Network::Format::CHW);
    fclose(fc1_stream);
#endif




    // fc2
    DATA_T* fc2_output = (DATA_T*) mem + FC2_MEM_CONT_OFFSET;

#ifdef BENCHMARK
    const Tick_T start_fc2 = tick();
#endif

    fccellPropagateDATA_T(fc1_output , fc2_output, fc2_biases, fc2_weights, 11,
    FC2_NB_CHANNELS, FC2_CHANNELS_HEIGHT, 
    FC2_CHANNELS_WIDTH, FC2_NB_OUTPUTS, 
    FC2_OUTPUTS_HEIGHT, FC2_OUTPUTS_WIDTH, 
    FC2_ACTIVATION, FC1_MEM_CONT_OFFSET, 
    FC1_MEM_CONT_SIZE, FC1_MEM_WRAP_OFFSET, 
    FC1_MEM_WRAP_SIZE, FC1_MEM_STRIDE, 
    FC2_MEM_CONT_OFFSET, FC2_MEM_CONT_SIZE, 
    FC2_MEM_WRAP_OFFSET, FC2_MEM_WRAP_SIZE, FC2_MEM_STRIDE);
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

#ifdef BENCHMARK
    const Tick_T end_fc2 = tick();
    static RunningMean_T fc2_timing = {0.0, 0};
    benchmark("fc2", start_fc2, end_fc2, fc2_timing);
#endif

#ifdef SAVE_OUTPUTS
    FILE* fc2_stream = fopen("fc2_output.txt", "w");
    saveOutputs(FC2_NB_OUTPUTS, FC2_OUTPUTS_HEIGHT, FC2_OUTPUTS_WIDTH, FC2_MEM_CONT_OFFSET, FC2_MEM_CONT_SIZE, 
    FC2_MEM_WRAP_OFFSET, FC2_MEM_WRAP_SIZE, FC2_MEM_STRIDE, fc2_output , fc2_stream, Network::Format::CHW);
    fclose(fc2_stream);
#endif

    maxPropagate1(fc2_output, outputs, maxPropagate_val, FC2_NB_OUTPUTS, FC2_OUTPUTS_HEIGHT, FC2_OUTPUTS_WIDTH, 
    FC2_MEM_CONT_OFFSET, FC2_MEM_CONT_SIZE, FC2_MEM_WRAP_OFFSET, FC2_MEM_WRAP_SIZE, FC2_MEM_STRIDE);
    
    #ifdef PRINT_OUT
        size_t crc4 =0x000000,crc5 = 0x000000;
        crc32(&crc4, fc2_output, 2160 + FC2_MEM_CONT_OFFSET);
        crc32(&crc4, outputs, 1);
        crc32(&crc4, maxPropagate_val, sizeof(maxPropagate_val));
        printf("After ARGMAX 2:\n");
        printf("fc2_output;  %10lu\n",fc2_output);
        printf("outputs;     %10lu\n",outputs);
        printf("credence;    %10lu\n",maxPropagate_val);
        #ifdef PRINT_EACH_CRC
            printf("crc;            %x\n",crc4);
        #endif
        #ifdef PRINT_LAST_CRC 
            crc5 = crc4 + crc3 + crc2 + crc1 + crc0;
            printf("crc_block %x\n",crc5);
        #endif
    #endif
#ifdef SAVE_OUTPUTS
    FILE* max_stream = fopen("max_output.txt", "w");
    saveOutputs(FC2_NB_OUTPUTS, FC2_OUTPUTS_HEIGHT, FC2_OUTPUTS_WIDTH, FC2_MEM_CONT_OFFSET, FC2_MEM_CONT_SIZE, 
    FC2_MEM_WRAP_OFFSET, FC2_MEM_WRAP_SIZE, FC2_MEM_STRIDE, outputs, max_stream, Network::Format::CHW);
    fclose(max_stream);
#endif

}

/*template<>
float Network::backpropagate(const DATA_T* input, const std::int32_t* labels){
   const float loss = 0.0f;
   return loss;
 }

int Network::gradientCheck(){
   return(0);
}*/

