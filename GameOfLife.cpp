// GameOfLife.cpp
#include "GameOfLife.h"
#include <cstdlib>
#include <ctime>

// Инициализация глобальных переменных, объявленных в GameOfLife.h
//HINSTANCE hInst;
//WCHAR szTitle[MAX_LOADSTRING];
//WCHAR szWindowClass[MAX_LOADSTRING];

std::vector<std::vector<bool>> grid(GRID_SIZE, std::vector<bool>(GRID_SIZE, false));

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
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if (i == 0 && j == 0) continue; // Пропускаем саму клетку

            // Вычисляем координаты соседа с учетом тороидальной топологии
            int nx = (x + i + GRID_SIZE) % GRID_SIZE;
            int ny = (y + j + GRID_SIZE) % GRID_SIZE;

            // Учитываем состояние соседа
            count += grid[nx][ny] ? 1 : 0;
        }
    }
    return count;
}

// Обновление состояния сетки в соответствии с правилами игры "Жизнь"
void UpdateGrid() {
    std::vector<std::vector<bool>> newGrid = grid;
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            int liveNeighbors = CountLiveNeighbors(i, j);
            if (grid[i][j]) {
                newGrid[i][j] = (liveNeighbors == 2 || liveNeighbors == 3);
            }
            else {
                newGrid[i][j] = (liveNeighbors == 3);
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