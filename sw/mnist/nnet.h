#ifndef N2D2_NETWORK_HPP
#define N2D2_NETWORK_HPP

#include "typedefs.h"
#include "env.h"

void propagate(const uint8_t* inputs, int32_t* outputs, uint8_t* maxPropagate_val);

#endif
