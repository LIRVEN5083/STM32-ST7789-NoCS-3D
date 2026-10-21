#include "draw.hpp"

void ST7789_PushBuffer(void) {
    ST7789_SendCommand(0x2C); // RAM Write
    DC_DATA();
    
    LL_SPI_Disable(SPI1);
    LL_SPI_SetDataWidth(SPI1, LL_SPI_DATAWIDTH_16BIT);
    LL_SPI_Enable(SPI1);
    
    uint32_t total_pixels = 240 * 240;
    uint32_t i = 0;
    
    for (; i < total_pixels; i += 4) {
        while (!LL_SPI_IsActiveFlag_TXE(SPI1));
        LL_SPI_TransmitData16(SPI1, frame_buffer[i]);

        while (!LL_SPI_IsActiveFlag_TXE(SPI1));
        LL_SPI_TransmitData16(SPI1, frame_buffer[i+1]);

        while (!LL_SPI_IsActiveFlag_TXE(SPI1));
        LL_SPI_TransmitData16(SPI1, frame_buffer[i+2]);

        while (!LL_SPI_IsActiveFlag_TXE(SPI1));
        LL_SPI_TransmitData16(SPI1, frame_buffer[i+3]);
    }
    
    while (!LL_SPI_IsActiveFlag_TXE(SPI1));
    while (LL_SPI_IsActiveFlag_BSY(SPI1));
    
    LL_SPI_Disable(SPI1);
    LL_SPI_SetDataWidth(SPI1, LL_SPI_DATAWIDTH_8BIT);
    LL_SPI_Enable(SPI1);
    (void)SPI1->DR;
}

void DrawPixel(uint16_t x, uint16_t y, uint16_t color) {
    if (x >= 240 || y >= 240) return;
    frame_buffer[y * 240 + x] = __builtin_bswap16(color);
}

void ClearBuffer(uint16_t color) {
    uint16_t swapped_color = __builtin_bswap16(color);
    for (uint32_t i = 0; i < 240 * 240; i++) {
        frame_buffer[i] = swapped_color;
    }
}

void DrawLine(int x0, int y0, int x1, int y1, uint16_t color) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    while (true) {
        DrawPixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void DrawLineThick(int x0, int y0, int x1, int y1, int thickness, uint16_t color) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    bool flat = (dx > abs(dy));

    while (true) {
        for (int t = 0; t < thickness; t++) {
            int offset = t - (thickness / 2);
            if (flat) {
                DrawPixel(x0, y0 + offset, color); // Смещение по Y
            } else {
                DrawPixel(x0 + offset, y0, color); // Смещение по X
            }
        }

        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void Cube3D_Update(float speedX, float speedY, float speedZ) {
    angleX += speedX;
    angleY += speedY;
    angleZ += speedZ;

    float cx = cosf(angleX), sx = sinf(angleX);
    float cy = cosf(angleY), sy = sinf(angleY);
    float cz = cosf(angleZ), sz = sinf(angleZ);

    float camera_distance = 180.0f; 

    for (int i = 0; i < 8; i++) {
        // Текущая вершина куба
        float x = cube_vertices[i].x;
        float y = cube_vertices[i].y;
        float z = cube_vertices[i].z;

        // Вращение по оси X
        float y1 = y * cx - z * sx;
        float z1 = y * sx + z * cx;

        // Вращение по оси Y
        float x2 = x * cy + z1 * sy;
        float z2 = -x * sy + z1 * cy;

        // Вращение по оси Z
        float x3 = x2 * cz - y1 * sz;
        float y3 = x2 * sz + y1 * cz;

        // Проекция 3D пространства на 2D плоскость экрана (с учетом перспективы)
        float depth = 1.0f / (camera_distance - z2);
        
        // Сдвигаем координаты на центр экрана 240х240 (то есть +120)
        projected_points[i].x = (int)(120 + x3 * camera_distance * depth);
        projected_points[i].y = (int)(120 + y3 * camera_distance * depth);
    }
}

void Cube3D_Render(uint16_t color) {
    ClearBuffer(0x0000);
    int thickness = 5;

    // Проходим по всем 12 ребрам куба и рисуем их линиями Брезенхема
    for (int i = 0; i < 12; i++) {
        int p0 = cube_edges[i][0];
        int p1 = cube_edges[i][1];

        DrawLineThick(projected_points[p0].x, projected_points[p0].y,
                      projected_points[p1].x, projected_points[p1].y,
                      thickness, color);
                 
    }

    ST7789_PushBuffer();
}