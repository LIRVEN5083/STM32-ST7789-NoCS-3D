#include "init.hpp"

void Hardware_Init(void) {
    LL_GPIO_InitTypeDef gpio_init = {0};
    LL_SPI_InitTypeDef spi_init = {0};

    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);

    // PA5 (SCL) и PA7 (SDA)
    gpio_init.Pin = LL_GPIO_PIN_5 | LL_GPIO_PIN_7;
    gpio_init.Mode = LL_GPIO_MODE_ALTERNATE;
    gpio_init.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_init.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    gpio_init.Pull = LL_GPIO_PULL_NO;
    gpio_init.Alternate = LL_GPIO_AF_5; 
    LL_GPIO_Init(GPIO_PORT, &gpio_init);

    // Управление: PA0, PA1, PA2
    gpio_init.Pin = PIN_RST | PIN_DC | PIN_BLK;
    gpio_init.Mode = LL_GPIO_MODE_OUTPUT;
    LL_GPIO_Init(GPIO_PORT, &gpio_init);

    RST_ACTIVE();
    LL_GPIO_ResetOutputPin(GPIO_PORT, PIN_DC);
    LL_GPIO_ResetOutputPin(GPIO_PORT, PIN_BLK);
    delay(100);

    // Инициализация SPI1 (8-бит на старте)
    spi_init.TransferDirection = LL_SPI_FULL_DUPLEX; 
    spi_init.Mode = LL_SPI_MODE_MASTER;
    spi_init.DataWidth = LL_SPI_DATAWIDTH_8BIT; 
    spi_init.ClockPolarity = LL_SPI_POLARITY_HIGH; 
    spi_init.ClockPhase = LL_SPI_PHASE_2EDGE;     
    spi_init.NSS = LL_SPI_NSS_SOFT;                
    spi_init.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV4; // 24 МГц!
    spi_init.BitOrder = LL_SPI_MSB_FIRST;
    spi_init.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
    LL_SPI_Init(SPI1, &spi_init);
    
    SPI1->CR1 |= SPI_CR1_SSI; 
    LL_SPI_Enable(SPI1); 
}

void ST7789_Init(void) {
    RST_IDLE(); 
    delay(150);
    ST7789_SendCommand(0x01); 
    delay(150);
    ST7789_SendCommand(0x11); 
    delay(120);
    ST7789_SendCommand(0x3A); 
    ST7789_SendData(0x55);    
    ST7789_SendCommand(0x36); 
    ST7789_SendData(0x00);  

    ST7789_SendCommand(0x2A); 
    ST7789_SendData(0x00); ST7789_SendData(0x00);
    ST7789_SendData(0x00); ST7789_SendData(0xEF);

    ST7789_SendCommand(0x2B); 
    ST7789_SendData(0x00); ST7789_SendData(0x00);
    ST7789_SendData(0x00); ST7789_SendData(0xEF);

    ST7789_SendCommand(0x21); 
    ST7789_SendCommand(0x29); 
    delay(50);
    BLK_ON(); 
}