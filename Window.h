// Window.h
#pragma once

#include "framework.h"
#include "Grid.h"
// ������� ��� ����������� ������ ����
ATOM MyRegisterClass(HINSTANCE hInstance);

// ������� ��� ������������� ���������� ����
BOOL InitializeWindow(HINSTANCE hInstance, int nCmdShow);

// ���������� ��������� ����
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// ���������� ������� "� ���������"
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

//// ������� ��� ��������� ������ ����
//void HandleMouseClick(HWND hWnd, int xPos, int yPos);