#include "init.hpp"
#include "draw.hpp"
#include "colors.hpp"

__attribute__((aligned(4))) uint16_t frame_buffer[240 * 240];

int main(void) {
    init(); 
    SystemCoreClockUpdate();
    
    Hardware_Init();
    ST7789_Init();

    uint32_t delayTime = 0;

    while (1) {
        Cube3D_Update(0.03f, 0.04f, 0.01f);

        Cube3D_Render(COLOR_GREEN);

        delay(delayTime); 
    }
    return 0;
}
