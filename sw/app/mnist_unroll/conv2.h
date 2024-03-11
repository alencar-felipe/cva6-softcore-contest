// N2D2 auto-generated file.
// @ Fri Sep 10 16:23:21 2021

#ifndef N2D2_EXPORTC_CONV2_LAYER_H
#define N2D2_EXPORTC_CONV2_LAYER_H

#include "typedefs.h"
#include "utils.h"
#define CONV2_NB_OUTPUTS 24
#define CONV2_NB_CHANNELS 16
#define CONV2_OUTPUTS_WIDTH 4
#define CONV2_OUTPUTS_HEIGHT 4
#define CONV2_OX_SIZE 4
#define CONV2_OY_SIZE 4
#define CONV2_CHANNELS_WIDTH 11
#define CONV2_CHANNELS_HEIGHT 11
#define CONV2_KERNEL_WIDTH 5
#define CONV2_KERNEL_HEIGHT 5
#define CONV2_SUB_SAMPLE_X 1
#define CONV2_SUB_SAMPLE_Y 1
#define CONV2_STRIDE_X 2
#define CONV2_STRIDE_Y 2
#define CONV2_PADDING_X 0
#define CONV2_PADDING_Y 0
#define CONV2_NO_BIAS 1

#define CONV2_ACTIVATION Rectifier
#define CONV2_SHIFT 8
static const int32_t CONV2_SCALING_FACTOR_PER_OUTPUT[] = {0};

#define CONV2_OUTPUTS_HEIGHT_NOPAD (CONV2_CHANNELS_HEIGHT - CONV2_KERNEL_HEIGHT + CONV2_STRIDE_Y) / CONV2_STRIDE_Y
#define CONV2_OUTPUTS_WIDTH_NOPAD (CONV2_CHANNELS_WIDTH - CONV2_KERNEL_WIDTH + CONV2_STRIDE_X) / CONV2_STRIDE_X

#define CONV2_OUTPUTS_SIZE (CONV2_NB_OUTPUTS*CONV2_OUTPUTS_WIDTH*CONV2_OUTPUTS_HEIGHT)
#define CONV2_CHANNELS_SIZE (CONV2_NB_CHANNELS*CONV2_CHANNELS_WIDTH*CONV2_CHANNELS_HEIGHT)

static const BDATA_T conv2_biases[CONV2_NB_OUTPUTS] = {0 + 128, 0 + 128, 0 + 128, 0 + 128, 0 + 128, 0 + 128, 0 + 128, 0 + 128, 0 + 128, 0 + 128, 0 + 128, 0 + 128, 0 + 128, 0 + 128, 0 + 128, 0 + 128, 0 + 128, 0 + 128, 0 + 128, 0 + 128, 0 + 128, 0 + 128, 0 + 128, 0 + 128, };

#define CONV2_WEIGHTS_SIZE (CONV2_NB_OUTPUTS*CONV2_KERNEL_WIDTH*CONV2_KERNEL_HEIGHT*CONV2_NB_CHANNELS)

