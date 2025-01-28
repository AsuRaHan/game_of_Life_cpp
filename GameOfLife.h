// GameOfLife.h
#pragma once

#ifndef GAMEOFLIFE_H
#define GAMEOFLIFE_H

#include <vector>
#include <random>
#include "framework.h"
//#include "Window.h"
// ���������


// ���������� ����������
extern int GRID_SIZE; // ����������
extern std::vector<std::vector<bool>> grid;

// ������� ��� ������ � ����� "�����"
void InitializeGrid();
int CountLiveNeighbors(int x, int y);
void UpdateGrid();
void ClearGrid();
void ChangeGridSize(int newSize, HWND hWnd);

#endif //GAMEOFLIFE_H