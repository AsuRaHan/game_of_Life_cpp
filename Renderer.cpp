// Renderer.cpp
#include "Renderer.h"
#include "GameOfLife.h"

// OpenGL ����������
HDC hdc;
HGLRC hglrc;

// ������������� OpenGL
void InitOpenGL(HWND hWnd)
{
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        32,
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        24,
        8,
        0,
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };

    hdc = GetDC(hWnd);
    int pixelFormat = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, pixelFormat, &pfd);
    hglrc = wglCreateContext(hdc);
    wglMakeCurrent(hdc, hglrc);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // ����� ���
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

// ������� �������� OpenGL
void CleanupOpenGL(HWND hWnd)
{
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(hglrc);
    ReleaseDC(hWnd, hdc);
}

// ��������� ����� � ������
void DrawGrid()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    float cellSizeX = 2.0f / GRID_SIZE;
    float cellSizeY = 2.0f / GRID_SIZE;

    // ��������� ����� ������
    glColor3f(0.0f, 0.0f, 0.0f); // ������ ���� ��� ����� ������
    glBegin(GL_QUADS);
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            if (grid[i][j]) {
                // ���������� ������ �������� ��� ����� ������
                glVertex2f(-1.0f + i * cellSizeX, 1.0f - j * cellSizeY);
                glVertex2f(-1.0f + (i + 1) * cellSizeX, 1.0f - j * cellSizeY);
                glVertex2f(-1.0f + (i + 1) * cellSizeX, 1.0f - (j + 1) * cellSizeY);
                glVertex2f(-1.0f + i * cellSizeX, 1.0f - (j + 1) * cellSizeY);
            }
        }
    }
    glEnd();

    // ��������� �����
    glColor3f(0.5f, 0.5f, 0.5f); // ����� ���� ��� �����
    glBegin(GL_LINES);
    for (int i = 0; i <= GRID_SIZE; ++i) {
        // ������������ �����
        glVertex2f(-1.0f + i * cellSizeX, -1.0f);
        glVertex2f(-1.0f + i * cellSizeX, 1.0f);
        // �������������� �����
        glVertex2f(-1.0f, -1.0f + i * cellSizeY);
        glVertex2f(1.0f, -1.0f + i * cellSizeY);
    }
    glEnd();

    SwapBuffers(hdc);
}