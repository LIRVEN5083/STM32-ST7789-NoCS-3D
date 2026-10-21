#pragma once
#include "core.hpp"
#include "commands.hpp"
#include "math.h"

void ST7789_PushBuffer(void);

void DrawPixel(uint16_t x, uint16_t y, uint16_t color);

void ClearBuffer(uint16_t color);

// Структура для 3D координат
struct Point3D {
    float x, y, z;
};

// Структура для спроецированных 2D координат экрана
struct Point2D {
    int x, y;
};

// 8 вершин куба в локальных координатах (размер куба от -40 до +40)
static const Point3D cube_vertices[8] = {
    {-40, -40, -40}, { 40, -40, -40}, { 40,  40, -40}, {-40,  40, -40},
    {-40, -40,  40}, { 40, -40,  40}, { 40,  40,  40}, {-40,  40,  40}
};

// Сетка ребер куба (какие вершины соединять линиями между собой)
static const uint8_t cube_edges[12][2] = {
    {0, 1}, {1, 2}, {2, 3}, {3, 0}, // Задняя грань
    {4, 5}, {5, 6}, {6, 7}, {7, 4}, // Передняя грань
    {0, 4}, {1, 5}, {2, 6}, {3, 7}  // Боковые ребра
};

// Массив для хранения текущих рассчитанных 2D точек экрана
static Point2D projected_points[8];

// Углы поворота куба по трем осям 
static float angleX = 0.0f;
static float angleY = 0.0f;
static float angleZ = 0.0f;

// Быстрый алгоритм Брезенхема для рисования линий в frame_buffer
void DrawLine(int x0, int y0, int x1, int y1, uint16_t color);
// Альтернативный метод с укаханием толщины
void DrawLineThick(int x0, int y0, int x1, int y1, int thickness, uint16_t color);

// TODO: ФУНКЦИЯ ПОДСЧЕТА
void Cube3D_Update(float speedX, float speedY, float speedZ);

// TODO: ФУНКЦИЯ ОТРИСОВКИ ble
void Cube3D_Render(uint16_t color);