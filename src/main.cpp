#include "init.hpp"
#include "draw.hpp"
#include "controller.hpp"
#include "colors.hpp"

__attribute__((aligned(4))) uint16_t frame_buffer[240 * 240];
// Флаг включение/выключения когда фигура сама по себе крутится типо IDLE анимация
bool enable_idle_animation = false;

// Переключение всех возможных цветов
static const uint16_t cube_colors[] = {
    COLOR_WHITE,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_BLUE,
    COLOR_YELLOW,
    COLOR_CYAN,
    COLOR_MAGENTA,
    COLOR_ORANGE,
    COLOR_PURPLE,
    COLOR_PINK,
    COLOR_GOLD
};
static const uint8_t TOTAL_COLORS = sizeof(cube_colors) / sizeof(cube_colors[0]);

int main(void) {
    init(); 
    SystemCoreClockUpdate();
    
    ADC_and_Button_Init();
    SPI1_Init();
    ST7789_Init();

    uint32_t x_val = 0;
    uint32_t y_val = 0;

    uint32_t delayTime = 0;

    bool button_was_pressed = false;
    static uint8_t current_color_index = 0;

    while (1) {
        x_val = ADC_ReadChannel(ADC_CHANNEL_X);
        y_val = ADC_ReadChannel(ADC_CHANNEL_Y);

        if (LL_GPIO_IsInputPinSet(GPIOB, STICK_BTN_PIN) == 0) {
            if (!button_was_pressed) {
                current_color_index++;
                
                if (current_color_index >= TOTAL_COLORS) {
                    current_color_index = 0;
                }
                
                button_was_pressed = true;
            }
        } else {
            button_was_pressed = false;
        }

        float current_speedX = 0.00f;
        float current_speedY = 0.00f;
        bool stick_moved = false;

        if (x_val > STICK_MAX) {
            current_speedY = 0.08f;
            stick_moved = true;
        } 
        else if (x_val < STICK_MIN) {
            current_speedY = -0.08f;
            stick_moved = true;
        }

        if (y_val > STICK_MAX) {
            current_speedX = -0.08f;
            stick_moved = true;
        }
        else if (y_val < STICK_MIN) {
            current_speedX = 0.08f;
            stick_moved = true;
        }

        if (stick_moved) {
            Cube3D_Update(current_speedX, current_speedY, 0.00f);
        } else {
            if (enable_idle_animation) {
                Cube3D_Update(0.01f, 0.01f, 0.00f);
            } else {
                Cube3D_Update(0.00f, 0.00f, 0.00f);
            }
        }

        Cube3D_Render(cube_colors[current_color_index]);
        
        if (delayTime) {
            delay(delayTime);
        } else {
            delay(10);
        }
    }
    return 0;
}
