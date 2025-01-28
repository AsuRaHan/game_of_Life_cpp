// GameOfLife.h
#pragma once

#ifndef GAMEOFLIFE_H
#define GAMEOFLIFE_H

#include <vector>
#include <random>
#include "framework.h"
//#include "Window.h"
// Константы


// Глобальные переменные
extern int GRID_SIZE; // Объявление
extern std::vector<std::vector<bool>> grid;

// Функции для работы с игрой "Жизнь"
void InitializeGrid();
int CountLiveNeighbors(int x, int y);
void UpdateGrid();
void ClearGrid();
void ChangeGridSize(int newSize, HWND hWnd);

#endif //GAMEOFLIFE_H