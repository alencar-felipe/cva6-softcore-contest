// N2D2 auto-generated file.
// @ Fri Sep 10 16:23:21 2021

#ifndef N2D2_EXPORTCPP_ENV_LAYER_H
#define N2D2_EXPORTCPP_ENV_LAYER_H

#include <stdint.h>

#define NO_EXCEPT

#define ENV_SIZE_X 24
#define ENV_SIZE_Y 24
#define ENV_NB_OUTPUTS 1

#define ENV_OUTPUTS_SIZE (ENV_NB_OUTPUTS*ENV_SIZE_X*ENV_SIZE_Y)

#define NETWORK_TARGETS 1
//Output targets network dimension definition:
static unsigned int OUTPUTS_HEIGHT[NETWORK_TARGETS] = {1};
static unsigned int OUTPUTS_WIDTH[NETWORK_TARGETS] = {1};
static unsigned int NB_OUTPUTS[NETWORK_TARGETS] = {10};
static unsigned int NB_TARGET[NETWORK_TARGETS] = {10};
static unsigned int OUTPUTS_SIZE[NETWORK_TARGETS] = {1};
#endif // N2D2_EXPORTCPP_ENV_LAYER_H
