#pragma once
#include "core.hpp"

//TODO: Base commands
void SPI_WriteByte8(uint8_t data);
void ST7789_SendCommand(uint8_t cmd);
void ST7789_SendData(uint8_t data);