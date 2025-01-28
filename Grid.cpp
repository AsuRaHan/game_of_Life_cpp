// Grid.cpp
#include "Grid.h"
//#include "GameOfLife.h"
//#include "Renderer.h"

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

    // ѕровер€ем, что координаты наход€тс€ в пределах сетки
    if (gridX >= 0 && gridX < GRID_SIZE && gridY >= 0 && gridY < GRID_SIZE)
    {
        // »змен€ем состо€ние клетки
        grid[gridX][gridY] = !grid[gridX][gridY];

        // ѕерерисовываем окно, чтобы отобразить изменени€
        InvalidateRect(hWnd, nullptr, TRUE);
    }
}