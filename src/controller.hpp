#pragma once
#include "core.hpp"

#define STICK_MIN  1928 // 2048 - 120
#define STICK_MAX  2168 // 2048 + 120

uint32_t ADC_ReadChannel(uint32_t Channel);