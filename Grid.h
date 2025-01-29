// Grid.h
#pragma once

#ifndef GRID_H
#define GRID_H

#include "GameOfLife.h"


extern int highlightX;
extern int highlightY;

// Функция для обработки кликов мыши
void HandleMouseClick(HWND hWnd, int xPos, int yPos);
void UpdateMouseHighlight(HWND hWnd, int xPos, int yPos);
#endif //GRID_H