// Window.h
#pragma once

#include "framework.h"
#include "Grid.h"
// Функция для регистрации класса окна
ATOM MyRegisterClass(HINSTANCE hInstance);

// Функция для инициализации экземпляра окна
BOOL InitializeWindow(HINSTANCE hInstance, int nCmdShow);

// Обработчик сообщений окна
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// Обработчик диалога "О программе"
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

//// Функция для обработки кликов мыши
//void HandleMouseClick(HWND hWnd, int xPos, int yPos);