// Window.h
#pragma once

#ifndef WINDOW_H
#define WINDOW_H

#include "framework.h"
#include "Grid.h"

extern int currentSpeed; // ��������� �������� � �������������

extern int windowWidth;  // ������ ����
extern int windowHeight; // ������ ����
extern bool gameRunning; // ���������� ���������� ����� ����

void StartGame();
void StopGame();
void UpdateWindowTitle(HWND hWnd);
// ������� ��� ����������� ������ ����
ATOM MyRegisterClass(HINSTANCE hInstance);

// ������� ��� ������������� ���������� ����
BOOL InitializeWindow(HINSTANCE hInstance, int nCmdShow);

// ���������� ��������� ����
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// ���������� ������� "� ���������"
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK ChangeSizeDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

#endif //WINDOW_H