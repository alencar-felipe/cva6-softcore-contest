#include <stdlib.h>
#include <stdio.h>

#include "env.h"
#include "mem_info.h"

#include "conv1.h"
#include "conv2.h"
#include "fc1.h"
#include "fc2.h"


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
    int rescaling)
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
                    const int sxMax = (CONV1_PADDING_X == 0 && CONV1_OUTPUTS_WIDTH == CONV1_OUTPUTS_WIDTH_NOPAD) ? CONV1_KERNEL_WIDTH : clamp(CONV1_CHANNELS_WIDTH + CONV1_PADDING_X - (ox * CONV1_STRIDE_X), 0, CONV1_KERNEL_WIDTH);
                    const int ix = (ox * CONV1_STRIDE_X) - CONV1_PADDING_X;
                #endif

                int oOffset = CONV1_MEM_STRIDE *(ox + (CONV1_OUTPUTS_WIDTH * oy));
                
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


                    #if (CONV1_PADDING_X == 0 && CONV1_OUTPUTS_WIDTH == CONV1_OUTPUTS_WIDTH_NOPAD) /*  sxMax == KERNEL_WIDTH  &&  sxMin = 0*/ 
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
                outputs[oOffset + output] = sat(weightedSum, output, CONV1_ACTIVATION, rescaling);
            }
        }
    }
}

static void convcellPropagate2(
    const UDATA_T* __restrict inputs,
    UDATA_T* __restrict outputs,
    const BDATA_T* __restrict biasses,
    const WDATA_T* __restrict weights,
    int rescaling)
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
                    const int sxMax = (CONV2_PADDING_X == 0 && CONV2_OUTPUTS_WIDTH == CONV2_OUTPUTS_WIDTH_NOPAD) ? CONV2_KERNEL_WIDTH : clamp(CONV2_CHANNELS_WIDTH + CONV2_PADDING_X - (ox * CONV2_STRIDE_X), 0, CONV2_KERNEL_WIDTH);
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
                outputs[oOffset + output] = sat(weightedSum, output, CONV1_ACTIVATION, rescaling);
            }
        }
    }
}

static void fccellPropagateUDATA_T(
    const UDATA_T* __restrict inputs,
    UDATA_T* __restrict outputs,
    const BDATA_T* __restrict biasses,
    const WDATA_T* __restrict weights,
    const int rescaling)
{
    // static_assert(FC1_OUTPUTS_HEIGHT == 1, "Outputs height should be 1");
    // static_assert(FC1_OUTPUTS_WIDTH == 1, "Outputs width should be 1");
    // static_assert(FC1_MEM_WRAP_SIZE == 0, "Output wrapping not supported");
    
    for (int och = 0; och < FC1_NB_OUTPUTS; och++) {
        SUM_T weightedSum = biasses[och];

        for (int iy = 0; iy < FC1_CHANNELS_HEIGHT; ++iy) {
            const int iPos = (FC1_CHANNELS_WIDTH * iy);
            int iOffset = CONV2_MEM_STRIDE * iPos;

            // Wrapping cannot occur in the middle of a line, except if
            // there is only one line (1D)!
            bool wrapInRange = false;

            if (CONV2_MEM_WRAP_SIZE > 0 && iOffset >= CONV2_MEM_CONT_SIZE) {
                iOffset += CONV2_MEM_WRAP_OFFSET - CONV2_MEM_CONT_OFFSET - CONV2_MEM_CONT_SIZE;
            }
            else if (CONV2_MEM_WRAP_SIZE > 0 && FC1_CHANNELS_WIDTH > 1 && FC1_CHANNELS_HEIGHT == 1 && iOffset + FC1_CHANNELS_WIDTH * FC1_NB_CHANNELS > CONV2_MEM_CONT_SIZE)
            {
                wrapInRange = true;
            }

            #if (CONV2_MEM_WRAP_SIZE > 0)
                if(iOffset >= CONV2_MEM_CONT_SIZE){
                    iOffset += CONV2_MEM_WRAP_OFFSET - CONV2_MEM_CONT_OFFSET - CONV2_MEM_CONT_SIZE;
                }
                #if (FC1_CHANNELS_WIDTH > 1 && FC1_CHANNELS_HEIGHT == 1)
                    else if (iOffset + FC1_CHANNELS_WIDTH * FC1_NB_CHANNELS > CONV2_MEM_CONT_SIZE){
                        wrapInRange = true;
                    }
                #endif
            #endif

            const int wOffset = FC1_NB_CHANNELS * FC1_CHANNELS_WIDTH * (iy + FC1_CHANNELS_HEIGHT * och);

            #if CONV2_MEM_STRIDE == FC1_NB_CHANNELS
                if(!wrapInRange){
                    macsOnRange( inputs + iOffset,  weights + wOffset, &weightedSum, FC1_NB_CHANNELS * FC1_CHANNELS_WIDTH);
                }
            #else
                for (int ix = 0; ix < FC1_CHANNELS_WIDTH; ++ix) {
                    int iOffsetInRange = iOffset + ix * CONV2_MEM_STRIDE;

                    if (wrapInRange && iOffsetInRange >= CONV2_MEM_CONT_SIZE)
                    {
                        iOffsetInRange += CONV2_MEM_WRAP_OFFSET - CONV2_MEM_CONT_OFFSET - CONV2_MEM_CONT_SIZE;
                    }

                    macsOnRange( inputs + iOffsetInRange,  weights + wOffset + ix * FC1_NB_CHANNELS, &weightedSum, FC1_NB_CHANNELS);
                }
            #endif
        }
        outputs[och] = sat(weightedSum, och, FC1_ACTIVATION, rescaling);
    }
}

