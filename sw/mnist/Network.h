#ifndef N2D2_NETWORK_HPP
#define N2D2_NETWORK_HPP

#include "typedefs.h"
#include "env.h"

void propagate(const uint_t* inputs, Target_T* outputs, uint_t* maxPropagate_val);

#endif