// Flatten weights with the order [NB_OUTPUTS][KERNEL_HEIGHT][KERNEL_WIDTH][NB_CHANNELS]
static const WDATA_T conv2_weights[CONV2_WEIGHTS_SIZE] = {5, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, -4, -6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
2, -16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, -33, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, -3, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
-1, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 15, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 10, -14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
-2, -17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 3, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, -6, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
2, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 7, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
-4, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 31, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 4, 51, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 37, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, -6, -19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
-3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 26, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 11, 27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
14, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, -23, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, -25, -51, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, -24, -32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -23, -42, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 11, -26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, -5, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 43, 20, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 41, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 56, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -6, -5, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 27, -2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 41, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 30, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, -2, 37, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, -39, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 26, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, -10, -5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 6, -14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -4, -11, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, -2, -7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, -45, -49, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -26, -28, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 18, -3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, -4, -3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -22, 1, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 9, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, -15, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -40, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, -22, 27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, -4, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 14, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, -3, -21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -22, 11, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -27, 37, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 7, -10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -7, 6, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, -26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, -19, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -31, 48, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 63, -7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 36, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 1, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -6, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, -17, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 75, 2, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 39, -18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 15, -8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, -8, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -10, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, -28, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -20, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -21, -2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, -11, -19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -3, -11, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -19, -25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 34, 25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -5, -5, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -15, -24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, -6, -27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, -10, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 15, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -4, -22, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 13, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, -15, -14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 3, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 12, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 39, 31, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -10, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, -19, -19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -14, -13, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -13, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 6, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, -6, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, -16, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -18, -9, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -6, -4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 30, -6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 4, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -7, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, -18, -18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -16, 1, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 35, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 9, -5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -17, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 4, -13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -8, 6, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 2, -2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 14, -6, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -11, -7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, -3, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -4, 10, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -2, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 26, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 9, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 29, -17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 2, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 23, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 48, -14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28, -13, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 34, -2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 37, -8, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 50, -13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 34, -4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 37, -19, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 47, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 30, -6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 36, 3, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 41, -4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 9, -13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 27, -3, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 56, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 42, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, -11, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, -3, -12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -9, -11, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -16, -11, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, -12, -14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -6, -10, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -12, -15, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 3, -18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 6, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 1, -3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, -11, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 14, -8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, -14, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -5, -19, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, -5, -10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, -1, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 17, 8, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, -2, -5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -2, 16, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -9, -23, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 24, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 10, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 7, -7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 
-6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 14, 9, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, -7, 27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -2, 
16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -54, -25, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 5, -14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 35, 
-4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 15, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 20, 44, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -45, 
12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -34, -19, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, -4, -11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 
3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -14, 36, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, -44, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -22, 
-14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -47, -45, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, -6, -8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -10, 
33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -35, 5, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, -10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -56, 
-32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -43, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, -10, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -22, 
12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -6, -10, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
-6, -3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 14, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, -7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -5, -23, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, -1, -10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 8, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, -7, -7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
-8, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -15, -4, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 9, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
1, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -4, -3, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, -8, -10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
-15, -13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, -10, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, -8, -9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
-1, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -14, -12, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 13, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
-15, -6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -16, -5, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, -12, -2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, -8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -12, 13, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 23, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 14, -6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -7, 21, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, -63, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 9, -17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 79, -30, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 71, -29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, -24, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, -23, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 57, -37, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 74, -56, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 26, -34, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 19, -25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 37, -22, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 3, -18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, -68, -12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, -17, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 1, -19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, -17, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -36, 21, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, -36, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, -7, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -16, 3, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 18, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 45, 25, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -3, -8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, -1, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, -13, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 31, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -17, -5, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -16, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 14, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 35, 30, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 26, 25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, -5, -2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 11, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -3, 34, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, -6, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 1, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -7, 21, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -8, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, -5, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, -6, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 7, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, -9, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 14, -23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, -12, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 2, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, -27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, -10, -28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -8, -25, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, -32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 7, -37, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -15, 47, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, -9, 37, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 18, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -22, 58, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, -12, 68, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -26, 41, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -21, -23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, -13, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -8, -22, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -2, -52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, -17, -47, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -29, -38, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, -19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, -5, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -22, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, -43, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 24, -49, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -4, -26, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, -3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 12, 61, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -34, 84, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -33, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, -17, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, -22, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 51, -64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 52, -16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 29, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -24, 25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, -44, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -34, -4, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, -9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 5, -8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 21, -1, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -13, -5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, -8, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -14, -31, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -8, -18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, -4, -5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 6, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -26, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, -11, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -3, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -15, -6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, -15, -6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -24, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -8, -18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 21, -12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -14, -9, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, -19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 13, -17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, -9, -17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -18, -7, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, -15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, -10, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20, 18, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 65, -16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 28, 26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 27, 4, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 9, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 41, -16, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 48, 19, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 36, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 11, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 6, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 40, -3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 3, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 29, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -3, 12, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 8, -4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 12, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 21, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 13, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -12, 16, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -19, -20, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, -3, -21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -15, -11, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -6, -11, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -10, -11, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -12, -11, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, -9, -17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -12, -20, 
1, 11, 7, -25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 45, 21, -35, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 5, 58, 15, -21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
10, 50, 33, -13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 23, 25, -20, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, -3, -1, -3, -26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 15, 30, -35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 18, 51, -22, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, -2, 4, 26, -13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
9, -1, 35, -26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -4, -19, -15, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -31, -12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
2, -11, -17, -9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 3, 21, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 6, -4, 27, -11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
-2, -9, 25, -9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -12, -20, -18, -4, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, -8, -17, -17, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
-11, -21, -16, 25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -10, -19, 19, 23, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 4, -2, 14, -4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
16, -2, -10, -17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 1, -12, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 1, -14, -24, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
8, -20, -26, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -28, 2, -16, -29, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -26, -8, -22, -25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, -25, 25, 2, -24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -56, -1, 2, -32, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -30, 3, -13, -21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, -23, 22, -1, -36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -10, 15, 8, -21, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -23, 22, 3, -30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, -69, -6, -28, -24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -21, 13, -4, -27, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -7, 42, 24, -19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, -13, 27, 24, -31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -46, -7, -11, -39, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -26, 2, -22, -22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 6, 14, 12, -6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -18, 40, 28, -29, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -17, 33, 22, -36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, -6, -24, -9, -17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 42, 1, -18, -6, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 34, -12, -12, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, -33, 17, 28, -10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -19, 22, 25, 4, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, -4, 8, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 30, -16, -8, -2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25, -5, -2, -6, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -5, -20, 0, -11, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, -6, 46, -16, -29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -35, 27, 5, 20, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 43, -14, 20, 56, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 1, -11, -20, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, -15, 31, 53, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, -19, -16, 20, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, -15, -23, 24, -59, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, -35, 26, 3, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -16, -24, 8, 2, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 29, -18, 89, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 41, -38, 13, 12, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -24, -33, 24, -42, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 24, -45, 51, -16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -8, -46, 47, -28, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, -14, 50, 10, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 60, -27, 32, -11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -15, -34, -21, -15, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, -25, 42, -27, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 5, -3, -8, -28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -8, -12, 32, -4, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28, -23, -6, -16, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, -17, -10, -9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, -11, 34, -40, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -12, -22, 11, -12, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, -6, 15, 6, -22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -21, -9, 
-21, -23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -6, -28, -34, -15, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, -8, -7, -7, -50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, -5, 
15, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 23, 50, -29, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, -12, -13, 16, -8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, -32, 
-47, -32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 27, 35, -54, -49, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 40, -15, 20, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 8, 
44, 46, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, -45, 35, 39, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 36, -7, -12, -60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 49, -6, 
-57, -4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 70, -18, 37, 26, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 27, -7, 2, 35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -4, -2, 
11, 33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20, -3, -46, -86, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 44, -9, -43, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, -36, 
44, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 27, -11, 11, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 16, 26, 2, -42, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -11, 
-30, -33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, -49, -1, 22, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 4, -30, 21, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
-26, -50, 18, 25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -56, -47, 11, -11, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, -52, -24, 5, -7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
-22, 8, 1, -13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 23, 5, -3, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, -7, -52, -3, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
-23, -49, 7, 26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -44, -62, 38, 27, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, -58, -31, 51, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
-34, 6, 55, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, -2, 12, 20, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 8, -63, 9, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
-1, -90, -3, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -33, -59, 11, 43, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, -53, -23, 20, 25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
11, 21, 29, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 7, 17, 25, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 37, 38, -1, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
25, 38, 3, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, -18, -4, 21, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 42, 34, -19, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
38, 67, -14, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 45, 50, -8, 2, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 27, 33, 34, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
17, -20, 24, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 29, -11, 33, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 23, -8, 33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 26, -8, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -14, 18, -18, 15, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 10, -17, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 4, 15, -23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 53, -5, 32, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 14, 20, -30, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 2, 28, -25, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 7, -14, -22, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -11, 14, -30, -21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, -8, 18, -12, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -16, 18, -51, 19, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -17, 28, -27, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, -6, 6, -24, -19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -18, 14, 4, 16, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -10, -9, 21, -20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, -18, -8, 22, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -23, -3, 7, -18, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -15, -11, -14, -31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 16, -8, 60, -16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -2, -38, 47, -15, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, -16, 12, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, -3, -1, -4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -4, -3, 26, -5, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -15, -14, -1, -13, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, -9, -12, -30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -10, -20, -28, -10, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -2, -53, -25, -11, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 8, -44, -6, -18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, -23, -19, -7, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -10, 20, 6, -15, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, -26, 19, -12, -16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -24, 23, -12, -25, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -18, 13, 3, -19, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 10, -27, 12, -9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 14, 43, 36, 6, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -2, 73, 44, -1, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 10, 76, 17, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -21, 19, -18, -14, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, -24, 11, -3, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 22, -52, 27, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 21, -30, 45, -1, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 16, 62, 22, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, -12, 32, 1, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -18, 13, -8, 4, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -7, 3, 6, -3, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, -18, -34, 20, -11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, -76, 17, -12, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -12, -27, 10, -3, -1, 17, 11, 5, 13, -9, -3, 1, 
3, 8, -3, -12, -6, 15, -6, -5, -1, 8, 3, 2, -11, 4, 24, 6, -2, -17, -18, 15, -5, 23, -5, 10, 
4, 3, 9, 19, 8, 13, 19, 8, -1, 8, -11, -16, -19, 0, 2, 17, 9, 19, -1, -1, 11, -4, 10, 11, 
-10, 29, -8, -1, -19, -13, -16, 7, 10, 58, 34, 2, 3, 19, 19, 15, 11, 16, -3, -20, 28, 9, 4, -4, 
7, 9, 11, -11, -4, 16, 0, -16, 15, -11, 5, -5, 32, -1, 3, -25, 8, -1, -2, 4, -3, 11, -3, -4, 
7, -9, 11, 16, 15, 15, 20, -10, 17, 18, -22, 16, 10, 0, 9, 7, 12, 9, 7, 3, 6, 36, 8, 9, 
11, 51, 6, 22, 19, -14, 22, -6, 2, 31, 6, -20, 28, 16, 15, 20, 7, 53, 30, 3, 4, -12, 9, -14, 
17, 20, -4, -12, 42, -14, 12, -4, 7, -13, 5, -40, -32, 16, -41, -2, -4, -29, 24, 1, 22, -7, 18, -9, 
8, 8, 23, -28, -29, 28, -29, -23, 1, -20, 27, -1, 19, 46, 27, 0, 13, 12, 49, -25, -25, 15, -29, -18, 
-9, 3, 21, -11, 20, 4, 16, 8, 16, 39, 23, -32, -10, 0, -44, -3, -14, 14, -5, -21, 12, -25, -4, -1, 
-4, 36, 13, -24, -12, -13, -12, -10, -13, 10, -14, -37, 15, -61, -4, -13, 1, 15, 20, -52, -31, 33, -45, -29, 
-11, -41, 33, -5, -17, 19, 32, 0, 6, 27, 33, -32, -29, 30, -30, -19, -19, -33, 29, 19, 15, 29, 25, 16, 
5, 40, 54, -40, -27, 4, -21, 0, -33, -40, 15, -2, 18, 3, 5, 12, 3, 50, 73, -42, -30, 38, -17, -3, 
-39, -21, -2, -34, 2, -39, -16, -3, 6, 35, 43, -25, -26, 15, -9, 0, -22, -18, -20, -33, -11, -35, -29, -11, 
-6, 39, 30, -32, -33, 11, -12, -17, -11, -5, 12, 1, 20, -13, -2, 9, -1, 12, 27, 4, -6, 12, 6, -6, 
-17, -10, -3, 6, -30, 5, -5, 9, 7, 9, 25, 10, -3, 7, -1, -18, -36, -13, -15, -4, -26, 14, -18, 3, 
10, 11, 24, 3, -10, 2, 3, -25, -21, 4, -8, -15, 0, -11, -5, 1, 1, 31, 66, 4, -9, 4, -1, -21, 
-3, 6, -2, -9, 33, 34, -5, -17, 0, 20, 0, 33, 23, -22, 26, -9, 77, 36, -12, -18, 19, 9, -6, 26, 
6, -14, -6, 16, -1, -15, 9, -41, 27, 16, -1, -21, 6, -10, 6, -7, -8, -29, 1, 10, -4, -14, 2, -21, 
6, 6, -2, -1, -13, -22, 0, 10, -4, -6, -9, -8, -7, -14, 7, -8, -4, -34, -7, 1, -18, -1, 2, -1, 
-5, 8, 2, 8, 3, -7, 8, -7, -19, -8, -8, 10, -20, -6, -1, 5, 15, -12, -6, 20, 4, -21, 3, -8, 
58, 44, -7, 13, 20, 6, -2, 30, -3, -9, -23, 8, -13, 1, 5, -23, 3, 13, 5, -10, 9, -2, -2, -15, 
-4, -2, -22, 13, -4, -1, 7, 6, -13, -30, -4, -3, 0, -24, -5, 0, -1, 4, -22, -5, 5, -15, 3, 12, 
-23, 1, -3, -22, -18, -29, -4, -10, -7, 16, -4, 2, 0, -16, 15, 0, -4, 6, -18, -6, -2, -35, -11, -9, 
1, -10, -21, 16, 4, -29, 1, -9, 29, 25, -9, -9, 9, -24, -9, 21, -15, 1, -29, 2, -14, -9, 2, -13, 
-14, -27, -15, -11, -26, -30, -19, -21, -5, 9, 0, 8, 4, -2, 11, 15, -26, -14, -12, -9, -25, -35, -10, -7, 
5, 28, 29, -6, -6, -3, 2, 1, -8, 14, -17, -13, -13, -15, -11, -9, 6, 17, 24, 2, 9, -8, 10, 3, 
-6, 24, -17, 0, 3, -22, -12, -5, 4, -9, -37, 6, -5, -7, -8, -14, -2, -1, -13, -8, -12, -23, -8, 16, 
-10, 13, -24, 4, -15, -14, 10, -1, -30, -41, -20, -15, -36, -25, -12, -2, -3, 12, 13, 10, 6, -5, 8, 2, 
-21, 20, -16, -17, -14, -23, -15, -13, -1, 3, 18, 4, 2, 5, 4, -1, 3, 11, -17, -17, -3, -17, -18, 2, 
6, 7, 5, 8, 8, 10, 11, 17, 14, 21, -9, -6, 5, -7, -3, -10, 0, 4, -30, 0, -10, -20, 8, -19, 
-13, -30, -17, -4, -42, -8, -21, 7, -6, 6, -15, 29, 0, -2, 22, -4, -19, -20, -14, 0, -32, -3, -9, -3, 
-7, 9, 6, 18, 17, -5, 2, -8, -6, 9, -6, -11, -13, -10, -12, -7, -2, -4, -8, 3, -1, 8, 1, 15, 
9, 4, 4, -16, -6, 9, -4, -6, 2, 7, 3, 17, 15, 13, 6, 6, 15, 6, -2, -6, 5, 14, -1, -8, 

};

#endif // N2D2_EXPORTC_CONV2_LAYER_H