static void fccellPropagateDATA_T(
    const UDATA_T* __restrict inputs,
    DATA_T* __restrict outputs,
    const BDATA_T* __restrict biasses,
    const WDATA_T* __restrict weights,
    const int rescaling)
{
    // static_assert(FC2_OUTPUTS_HEIGHT == 1, "Outputs height should be 1");
    // static_assert(FC2_OUTPUTS_WIDTH == 1, "Outputs width should be 1");
    // static_assert(FC2_MEM_WRAP_SIZE == 0, "Output wrapping not supported");

    for (int och = 0; och < FC2_NB_OUTPUTS; och++) {
        SUM_T weightedSum = biasses[och];

        for (int iy = 0; iy < FC2_CHANNELS_HEIGHT; ++iy) {
            const int iPos = (FC2_CHANNELS_WIDTH * iy);
            int iOffset = FC1_MEM_STRIDE * iPos;

            // Wrapping cannot occur in the middle of a line, except if
            // there is only one line (1D)!
            bool wrapInRange = false;
            #if FC1_MEM_WRAP_SIZE > 0
                if ( iOffset >= FC1_MEM_CONT_SIZE ){
                    iOffset += FC1_MEM_WRAP_OFFSET - FC1_MEM_CONT_OFFSET - FC1_MEM_CONT_SIZE;
                }
                #if(FC2_CHANNELS_WIDTH > 1 && FC2_CHANNELS_HEIGHT == 1 )
                    else if(iOffset + FC2_CHANNELS_WIDTH * FC2_NB_CHANNELS > FC1_MEM_CONT_SIZE){
                        wrapInRange = true;
                    }
                #endif  
            #endif

            const int wOffset = FC2_NB_CHANNELS * FC2_CHANNELS_WIDTH * (iy + FC2_CHANNELS_HEIGHT * och);

            #if FC1_MEM_STRIDE == FC2_NB_CHANNELS
                if(!wrapInRange){
                    macsOnRange( inputs + iOffset, weights + wOffset, &weightedSum, FC2_NB_CHANNELS * FC2_CHANNELS_WIDTH);
                }
            #else
                for (int ix = 0; ix < FC2_CHANNELS_WIDTH; ++ix) {
                    int iOffsetInRange = iOffset + ix * FC1_MEM_STRIDE;

                    if (wrapInRange && iOffsetInRange >= FC1_MEM_CONT_SIZE)
                    {
                        iOffsetInRange += FC1_MEM_WRAP_OFFSET - FC1_MEM_CONT_OFFSET - FC1_MEM_CONT_SIZE;
                    }

                    macsOnRange(inputs + iOffsetInRange, weights + wOffset + ix * FC2_NB_CHANNELS, &weightedSum, FC2_NB_CHANNELS);
                }
            #endif
        }

        outputs[och] = sat(weightedSum, och, FC1_ACTIVATION, rescaling);
    }
}

