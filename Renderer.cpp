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

#ifndef GL_DYNAMIC_DRAW
#define GL_DYNAMIC_DRAW 0x88E8
#endif

#ifndef GL_STREAM_DRAW
#define GL_STREAM_DRAW 0x88E0
#endif

#ifndef GL_ARRAY_BUFFER_BINDING
#define GL_ARRAY_BUFFER_BINDING 0x8894
#endif

#ifndef GL_ARRAY_BUFFER
#define GL_ARRAY_BUFFER 0x8892
#endif

//GLuint vbo; // Идентификатор VBO
GLuint vboCells; // Для клеток
GLuint vboGrid;  // Для сетки

// Определение если он не определен
typedef ptrdiff_t GLsizeiptr;
typedef ptrdiff_t GLintptr;

// Объявление указателей на функции
typedef void (APIENTRY* PFNGLGENBUFFERSPROC)(GLsizei n, GLuint* buffers);
typedef void (APIENTRY* PFNGLBINDBUFFERPROC)(GLenum target, GLuint buffer);
typedef void (APIENTRY* PFNGLBUFFERDATAPROC)(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);
typedef void (APIENTRY* PFNGLDELETEBUFFERSPROC)(GLsizei n, const GLuint* buffers);
// Добавьте объявление для glBufferSubData
typedef void (APIENTRY* PFNGLBUFFERSUBDATAPROC)(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data);

PFNGLGENBUFFERSPROC glGenBuffers = nullptr;
PFNGLBINDBUFFERPROC glBindBuffer = nullptr;
PFNGLBUFFERDATAPROC glBufferData = nullptr;
PFNGLDELETEBUFFERSPROC glDeleteBuffers = nullptr;
// Объявите указатель на функцию
PFNGLBUFFERSUBDATAPROC glBufferSubData = nullptr;

void LoadOpenGLFunctions() {
    // Загружаем функции
    glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
    glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
    glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
    glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
    glBufferSubData = (PFNGLBUFFERSUBDATAPROC)wglGetProcAddress("glBufferSubData");

    // Проверяем, что все функции успешно загружены
    if (!glGenBuffers || !glBindBuffer || !glBufferData || !glDeleteBuffers || !glBufferSubData) {
        MessageBox(NULL, L"Неполучилось загрузить указатели на OpenGL функции.", L"Ошибка", MB_OK | MB_ICONERROR);
        exit(1); // или другой способ обработки ошибки
    }
}


void CheckOpenGLError(const char* stmt, const char* fname, int line)
{
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        WCHAR errorMsg[256];
        swprintf_s(errorMsg, L"OpenGL error %08X, at %S:%d - for %S", err, fname, line, stmt);

        MessageBoxW(NULL, errorMsg, L"OpenGL Error", MB_OK | MB_ICONERROR);
        // Можно добавить дополнительную обработку ошибок или выход из программы
    }
}

#ifdef _DEBUG
#define GL_CHECK(stmt) do { \
    stmt; \
    CheckOpenGLError(#stmt, __FILE__, __LINE__); \
} while (0)
#else
#define GL_CHECK(stmt) stmt
#endif


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
    if (pixelFormat == 0) {
        MessageBox(NULL, L"Не удалось установить pixel format.", L"Ошибка", MB_OK | MB_ICONERROR);
        return;
    }
    SetPixelFormat(hdc, pixelFormat, &pfd);
    hglrc = wglCreateContext(hdc);
    if (!hglrc) {
        MessageBox(NULL, L"Не удалось создать OpenGL rendering context.", L"Ошибка", MB_OK | MB_ICONERROR);
        ReleaseDC(hWnd, hdc);
        return;
    }
    if (!wglMakeCurrent(hdc, hglrc)) {
        MessageBox(NULL, L"Failed to make the context current.", L"Ошибка", MB_OK | MB_ICONERROR);
        wglDeleteContext(hglrc);
        ReleaseDC(hWnd, hdc);
        return; // или обработка ошибки
    }

    // Загрузка функций OpenGL
    LoadOpenGLFunctions();

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Белый фон
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);

    // Создаем VBO один раз при инициализации
    RebuildVBO(GRID_SIZE);
}

