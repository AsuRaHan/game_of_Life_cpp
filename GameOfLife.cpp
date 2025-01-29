// GameOfLife.cpp
#include "GameOfLife.h"
#include <cstdlib>
#include <ctime>

// Инициализация глобальных переменных, объявленных в GameOfLife.h

std::vector<std::vector<int>> grid(GRID_SIZE, std::vector<int>(GRID_SIZE, false));

int GRID_SIZE = 100; // Определение

// Инициализация сетки случайными значениями
void InitializeGrid() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1);

    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            grid[i][j] = dis(gen);
        }
    }
}


// Подсчет количества живых соседей для клетки
int CountLiveNeighbors(int x, int y) {
    int count = 0;
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) continue; // пропустить саму клетку
            // Вычисляем координаты соседа с учетом тороидальной топологии
            int nx = (x + dx + GRID_SIZE) % GRID_SIZE;
            int ny = (y + dy + GRID_SIZE) % GRID_SIZE;
            // Учитываем состояние соседа
            count += grid[nx][ny];
        }
    }
    return count;
}

// Обновление состояния сетки в соответствии с правилами игры "Жизнь"
void UpdateGrid() {
    std::vector<std::vector<int>> newGrid = grid;
    for (int x = 0; x < GRID_SIZE; ++x) {
        for (int y = 0; y < GRID_SIZE; ++y) {
            int neighbors = CountLiveNeighbors(x, y);
            if (grid[x][y] == 1) {
                newGrid[x][y] = (neighbors == 2 || neighbors == 3) ? 1 : 0;
            }
            else {
                newGrid[x][y] = (neighbors == 3) ? 1 : 0;
            }
        }
    }
    grid = newGrid;
}
// Очистка сетки
void ClearGrid() {
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            grid[i][j] = false;
        }
    }
}

// применяем новый размер сетки
void ChangeGridSize(int newSize, HWND hWnd)
{
    // Проверяем, что новый размер положительный
    if (newSize >= 10 and newSize <= 5000)
    {
        // Изменяем размер переменной GRID_SIZE
        GRID_SIZE = newSize;
        // Создаем новую сетку с новым размером
        grid = std::vector<std::vector<int>>(GRID_SIZE, std::vector<int>(GRID_SIZE, false));
        // Перерисовываем окно, чтобы отразить изменения
        InvalidateRect(hWnd, nullptr, TRUE);
    }
    else {
        MessageBox(NULL, L"Размер сетки либо слишком большой либо слишком маленький!", L"Ошибка", MB_OK | MB_ICONERROR);
    }
}
