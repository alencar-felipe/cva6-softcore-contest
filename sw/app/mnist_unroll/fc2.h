// N2D2 auto-generated file.
// @ Fri Sep 10 16:23:21 2021

#ifndef N2D2_EXPORTC_FC2_LAYER_H
#define N2D2_EXPORTC_FC2_LAYER_H

#include "typedefs.h"
#include "utils.h"
#define FC2_NB_OUTPUTS 10
#define FC2_NB_CHANNELS 150
#define FC2_OUTPUTS_WIDTH 1
#define FC2_OUTPUTS_HEIGHT 1
#define FC2_CHANNELS_WIDTH 1
#define FC2_CHANNELS_HEIGHT 1
#define FC2_NO_BIAS 1

#define FC2_ACTIVATION Linear
#define FC2_SHIFT 11
static const int32_t FC2_SCALING_FACTOR_PER_OUTPUT[] = {0};

#define FC2_OUTPUTS_SIZE (FC2_NB_OUTPUTS*FC2_OUTPUTS_WIDTH*FC2_OUTPUTS_HEIGHT)
#define FC2_CHANNELS_SIZE (FC2_NB_CHANNELS*FC2_CHANNELS_WIDTH*FC2_CHANNELS_HEIGHT)

static const BDATA_T fc2_biases[FC2_OUTPUTS_SIZE] = {0 + 1024, 0 + 1024, 0 + 1024, 0 + 1024, 0 + 1024, 0 + 1024, 0 + 1024, 0 + 1024, 0 + 1024, 0 + 1024, };
#define FC2_WEIGHTS_SIZE (FC2_OUTPUTS_SIZE*FC2_CHANNELS_SIZE)

