#include "commands.hpp"

void SPI_WriteByte8(uint8_t data) {
    while (!LL_SPI_IsActiveFlag_TXE(SPI1)); 
    LL_SPI_TransmitData8(SPI1, data);       
    while (LL_SPI_IsActiveFlag_BSY(SPI1));  
    (void)SPI1->DR; 
}

void ST7789_SendCommand(uint8_t cmd) {
    DC_CMD();
    SPI_WriteByte8(cmd);
}

void ST7789_SendData(uint8_t data) {
    DC_DATA();
    SPI_WriteByte8(data);
}