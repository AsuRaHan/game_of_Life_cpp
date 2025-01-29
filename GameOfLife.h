// GameOfLife.h
#pragma once

#ifndef GAMEOFLIFE_H
#define GAMEOFLIFE_H

#include <vector>
#include <random>
#include "framework.h"
#include "Renderer.h"

// √лобальные переменные
extern int GRID_SIZE; // ќбъ€вление
extern std::vector<std::vector<int>> grid;


// ‘ункции дл€ работы с игрой "∆изнь"
void InitializeGrid();
int CountLiveNeighbors(int x, int y);
void UpdateGrid();
void ClearGrid();
void ChangeGridSize(int newSize, HWND hWnd);

#endif //GAMEOFLIFE_H