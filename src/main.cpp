#include "init.hpp"
#include "draw.hpp"
#include "controller.hpp"
#include "colors.hpp"

__attribute__((aligned(4))) uint16_t frame_buffer[240 * 240];

int main(void) {
    init(); 
    SystemCoreClockUpdate();
    
    ADC_and_Button_Init();
    SPI1_Init();
    ST7789_Init();

    uint32_t x_val = 0;
    uint32_t y_val = 0;

    uint32_t delayTime = 0;

    while (1) {
        x_val = ADC_ReadChannel(ADC_CHANNEL_X);
        y_val = ADC_ReadChannel(ADC_CHANNEL_Y);

        // Логика цвета: по умолчанию белый
        uint16_t color = COLOR_WHITE;

        // Быстрая проверка выхода за пороги без тяжелой математики
        if (x_val > STICK_MAX) {
            Cube3D_Update(0.03f, 0.04f, 0.01f);
            color = COLOR_RED;
        } 
        else if (x_val < STICK_MIN) {
            Cube3D_Update(-0.03f, 0.04f, 0.01f);
            color = COLOR_WHITE; // Твой дефолтный цвет
        }

        if (y_val > STICK_MAX) {
            Cube3D_Update(0.03f, 0.04f, 0.01f);
            color = COLOR_GREEN;
        }
        else if (y_val < STICK_MIN) {
            Cube3D_Update(0.03f, -0.04f, 0.01f);
        }

        // Если стик в мертвой зоне — просто обновляем сцену дефолтным вращением
        if (x_val >= STICK_MIN && x_val <= STICK_MAX && y_val >= STICK_MIN && y_val <= STICK_MAX) {
            Cube3D_Update(0.01f, 0.01f, 0.01f);
        }

        // Вызываем рендер строго ОДИН раз за проход цикла
        Cube3D_Render(color);
        
        // delayTime должен быть 0 для максимального FPS, если дисплей успевает кушать данные
        if (delayTime) delay(delayTime); 
    }
    return 0;
}
