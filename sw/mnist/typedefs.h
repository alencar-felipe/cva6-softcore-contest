#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <stdint.h>

#include "params.h"

typedef uint8_t uint_t;
typedef int8_t int_t;
typedef int32_t long_t;

typedef enum {
    Logistic,
    LogisticWithLoss,
    FastSigmoid,
    Tanh,
    TanhLeCun,
    Saturation,
    Rectifier,
    Linear,
    Softplus
} ActivationFunction_T;

typedef enum {
    Max,
    Average
} Pooling_T;

typedef struct {
    unsigned int lowIndex;
    unsigned int highIndex;
    float interpolation;
} Interpolation;

typedef enum {
    Sum,
    Mult
} OpMode_T;

typedef enum {
    PerLayer,
    PerInput,
    PerChannel
} CoeffMode_T;

#if defined(HAS_AP_CINT) && NB_BITS > 0 && NB_BITS != 8 && NB_BITS != 16 \
    && NB_BITS != 32 && NB_BITS != 64
#define CONCAT(x, y) x##y
#define INT(x) CONCAT(int, x)
#define UINT(x) CONCAT(uint, x)

#define MULT_0_4 0
#define MULT_1_4 4
#define MULT_2_4 8
#define MULT_3_4 12
#define MULT_4_4 16
#define MULT_5_4 20
#define MULT_6_4 24
#define MULT_7_4 28
#define MULT_8_4 32
#define MULT_9_4 36
#define MULT_10_4 40
#define MULT_11_4 44
#define MULT_12_4 48
#define MULT_13_4 52
#define MULT_14_4 56
#define MULT_15_4 60
#define MULT_16_4 64
#define CONCAT_MULT(x, y) MULT_##x##_##y
#define MULT(x, y) CONCAT_MULT(x, y)

#include <ap_cint.h>

#endif

typedef int_t Wint_t;

#if NB_BITS < 0
#define int_t_MAX 1.0
#define int_t_MIN -1.0
#define uint_t_MAX 1.0
#define uint_t_MIN 0.0
#else
#define int_t_MAX ((1LL << (NB_BITS - 1)) - 1)
#define int_t_MIN (-(1LL << (NB_BITS - 1)))
#define uint_t_MAX ((1LL << NB_BITS) - 1)
#define uint_t_MIN 0LL
#endif

#endif // N2D2_EXPORTC_TYPEDEFS_H
