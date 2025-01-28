// Grid.cpp
#include "Grid.h"
//#include "GameOfLife.h"
//#include "Renderer.h"

void HandleMouseClick(HWND hWnd, int xPos, int yPos)
{
    // Получаем размеры клиентской области окна
    RECT clientRect;
    GetClientRect(hWnd, &clientRect);
    int clientWidth = clientRect.right - clientRect.left;
    int clientHeight = clientRect.bottom - clientRect.top;

    // Вычисляем аспектное соотношение
    float aspect = (float)clientWidth / clientHeight;

    // Вычисляем размер ячейки в пикселях, учитывая аспектное соотношение
    float cellWidth, cellHeight;
    if (aspect > 1.0f) {
        // Если окно шире, чем высота
        cellWidth = (float)clientWidth / GRID_SIZE;
        cellHeight = cellWidth / aspect; // Корректируем высоту ячейки
    }
    else {
        // Если окно выше, чем ширина
        cellHeight = (float)clientHeight / GRID_SIZE;
        cellWidth = cellHeight * aspect; // Корректируем ширину ячейки
    }

    // Переводим координаты мыши в координаты сетки без инверсии Y
    int gridX = (int)(xPos / cellWidth);
    int gridY = (int)(yPos / cellHeight);

    // Проверяем, что координаты находятся в пределах сетки
    if (gridX >= 0 && gridX < GRID_SIZE && gridY >= 0 && gridY < GRID_SIZE)
    {
        // Изменяем состояние клетки
        grid[gridX][gridY] = !grid[gridX][gridY];

        // Перерисовываем окно, чтобы отобразить изменения
        InvalidateRect(hWnd, nullptr, TRUE);
    }
}