// Flatten weights with the order[OUTPUTS_SIZE][CHANNELS_SIZE]. 
// If the previous cell was a 2D cell, CHANNELS_SIZE is flatten in the [CHANNELS_HEIGHT][CHANNELS_WIDTH][NB_CHANNELS] order.
static const WDATA_T fc2_weights[FC2_WEIGHTS_SIZE] = {
-55, -5, -6, -28, -26, -38, 30, -29, -5, -28, -25, 44, -8, -19, -7, -23, -14, 29, -10, 28, 30, 33, -38, -39, -36, 16, -14, -19, -25, -12, 
-3, 58, 6, -5, -43, 41, 27, -4, -23, 44, 28, -33, -2, 3, 40, -23, -7, 46, 38, -21, -19, -40, -9, -17, -21, -13, 34, -7, -4, -42, 
47, 36, -10, -30, -21, -26, 28, -30, 50, 1, 20, 60, 44, 42, -10, -5, 30, -4, -8, -8, -6, -35, 40, -20, -20, -23, 18, -32, 12, -19, 
-8, 44, -24, 18, -33, 39, 37, -23, 27, -39, 2, 31, 14, -30, 49, -34, -48, -25, -20, -3, 38, -16, 39, -29, 0, -5, 7, -27, 6, 20, 
39, -13, -10, -3, -37, -41, -8, -33, -8, -3, -4, -7, 2, 33, 10, -25, 46, -13, -11, -36, -29, -13, -28, -1, 5, -10, -11, -19, -35, -26, 
14, 52, -30, 4, -21, -35, -35, -16, 40, -18, -31, 36, 39, -7, 45, -6, 32, -5, 2, -19, -27, -26, 41, 35, 58, -25, 34, -38, 6, -20, 
-10, -17, 48, 4, -6, 31, -31, -8, 4, -35, 52, 49, -22, -5, -19, -31, -15, -8, -45, -21, 37, -37, 1, 42, -26, -48, -17, -11, -21, -28, 
-16, -18, 19, 22, 9, 53, 7, 24, 29, -41, -13, -27, -15, -18, -25, -19, -17, -14, -38, 28, -6, 53, 33, -27, -37, 22, 34, -47, 11, -8, 
-12, -37, 46, -20, 37, -32, -29, 21, -24, 38, 50, 26, -24, 54, -19, -28, -14, -54, 24, -13, -27, -17, 1, -27, -13, 22, 41, 31, -21, -39, 
42, 43, 13, -20, -21, 25, -27, 47, -15, -38, -16, -25, -2, 55, -4, -21, -11, -20, 19, -15, 41, 54, 53, -17, 42, 50, -6, -7, -18, 53, 
-25, 39, 48, 50, -33, -37, 0, 39, -43, -17, 3, 22, 44, -2, -28, -27, 48, -12, 8, 39, 27, 29, -52, -51, 57, 30, -34, -24, 7, 2, 
7, 7, -27, -3, 45, 19, 20, 32, 33, -13, 44, -13, -18, -11, -23, 47, -9, -28, 7, -49, -50, -63, -1, 50, -16, 20, -9, 1, 2, -14, 
56, -28, 10, -43, 4, -3, -27, 45, 25, -42, 55, 22, -41, 33, -33, -20, 32, -11, -44, -26, 40, 49, -44, -30, 40, 47, 20, -28, -12, 29, 
49, -32, -47, 48, -47, -13, -48, -57, -28, 43, -5, 17, -7, 42, 39, -40, 27, -33, -19, -12, -27, -22, 23, -25, 42, -1, -24, 17, -6, 49, 
-23, 1, 36, -1, -8, 31, -38, -16, -4, -26, -24, 54, -39, -19, 37, 34, -52, 27, -1, 45, 35, -28, -39, 35, 5, -19, 8, -4, 12, -28, 
-7, -24, 29, -3, -39, 13, 50, -14, -31, -28, 58, -23, 47, 3, -30, -8, 46, -8, 20, 34, -40, 29, 54, 44, -44, -29, -30, -39, -8, -8, 
-27, -29, -19, -17, 55, -50, -26, 55, 40, -30, -26, -25, 35, 6, -20, -8, -28, -13, -44, 47, 40, 47, -16, -9, -25, -20, -42, 57, -5, -35, 
-28, -16, -2, 34, 1, -21, 28, -23, -9, -53, -3, -37, -5, 11, 57, 51, 16, 0, 46, -29, -35, -26, -18, -29, 20, -18, 11, -3, -1, -6, 
-21, 6, -27, -10, -37, -26, -34, 27, -42, 41, -21, 15, 43, -21, -24, 50, 39, -39, -27, -3, -18, 57, -35, 48, 36, 49, -17, -49, -13, 46, 
-19, 50, 32, 39, -37, 29, -36, 57, -21, 46, -5, 43, 23, -31, -5, 30, 11, 21, -19, 34, -27, -7, -20, -17, -28, -18, -6, -14, -20, -33, 
46, -15, -28, -10, -19, 47, -16, -25, -4, 51, -29, -10, -50, -28, 25, 27, -37, -5, -8, -34, -17, -10, -32, -3, -2, 46, 29, 0, -22, -2, 
-1, -47, 37, -10, -31, -16, 22, -55, -49, -32, 15, 45, -25, 7, 64, -14, 55, -21, -35, -23, -36, 35, -2, -4, 49, 44, -30, -27, 48, 52, 
-8, -19, 39, 33, -13, 54, -51, -27, 9, 42, -3, -31, 27, -43, -16, -27, 57, -4, -37, -10, 52, -5, -28, -25, 28, 9, -6, 41, 6, -2, 
48, 35, 3, -17, 28, 38, -18, -34, -9, -38, 29, -16, -30, -3, -22, 4, -49, 38, -15, -8, -53, 36, -22, 6, -37, -9, -21, 31, 55, -28, 
-42, 45, 17, -16, 39, -35, 41, -25, 11, -6, 50, -29, 27, 54, 26, -50, 36, 34, -3, -44, 33, -16, 37, 47, -4, 46, -5, -13, 29, 41, 
37, -11, -6, -2, 35, -25, 40, 28, 4, -3, -30, 5, -14, -14, -25, 19, -56, -18, 1, -44, 32, -52, 46, 29, -40, -8, -45, 59, -13, 25, 
53, 36, -23, 55, 21, -45, -19, 14, -47, 31, -12, -12, 3, 13, -24, -6, 1, 14, 33, 40, -32, 42, -11, -20, -1, 27, 39, 42, -12, 21, 
-9, 37, -28, -27, -19, -10, 12, 14, -37, 24, 0, -10, -42, -51, 45, 29, -48, -2, 47, -2, -31, -18, 20, 51, 29, -44, 9, -32, -7, -5, 
0, -42, 36, -15, 34, -12, 33, 34, 23, -38, -2, -28, 38, -22, -19, 46, 41, 52, 20, -3, 46, -26, -21, -22, 34, -22, 33, 16, -19, -29, 
6, -29, -33, 37, -30, 25, -5, 54, 4, 6, -11, -43, 22, -30, 20, -49, -44, -44, 5, -30, -12, 59, -32, 1, -27, -13, 5, 16, -33, -37, 
-41, -6, -41, -23, 56, -26, 20, 21, -27, -6, -29, 12, -29, -17, -25, -18, -17, 24, -19, -27, -40, -33, 24, -27, -25, -29, -4, 46, -12, -28, 
1, -15, -22, 25, 34, 39, 42, 21, -20, 40, -39, 57, -16, -3, -18, 33, -23, -7, 30, 10, -31, -40, 5, -20, -20, 55, 6, -8, 0, -34, 
-14, 46, -22, -25, -9, 1, -20, 35, 47, -17, -19, 56, 47, -38, -16, -12, -41, -16, 41, 19, -47, 47, 11, 59, -14, -18, 1, -32, 11, 0, 
-14, 44, 23, -15, 38, -46, 33, -26, -14, -33, -26, -12, -49, 35, -16, -24, -39, 34, 45, 75, 39, -20, 24, -15, -11, -16, -8, 24, 50, -27, 
13, -21, -4, -27, -32, -34, 6, -23, -8, -19, 58, -31, 48, -27, -17, -42, 54, -49, -1, -47, 37, -26, -29, -19, -24, 3, -4, -37, -10, -25, 
39, 27, 39, 54, -19, 49, -34, -30, 16, 42, -34, -36, 34, 32, -14, 68, 53, -10, -17, -25, 37, 0, -32, -32, 0, -11, -21, -50, 49, 57, 
4, 13, 3, -13, -40, -35, 21, -40, 41, -39, -19, -44, -18, 6, -11, 1, -22, 31, -22, -37, 33, -12, -8, 41, 7, -15, 43, -35, -30, 34, 
30, 34, 53, 37, 47, -5, -3, -10, -39, 46, -17, -50, -5, 16, -12, 28, -31, -8, -22, -6, -40, -19, -20, -29, -10, 31, -27, 17, 13, 49, 
-16, -46, -18, -4, -47, 41, 12, 13, -31, 46, 61, 9, 27, -13, 24, -27, 31, -49, -21, -4, -32, 49, 33, -8, -14, 49, 40, -39, -21, -29, 
38, -3, 19, -26, 35, -36, -23, -12, -4, -30, -1, 32, -13, -39, -20, 38, -25, 37, -21, 34, -26, -11, 51, -34, -14, -19, -1, 44, 52, 44, 
-57, -32, 40, -20, 50, 18, -24, 50, 33, 47, 41, -27, -42, 23, 4, 0, -38, 0, 16, 50, -47, 28, -9, 1, 18, -9, 30, 38, -15, -2, 
-9, 26, -18, -19, 25, 42, -25, 47, 36, 20, -47, -1, 53, 0, -19, 28, -39, -6, 33, 24, 15, 40, 39, -34, -8, -33, -42, -4, -30, 28, 
14, -40, -29, -46, -21, 6, 13, 4, -35, 50, -33, 48, -27, 15, -18, -8, -7, -13, 39, 23, 16, -21, 20, 43, -14, 12, -64, 32, 2, -16, 
-12, 13, 45, 41, -32, -46, 22, 24, 42, 35, -20, -41, -36, -29, 28, 5, 29, 26, 43, -6, 37, -10, -32, 35, 17, -30, -7, 27, 4, 21, 
5, -20, -23, -25, 40, 37, 62, -33, 70, 61, -15, 50, -23, 21, 11, 38, -44, -16, 16, 37, -22, -20, -43, -34, 53, -20, 6, -42, 10, -21, 
39, -18, -47, -32, 3, 28, -48, -33, 31, -14, 42, -26, -4, 49, 37, -43, -22, 9, 18, -5, 28, -6, 0, 24, 38, 19, 29, 50, 36, 1, 
-15, -3, 34, -6, -53, -35, -49, -50, -49, 34, 15, -49, 37, 6, 40, -29, 55, -9, 10, 33, 26, 43, -17, -21, 46, -36, 15, 0, 41, 20, 
-61, -12, -22, 44, 6, -36, 14, -12, -15, -16, -4, -52, 20, 38, 27, -15, 34, 60, -31, 11, 51, -17, -9, 0, -19, -45, -9, 56, 9, -21, 
-37, 34, -27, -27, 37, 47, 16, 15, 43, -53, -56, -4, 25, -6, -35, 56, -19, 40, -30, -6, 11, -39, -28, 29, -38, -18, -19, -32, -36, 13, 
-55, -52, -60, 49, 36, 24, 51, -13, -14, 34, -32, -25, -59, -45, -44, 37, 37, 27, 16, 20, -44, 3, 39, 26, -10, 9, 5, 51, 43, 32, 
};

#endif // N2D2_EXPORTC_FC2_LAYER_H