// Очистка ресурсов OpenGL
void CleanupOpenGL(HWND hWnd)
{
    GL_CHECK(glDeleteBuffers(1, &vboCells));
    GL_CHECK(glDeleteBuffers(1, &vboGrid));

    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(hglrc);
    ReleaseDC(hWnd, hdc);
}

void RebuildVBO(int newGridSize)
{
    // Удаляем старые VBO
    GL_CHECK(glDeleteBuffers(1, &vboCells));
    GL_CHECK(glDeleteBuffers(1, &vboGrid));

    // Создаем новый VBO для клеток с увеличенным размером
    GL_CHECK(glGenBuffers(1, &vboCells));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vboCells));
    // Увеличиваем размер на 10% для запаса
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * newGridSize * newGridSize * 1.1, NULL, GL_DYNAMIC_DRAW));

    // Создаем новый VBO для сетки
    GL_CHECK(glGenBuffers(1, &vboGrid));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vboGrid));
    // Для сетки нам нужно достаточно памяти для всех линий
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * (newGridSize + 1) * 4, NULL, GL_STATIC_DRAW));
}

void DrawGrid() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    float cellSizeX = 2.0f / GRID_SIZE;
    float cellSizeY = 2.0f / GRID_SIZE;

    std::vector<float> verticesCells;
    std::vector<float> verticesGrid;

    // Сбор данных для клеток
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            if (grid[i][j]) {
                verticesCells.push_back(-1.0f + i * cellSizeX);
                verticesCells.push_back(1.0f - j * cellSizeY);
                verticesCells.push_back(-1.0f + (i + 1) * cellSizeX);
                verticesCells.push_back(1.0f - j * cellSizeY);
                verticesCells.push_back(-1.0f + (i + 1) * cellSizeX);
                verticesCells.push_back(1.0f - (j + 1) * cellSizeY);
                verticesCells.push_back(-1.0f + i * cellSizeX);
                verticesCells.push_back(1.0f - (j + 1) * cellSizeY);
            }
        }
    }

    // Обновляем данные в VBO для клеток
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vboCells));
    GL_CHECK(glBufferSubData(GL_ARRAY_BUFFER, 0, verticesCells.size() * sizeof(float), verticesCells.data()));


    // Отрисовка живых клеток
    GL_CHECK(glColor3f(0.0f, 0.0f, 0.0f));
    GL_CHECK(glEnableClientState(GL_VERTEX_ARRAY));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vboCells));
    GL_CHECK(glVertexPointer(2, GL_FLOAT, 0, 0));
    GL_CHECK(glDrawArrays(GL_QUADS, 0, static_cast<GLsizei>(verticesCells.size() / 2)));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));

    // Сбор данных для сетки
    for (int i = 0; i <= GRID_SIZE; ++i) {
        // Вертикальные линии
        verticesGrid.push_back(-1.0f + i * cellSizeX);
        verticesGrid.push_back(-1.0f);
        verticesGrid.push_back(-1.0f + i * cellSizeX);
        verticesGrid.push_back(1.0f);

        // Горизонтальные линии
        verticesGrid.push_back(-1.0f);
        verticesGrid.push_back(-1.0f + i * cellSizeY);
        verticesGrid.push_back(1.0f);
        verticesGrid.push_back(-1.0f + i * cellSizeY);
    }

    // Обновляем данные в VBO для сетки
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vboGrid));
    GL_CHECK(glBufferSubData(GL_ARRAY_BUFFER, 0, verticesGrid.size() * sizeof(float), verticesGrid.data()));

    // Отрисовка сетки
    GL_CHECK(glColor3f(0.5f, 0.5f, 0.5f)); // Серый цвет для сетки
    GL_CHECK(glEnableClientState(GL_VERTEX_ARRAY));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vboGrid));
    GL_CHECK(glVertexPointer(2, GL_FLOAT, 0, 0));
    GL_CHECK(glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(verticesGrid.size() / 2)));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));

    GL_CHECK(glDisableClientState(GL_VERTEX_ARRAY));
}