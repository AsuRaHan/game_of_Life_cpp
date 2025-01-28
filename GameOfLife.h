// GameOfLife.h
#pragma once

#include <vector>
#include <random>
#include "framework.h"

// ���������
#define MAX_LOADSTRING 100

// ���������� ����������
//extern HINSTANCE hInst;
//extern WCHAR szTitle[MAX_LOADSTRING];
//extern WCHAR szWindowClass[MAX_LOADSTRING];

const int GRID_SIZE = 50;
extern std::vector<std::vector<bool>> grid;

// ������� ��� ������ � ����� "�����"
void InitializeGrid();
int CountLiveNeighbors(int x, int y);
void UpdateGrid();
void ClearGrid();