static void maxPropagate1(
    const DATA_T* __restrict inputs,
    int32_t* __restrict outputs,
    DATA_T* output_value)
{
    int iMaxInput = 0;
    DATA_T maxInput = SCHAR_MIN;

    for (int iy = 0; iy < FC2_OUTPUTS_HEIGHT; ++iy) {
        for (int ix = 0; ix < FC2_OUTPUTS_WIDTH; ++ix) {
            const int oPos = (ix + FC2_OUTPUTS_WIDTH * iy);
            int iOffset = FC2_MEM_STRIDE * oPos;
            #if FC2_MEM_WRAP_SIZE > 0
                if( iOffset >= FC2_MEM_CONT_SIZE ){
                    iOffset += FC2_MEM_WRAP_OFFSET - FC2_MEM_CONT_OFFSET - FC2_MEM_CONT_SIZE;
                }
            #endif

            #if FC2_NB_OUTPUTS > 1 
                for (int ch = 0; ch < FC2_NB_OUTPUTS; ++ch) {
                    if (inputs[iOffset + ch] > maxInput) {
                        iMaxInput = ch;
                        maxInput = inputs[iOffset + ch];
                    }
                }

                outputs[oPos] = (int32_t)(iMaxInput);
		        *output_value = maxInput;
            #else
                outputs[oPos] = (inputs[iOffset] > 0);
		        output_value = inputs[iOffset];
            #endif
        
        }
    }
}

