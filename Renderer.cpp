// Renderer.cpp
#include "Renderer.h"

// OpenGL переменные
HDC hdc;
HGLRC hglrc;

// Определение идентификаторов, если они не определены
#ifndef GL_ARRAY_BUFFER
#define GL_ARRAY_BUFFER 0x8892
#endif

#ifndef GL_STATIC_DRAW
#define GL_STATIC_DRAW 0x88E4
#endif

GLuint vbo; // Идентификатор VBO

// Определение GLsizeiptr, если он не определен
typedef ptrdiff_t GLsizeiptr;
// Объявление указателей на функции
typedef void (APIENTRY* PFNGLGENBUFFERSPROC)(GLsizei n, GLuint* buffers);
typedef void (APIENTRY* PFNGLBINDBUFFERPROC)(GLenum target, GLuint buffer);
typedef void (APIENTRY* PFNGLBUFFERDATAPROC)(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);
typedef void (APIENTRY* PFNGLDELETEBUFFERSPROC)(GLsizei n, const GLuint* buffers);

PFNGLGENBUFFERSPROC glGenBuffers = nullptr;
PFNGLBINDBUFFERPROC glBindBuffer = nullptr;
PFNGLBUFFERDATAPROC glBufferData = nullptr;
PFNGLDELETEBUFFERSPROC glDeleteBuffers = nullptr;

void LoadOpenGLFunctions() {
    glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
    glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
    glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
    glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");

    if (!glGenBuffers || !glBindBuffer || !glBufferData || !glDeleteBuffers) {
        MessageBox(NULL, L"Failed to load OpenGL functions.", L"Error", MB_OK | MB_ICONERROR);
        exit(1);
    }
}

// Инициализация OpenGL
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

    // Загрузка функций OpenGL
    LoadOpenGLFunctions();

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Белый фон
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

// Очистка ресурсов OpenGL
void CleanupOpenGL(HWND hWnd)
{
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(hglrc);
    ReleaseDC(hWnd, hdc);
}

// Отрисовка сетки и клеток
void DrawGrid() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    float cellSizeX = 2.0f / GRID_SIZE;
    float cellSizeY = 2.0f / GRID_SIZE;

    // Вектор для хранения вершин живых клеток
    std::vector<float> vertices;

    // Заполнение вектора vertices данными о живых клетках
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            if (grid[i][j]) {
                // Координаты вершин квадрата для живой клетки
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

    // Создание VBO
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Отрисовка живых клеток
    glColor3f(0.0f, 0.0f, 0.0f); // Черный цвет для живых клеток
    glEnableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexPointer(2, GL_FLOAT, 0, 0);

    // Явное преобразование size_t в GLsizei
    glDrawArrays(GL_QUADS, 0, static_cast<GLsizei>(vertices.size() / 2)); // Убедитесь, что делите на 2, так как каждая клетка состоит из 4 вершин

    glDisableClientState(GL_VERTEX_ARRAY);

    // Отрисовка сетки
    glColor3f(0.5f, 0.5f, 0.5f); // Серый цвет для сетки
    glBegin(GL_LINES);
    for (int i = 0; i <= GRID_SIZE; ++i) {
        // Вертикальные линии
        glVertex2f(-1.0f + i * cellSizeX, -1.0f);
        glVertex2f(-1.0f + i * cellSizeX, 1.0f);
        // Горизонтальные линии
        glVertex2f(-1.0f, -1.0f + i * cellSizeY);
        glVertex2f(1.0f, -1.0f + i * cellSizeY);
    }
    glEnd();

    // Очистка VBO (если больше не нужен)
    glDeleteBuffers(1, &vbo);
}