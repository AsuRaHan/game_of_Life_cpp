// GameOfLife.h
#pragma once

#include <vector>
#include <random>
#include "framework.h"

// Константы
#define MAX_LOADSTRING 100

// Глобальные переменные
//extern HINSTANCE hInst;
//extern WCHAR szTitle[MAX_LOADSTRING];
//extern WCHAR szWindowClass[MAX_LOADSTRING];

const int GRID_SIZE = 50;
extern std::vector<std::vector<bool>> grid;

// Функции для работы с игрой "Жизнь"
void InitializeGrid();
int CountLiveNeighbors(int x, int y);
void UpdateGrid();
void ClearGrid();