void propagate(const UDATA_T* inputs, Target_T* outputs, UDATA_T* maxPropagate_val)
{
#ifdef SAVE_OUTPUTS
    FILE* env_stream = fopen("env_output.txt", "w");
    saveOutputs(ENV_NB_OUTPUTS, ENV_SIZE_Y, ENV_SIZE_X, ENV_MEM_CONT_OFFSET, ENV_MEM_CONT_SIZE, ENV_MEM_WRAP_OFFSET, ENV_MEM_WRAP_SIZE, ENV_MEM_STRIDE, inputs, env_stream, Network::Format::CHW);
    fclose(env_stream);
#endif
    // conv1
    UDATA_T* conv1_output = (UDATA_T*) mem + CONV1_MEM_CONT_OFFSET;

#ifdef BENCHMARK
    const Tick_T start_conv1 = tick();
#endif

    convcellPropagate1(inputs , conv1_output, conv1_biases, conv1_weights, 8);

    
#ifdef BENCHMARK
    const Tick_T end_conv1 = tick();
    static RunningMean_T conv1_timing = {0.0, 0};
    benchmark("conv1", start_conv1, end_conv1, conv1_timing);
#endif

#ifdef SAVE_OUTPUTS
    FILE* conv1_stream = fopen("conv1_output.txt", "w");
    saveOutputs(CONV1_NB_OUTPUTS, CONV1_OUTPUTS_HEIGHT, CONV1_OUTPUTS_WIDTH, CONV1_MEM_CONT_OFFSET, CONV1_MEM_CONT_SIZE, CONV1_MEM_WRAP_OFFSET, CONV1_MEM_WRAP_SIZE, CONV1_MEM_STRIDE, conv1_output , conv1_stream, Network::Format::CHW);
    fclose(conv1_stream);
#endif




    // conv2
    UDATA_T* conv2_output = (UDATA_T*) mem + CONV2_MEM_CONT_OFFSET;

#ifdef BENCHMARK
    const Tick_T start_conv2 = tick();
#endif

    convcellPropagate2(conv1_output , conv2_output, conv2_biases, conv2_weights, 8);

    //convcellPropagate2(conv1_output , conv2_output, conv2_biases, conv2_weights, CONV2_SCALING);

#ifdef BENCHMARK
    const Tick_T end_conv2 = tick();
    static RunningMean_T conv2_timing = {0.0, 0};
    benchmark("conv2", start_conv2, end_conv2, conv2_timing);
#endif

#ifdef SAVE_OUTPUTS
    FILE* conv2_stream = fopen("conv2_output.txt", "w");
    saveOutputs(CONV2_NB_OUTPUTS, CONV2_OUTPUTS_HEIGHT, CONV2_OUTPUTS_WIDTH, CONV2_MEM_CONT_OFFSET, CONV2_MEM_CONT_SIZE, CONV2_MEM_WRAP_OFFSET, CONV2_MEM_WRAP_SIZE, CONV2_MEM_STRIDE, conv2_output , conv2_stream, Network::Format::CHW);
    fclose(conv2_stream);
#endif




    // fc1
    UDATA_T* fc1_output = (UDATA_T*) mem + FC1_MEM_CONT_OFFSET;

#ifdef BENCHMARK
    const Tick_T start_fc1 = tick();
#endif

    fccellPropagateUDATA_T(conv2_output , fc1_output, fc1_biases, fc1_weights, 8);

#ifdef BENCHMARK
    const Tick_T end_fc1 = tick();
    static RunningMean_T fc1_timing = {0.0, 0};
    benchmark("fc1", start_fc1, end_fc1, fc1_timing);
#endif

#ifdef SAVE_OUTPUTS
    FILE* fc1_stream = fopen("fc1_output.txt", "w");
    saveOutputs(FC1_NB_OUTPUTS, FC1_OUTPUTS_HEIGHT, FC1_OUTPUTS_WIDTH, FC1_MEM_CONT_OFFSET, FC1_MEM_CONT_SIZE, FC1_MEM_WRAP_OFFSET, FC1_MEM_WRAP_SIZE, FC1_MEM_STRIDE, fc1_output , fc1_stream, Network::Format::CHW);
    fclose(fc1_stream);
#endif




    // fc2
    DATA_T* fc2_output = (DATA_T*) mem + FC2_MEM_CONT_OFFSET;

#ifdef BENCHMARK
    const Tick_T start_fc2 = tick();
#endif

    fccellPropagateDATA_T(fc1_output , fc2_output, fc2_biases, fc2_weights, 11);
#ifdef BENCHMARK
    const Tick_T end_fc2 = tick();
    static RunningMean_T fc2_timing = {0.0, 0};
    benchmark("fc2", start_fc2, end_fc2, fc2_timing);
#endif

#ifdef SAVE_OUTPUTS
    FILE* fc2_stream = fopen("fc2_output.txt", "w");
    saveOutputs(FC2_NB_OUTPUTS, FC2_OUTPUTS_HEIGHT, FC2_OUTPUTS_WIDTH, FC2_MEM_CONT_OFFSET, FC2_MEM_CONT_SIZE, FC2_MEM_WRAP_OFFSET, FC2_MEM_WRAP_SIZE, FC2_MEM_STRIDE, fc2_output , fc2_stream, Network::Format::CHW);
    fclose(fc2_stream);
#endif

    maxPropagate1(fc2_output, outputs, maxPropagate_val);

#ifdef SAVE_OUTPUTS
    FILE* max_stream = fopen("max_output.txt", "w");
    saveOutputs(FC2_NB_OUTPUTS, FC2_OUTPUTS_HEIGHT, FC2_OUTPUTS_WIDTH, FC2_MEM_CONT_OFFSET, FC2_MEM_CONT_SIZE, FC2_MEM_WRAP_OFFSET, FC2_MEM_WRAP_SIZE, FC2_MEM_STRIDE, outputs, max_stream, Network::Format::CHW);
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


