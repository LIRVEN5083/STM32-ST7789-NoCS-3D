#include <Arduino.h> // Нужен для delay()
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_spi.h"
#include "stm32f4xx_ll_adc.h"
#include <stdlib.h>

#define STICK_PIN_X     LL_GPIO_PIN_0  // PB0 -> Ось X (Канал ADC1_IN8)
#define STICK_PIN_Y     LL_GPIO_PIN_1  // PB1 -> Ось Y (Канал ADC1_IN9)
#define STICK_BTN_PIN   LL_GPIO_PIN_10  // PB10 -> Кнопка нажатия стика

#define ADC_CHANNEL_X   LL_ADC_CHANNEL_8 // Соответствует PB0
#define ADC_CHANNEL_Y   LL_ADC_CHANNEL_9 // Соответствует PB1

#define GPIO_PORT       GPIOA
#define PIN_RST         LL_GPIO_PIN_0 // PA0
#define PIN_DC          LL_GPIO_PIN_1 // PA1
#define PIN_BLK         LL_GPIO_PIN_2 // PA2

#define DC_CMD()        { while(LL_SPI_IsActiveFlag_BSY(SPI1)); LL_GPIO_ResetOutputPin(GPIO_PORT, PIN_DC); }
#define DC_DATA()       { while(LL_SPI_IsActiveFlag_BSY(SPI1)); LL_GPIO_SetOutputPin(GPIO_PORT, PIN_DC); }

#define RST_ACTIVE()    LL_GPIO_ResetOutputPin(GPIO_PORT, PIN_RST)
#define RST_IDLE()      LL_GPIO_SetOutputPin(GPIO_PORT, PIN_RST)
#define BLK_ON()        LL_GPIO_SetOutputPin(GPIO_PORT, PIN_BLK)

extern __attribute__((aligned(4))) uint16_t frame_buffer[240 * 240];
extern bool enable_idle_animation;