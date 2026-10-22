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

void RotateAxis(Point3D* axis, float sinA, float cosA, int mode) {
    float tmp1, tmp2;
    if (mode == 0) {
        tmp1 = axis->y * cosA - axis->z * sinA;
        tmp2 = axis->y * sinA + axis->z * cosA;
        axis->y = tmp1;
        axis->z = tmp2;
    } else if (mode == 1) {
        tmp1 = axis->x * cosA + axis->z * sinA;
        tmp2 = -axis->x * sinA + axis->z * cosA;
        axis->x = tmp1;
        axis->z = tmp2;
    } else {
        tmp1 = axis->x * cosA - axis->y * sinA;
        tmp2 = axis->x * sinA + axis->y * cosA;
        axis->x = tmp1;
        axis->y = tmp2;
    }
}

void Cube3D_Update(float speedX, float speedY, float speedZ) {
    float halfX = speedX * 0.5f;
    float halfY = speedY * 0.5f;
    float halfZ = speedZ * 0.5f;

    float sx = sinf(halfX), cx = cosf(halfX);
    float sy = sinf(halfY), cy = cosf(halfY);
    float sz = sinf(halfZ), cz = cosf(halfZ);

    Quaternion q_step;
    q_step.x = sx * cy * cz - cx * sy * sz;
    q_step.y = cx * sy * cz + sx * cy * sz;
    q_step.z = cx * cy * sz - sx * sy * cz;
    q_step.w = cx * cy * cz + sx * sy * sz;

    Quaternion q_next;
    q_next.w = q_step.w * q_current.w - q_step.x * q_current.x - q_step.y * q_current.y - q_step.z * q_current.z;
    q_next.x = q_step.w * q_current.x + q_step.x * q_current.w + q_step.y * q_current.z - q_step.z * q_current.y;
    q_next.y = q_step.w * q_current.y - q_step.x * q_current.z + q_step.y * q_current.w + q_step.z * q_current.x;
    q_next.z = q_step.w * q_current.z + q_step.x * q_current.y - q_step.y * q_current.x + q_step.z * q_current.w;

    float len = sqrtf(q_next.x * q_next.x + q_next.y * q_next.y + q_next.z * q_next.z + q_next.w * q_next.w);
    if (len > 0.0f) {
        q_current.x = q_next.x / len;
        q_current.y = q_next.y / len;
        q_current.z = q_next.z / len;
        q_current.w = q_next.w / len;
    }

    float xx = q_current.x * q_current.x;
    float xy = q_current.x * q_current.y;
    float xz = q_current.x * q_current.z;
    float xw = q_current.x * q_current.w;
    float yy = q_current.y * q_current.y;
    float yz = q_current.y * q_current.z;
    float yw = q_current.y * q_current.w;
    float zz = q_current.z * q_current.z;
    float zw = q_current.z * q_current.w;

    float r00 = 1.0f - 2.0f * (yy + zz); float r01 = 2.0f * (xy - zw);        float r02 = 2.0f * (xz + yw);
    float r10 = 2.0f * (xy + zw);        float r11 = 1.0f - 2.0f * (xx + zz); float r12 = 2.0f * (yz - xw);
    float r20 = 2.0f * (xz - yw);        float r21 = 2.0f * (yz + xw);        float r22 = 1.0f - 2.0f * (xx + yy);

    const float camera_distance = 180.0f; 


    for (int i = 0; i < 8; i++) {
        float x = cube_vertices[i].x;
        float y = cube_vertices[i].y;
        float z = cube_vertices[i].z;

        float x_rot = x * r00 + y * r01 + z * r02;
        float y_rot = x * r10 + y * r11 + z * r12;
        float z_rot = x * r20 + y * r21 + z * r22;

        float depth = 1.0f / (camera_distance - z_rot);
        
        projected_points[i].x = (int)(120 + x_rot * camera_distance * depth);
        projected_points[i].y = (int)(120 + y_rot * camera_distance * depth);
    }
}

void Cube3D_Render(uint16_t color) {
    ClearBuffer(0xFFFF);
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