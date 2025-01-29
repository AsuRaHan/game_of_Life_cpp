// Grid.cpp
#include "Grid.h"

int highlightX = -1;
int highlightY = -1;

void HandleMouseClick(HWND hWnd, int xPos, int yPos)
{
    RECT clientRect;
    GetClientRect(hWnd, &clientRect);
    int clientWidth = clientRect.right - clientRect.left;
    int clientHeight = clientRect.bottom - clientRect.top;

    float aspect = (float)clientWidth / clientHeight;
    float cellWidth, cellHeight;
    if (aspect > 1.0f) {
        cellWidth = (float)clientWidth / GRID_SIZE;
        cellHeight = cellWidth / aspect;
    }
    else {
        cellHeight = (float)clientHeight / GRID_SIZE;
        cellWidth = cellHeight * aspect;
    }

    int gridX = (int)(xPos / cellWidth);
    int gridY = (int)(yPos / cellHeight);

    // Проверка границ
    gridX = min(max(gridX, 0), GRID_SIZE - 1);
    gridY = min(max(gridY, 0), GRID_SIZE - 1);

    if (gridX >= 0 && gridX < GRID_SIZE && gridY >= 0 && gridY < GRID_SIZE)
    {
        // Установка клетки в "живую" (1) или "мертвую" (0)
        grid[gridX][gridY] = (grid[gridX][gridY] == 0) ? 1 : 0; // Инвертируем состояние
        InvalidateRect(hWnd, nullptr, TRUE);
    }
}

void UpdateMouseHighlight(HWND hWnd, int xPos, int yPos)
{
    RECT clientRect;
    GetClientRect(hWnd, &clientRect);
    int clientWidth = clientRect.right - clientRect.left;
    int clientHeight = clientRect.bottom - clientRect.top;

    float aspect = (float)clientWidth / clientHeight;
    float cellWidth, cellHeight;
    if (aspect > 1.0f) {
        cellWidth = (float)clientWidth / GRID_SIZE;
        cellHeight = cellWidth / aspect;
    }
    else {
        cellHeight = (float)clientHeight / GRID_SIZE;
        cellWidth = cellHeight * aspect;
    }

    highlightX = (int)(xPos / cellWidth);
    highlightY = (int)(yPos / cellHeight);

    // Ограничиваем значения, чтобы избежать выхода за пределы массива
    highlightX = min(max(highlightX, 0), GRID_SIZE - 1);
    highlightY = min(max(highlightY, 0), GRID_SIZE - 1);
}