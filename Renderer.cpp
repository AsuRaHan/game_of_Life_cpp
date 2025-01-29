// Renderer.cpp
#include "Renderer.h"

// OpenGL ����������
HDC hdc;
HGLRC hglrc;

// ����������� ���������������, ���� ��� �� ����������
#ifndef GL_ARRAY_BUFFER
#define GL_ARRAY_BUFFER 0x8892
#endif

#ifndef GL_STATIC_DRAW
#define GL_STATIC_DRAW 0x88E4
#endif

#ifndef GL_DYNAMIC_DRAW
#define GL_DYNAMIC_DRAW 0x88E8
#endif

#ifndef GL_STREAM_DRAW
#define GL_STREAM_DRAW 0x88E0
#endif

GLuint vbo; // ������������� VBO

// ����������� ���� �� �� ���������
typedef ptrdiff_t GLsizeiptr;
typedef ptrdiff_t GLintptr;

// ���������� ���������� �� �������
typedef void (APIENTRY* PFNGLGENBUFFERSPROC)(GLsizei n, GLuint* buffers);
typedef void (APIENTRY* PFNGLBINDBUFFERPROC)(GLenum target, GLuint buffer);
typedef void (APIENTRY* PFNGLBUFFERDATAPROC)(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);
typedef void (APIENTRY* PFNGLDELETEBUFFERSPROC)(GLsizei n, const GLuint* buffers);
// �������� ���������� ��� glBufferSubData
typedef void (APIENTRY* PFNGLBUFFERSUBDATAPROC)(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data);

PFNGLGENBUFFERSPROC glGenBuffers = nullptr;
PFNGLBINDBUFFERPROC glBindBuffer = nullptr;
PFNGLBUFFERDATAPROC glBufferData = nullptr;
PFNGLDELETEBUFFERSPROC glDeleteBuffers = nullptr;
// �������� ��������� �� �������
PFNGLBUFFERSUBDATAPROC glBufferSubData = nullptr;

void LoadOpenGLFunctions() {
    // ��������� �������
    glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
    glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
    glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
    glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
    glBufferSubData = (PFNGLBUFFERSUBDATAPROC)wglGetProcAddress("glBufferSubData");

    // ���������, ��� ��� ������� ������� ���������
    if (!glGenBuffers || !glBindBuffer || !glBufferData || !glDeleteBuffers || !glBufferSubData) {
        MessageBox(NULL, L"������������ ��������� ��������� �� OpenGL �������.", L"������", MB_OK | MB_ICONERROR);
        exit(1); // ��� ������ ������ ��������� ������
    }
}
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
    if (pixelFormat == 0) {
        MessageBox(NULL, L"�� ������� ���������� pixel format.", L"������", MB_OK | MB_ICONERROR);
        return;
    }
    SetPixelFormat(hdc, pixelFormat, &pfd);
    hglrc = wglCreateContext(hdc);
    if (!hglrc) {
        MessageBox(NULL, L"�� ������� ������� OpenGL rendering context.", L"������", MB_OK | MB_ICONERROR);
        ReleaseDC(hWnd, hdc);
        return;
    }
    if (!wglMakeCurrent(hdc, hglrc)) {
        MessageBox(NULL, L"Failed to make the context current.", L"������", MB_OK | MB_ICONERROR);
        wglDeleteContext(hglrc);
        ReleaseDC(hWnd, hdc);
        return; // ��� ��������� ������
    }

    // �������� ������� OpenGL
    LoadOpenGLFunctions();

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // ����� ���
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

// ������� �������� OpenGL
void CleanupOpenGL(HWND hWnd)
{
    glDeleteBuffers(1, &vbo); // �������� VBO
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(hglrc);
    ReleaseDC(hWnd, hdc);
}

// ��������� ����� � ������
void DrawGrid() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    float cellSizeX = 2.0f / GRID_SIZE;
    float cellSizeY = 2.0f / GRID_SIZE;

    // ������ ��� �������� ������ ����� ������
    std::vector<float> vertices;

    // ���������� ������� vertices ������� � ����� �������
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            if (grid[i][j]) {
                // ���������� ������ �������� ��� ����� ������
                vertices.push_back(-1.0f + i * cellSizeX);
                vertices.push_back(1.0f - j * cellSizeY);
                vertices.push_back(-1.0f + (i + 1) * cellSizeX);
                vertices.push_back(1.0f - j * cellSizeY);
                vertices.push_back(-1.0f + (i + 1) * cellSizeX);
                vertices.push_back(1.0f - (j + 1) * cellSizeY);
                vertices.push_back(-1.0f + i * cellSizeX);
                vertices.push_back(1.0f - (j + 1) * cellSizeY);
            }
        }
    }

     //�������� VBO
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // ��������� ����� ������
    glColor3f(0.0f, 0.0f, 0.0f); // ������ ���� ��� ����� ������
    glEnableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexPointer(2, GL_FLOAT, 0, 0);

    // ����� �������������� size_t � GLsizei
    glDrawArrays(GL_QUADS, 0, static_cast<GLsizei>(vertices.size() / 2)); // ���������, ��� ������ �� 2, ��� ��� ������ ������ ������� �� 4 ������

    glDisableClientState(GL_VERTEX_ARRAY);

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

    // ������� VBO (���� ������ �� �����)
    glDeleteBuffers(1, &vbo);
}