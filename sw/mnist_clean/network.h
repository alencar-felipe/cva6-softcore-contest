#pragma once

#include <stdlib.h>
#include <stdio.h>

#include "misc.h"

#include "resources/all.h"

void inference(const uint8_t* input, int32_t* output, uint8_t* credence);
