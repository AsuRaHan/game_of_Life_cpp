// Window.h
#pragma once

#ifndef WINDOW_H
#define WINDOW_H

#include "framework.h"
#include "Grid.h"

extern int currentSpeed; // Начальная скорость в миллисекундах

extern int windowWidth;  // Ширина окна
extern int windowHeight; // Высота окна
extern bool gameRunning; // Глобальная переменная пауза игры

void StartGame();
void StopGame();
void UpdateWindowTitle(HWND hWnd);
// Функция для регистрации класса окна
ATOM MyRegisterClass(HINSTANCE hInstance);

// Функция для инициализации экземпляра окна
BOOL InitializeWindow(HINSTANCE hInstance, int nCmdShow);

// Обработчик сообщений окна
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// Обработчик диалога "О программе"
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK ChangeSizeDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

#endif //WINDOW_H