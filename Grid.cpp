// Grid.cpp
#include "Grid.h"

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
        grid[gridX][gridY] = !grid[gridX][gridY];
        InvalidateRect(hWnd, nullptr, TRUE);
    }
}