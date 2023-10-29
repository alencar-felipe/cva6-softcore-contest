#pragma once

#include <stdint.h>

#include "params.h"

typedef enum {
    Logistic,
    LogisticWithLoss,
    FastSigmoid,
    Tanh,
    TanhLeCun,
    Saturation,
    ReLU,
    Linear,
    Softplus
} ActivationFunction_T;

