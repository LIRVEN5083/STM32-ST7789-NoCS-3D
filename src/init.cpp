#include "init.hpp"

void SPI1_Init(void) {
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

void ADC_and_Button_Init(void) {
    LL_GPIO_InitTypeDef gpio_init = {0};

    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1);

    gpio_init.Pin = STICK_PIN_X | STICK_PIN_Y;
    gpio_init.Mode = LL_GPIO_MODE_ANALOG;
    gpio_init.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOB, &gpio_init);

    gpio_init.Pin = STICK_BTN_PIN;
    gpio_init.Mode = LL_GPIO_MODE_INPUT;
    gpio_init.Pull = LL_GPIO_PULL_UP; 
    LL_GPIO_Init(GPIOB, &gpio_init);

    LL_ADC_CommonInitTypeDef adc_common_init = {0};
    adc_common_init.CommonClock = LL_ADC_CLOCK_SYNC_PCLK_DIV4;
    LL_ADC_CommonInit(__LL_ADC_COMMON_INSTANCE(ADC1), &adc_common_init);

    LL_ADC_InitTypeDef adc_init = {0};
    adc_init.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
    adc_init.Resolution = LL_ADC_RESOLUTION_12B;
    LL_ADC_Init(ADC1, &adc_init);

    LL_ADC_REG_InitTypeDef adc_reg_init = {0};
    adc_reg_init.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
    adc_reg_init.SequencerLength = LL_ADC_REG_SEQ_SCAN_DISABLE;
    adc_reg_init.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
    adc_reg_init.ContinuousMode = LL_ADC_REG_CONV_SINGLE;
    LL_ADC_REG_Init(ADC1, &adc_reg_init);

    LL_ADC_Enable(ADC1);
    delay(10);
}