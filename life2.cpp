#include "framework.h"
#include "life2.h"
#include <gl/GL.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <random>
#pragma comment(lib, "opengl32.lib")

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

int windowWidth = 1000;  // Ширина окна
int windowHeight = 1000; // Высота окна

// OpenGL переменные
HDC hdc;
HGLRC hglrc;
HWND hWnd;

// Игра "Жизнь"
int GRID_SIZE = 250; // размер мира
std::vector<std::vector<bool>> grid(GRID_SIZE, std::vector<bool>(GRID_SIZE, false));

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void InitializeGrid() {
    // Создаем генератор случайных чисел
    // Используем std::random_device для получения случайного seed
    std::random_device rd;
    // std::mt19937 — это реализация Mersenne Twister, популярный генератор случайных чисел
    std::mt19937 gen(rd());
    // Создаем распределение от 0 до 1, где 0 будет false, а 1 — true
    std::uniform_int_distribution<> dis(0, 1);

    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            // Используем распределение для генерации случайного значения
            grid[i][j] = dis(gen);
        }
    }
}
void ChangeGridSize(int newSize)
{
    // Проверяем, что новый размер положительный
    if (newSize > 0)
    {
        // Изменяем размер переменной GRID_SIZE
        GRID_SIZE = newSize;

        // Создаем новую сетку с новым размером
        grid = std::vector<std::vector<bool>>(GRID_SIZE, std::vector<bool>(GRID_SIZE, false));

        // Перерисовываем окно, чтобы отразить изменения
        InvalidateRect(hWnd, nullptr, TRUE);
    }
}
int CountLiveNeighbors(int x, int y) {
    int count = 0;
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if (i == 0 && j == 0) continue; // Пропускаем саму клетку

            // Вычисляем координаты соседа с учетом тороидальной топологии
            int nx = (x + i + GRID_SIZE) % GRID_SIZE;
            int ny = (y + j + GRID_SIZE) % GRID_SIZE;

            // Учитываем состояние соседа
            count += grid[nx][ny] ? 1 : 0;
        }
    }
    return count;
}

int CountLiveNeighbors2(int x, int y) {
    int count = 0;
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if (i == 0 && j == 0) continue;
            int nx = x + i;
            int ny = y + j;
            if (nx >= 0 && nx < GRID_SIZE && ny >= 0 && ny < GRID_SIZE) {
                count += grid[nx][ny] ? 1 : 0;
            }
        }
    }
    return count;
}

void UpdateGrid() {
    std::vector<std::vector<bool>> newGrid = grid;
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            int liveNeighbors = CountLiveNeighbors(i, j);
            if (grid[i][j]) {
                newGrid[i][j] = (liveNeighbors == 2 || liveNeighbors == 3);
            }
            else {
                newGrid[i][j] = (liveNeighbors == 3);
            }
        }
    }
    grid = newGrid;
}

void DrawGrid() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    float cellSizeX = 2.0f / GRID_SIZE;
    float cellSizeY = 2.0f / GRID_SIZE;

    // Отрисовка живых клеток
    glColor3f(0.0f, 0.0f, 0.0f); // Черный цвет для живых клеток
    glBegin(GL_QUADS);
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            if (grid[i][j]) {
                // Координаты вершин квадрата для живой клетки
                glVertex2f(-1.0f + i * cellSizeX, 1.0f - j * cellSizeY);
                glVertex2f(-1.0f + (i + 1) * cellSizeX, 1.0f - j * cellSizeY);
                glVertex2f(-1.0f + (i + 1) * cellSizeX, 1.0f - (j + 1) * cellSizeY);
                glVertex2f(-1.0f + i * cellSizeX, 1.0f - (j + 1) * cellSizeY);
            }
        }
    }
    glEnd();

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

    SwapBuffers(hdc);
}

void ClearGrid() {
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            grid[i][j] = false;
        }
    }
    // Перерисовываем окно после очистки сетки
    //InvalidateRect(hWnd, nullptr, TRUE);
}

void InitOpenGL(HWND hWnd) {
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

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Белый фон
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

void CleanupOpenGL(HWND hWnd) {
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(hglrc);
    ReleaseDC(hWnd, hdc);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LIFE2, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LIFE2));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LIFE2));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LIFE2);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

    hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, windowWidth, windowHeight, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    InitOpenGL(hWnd);
    InitializeGrid();
    //SetTimer(hWnd, 1, 100, nullptr); // Таймер для обновления каждые 100 мс

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

void HandleMouseClick(HWND hWnd, int xPos, int yPos)
{
    // Получаем размеры клиентской области окна
    RECT clientRect;
    GetClientRect(hWnd, &clientRect);
    int clientWidth = clientRect.right - clientRect.left;
    int clientHeight = clientRect.bottom - clientRect.top;

    // Вычисляем аспектное соотношение
    float aspect = (float)clientWidth / clientHeight;

    // Вычисляем размер ячейки в пикселях, учитывая аспектное соотношение
    float cellWidth, cellHeight;
    if (aspect > 1.0f) {
        // Если окно шире, чем высота
        cellWidth = (float)clientWidth / GRID_SIZE;
        cellHeight = cellWidth / aspect; // Корректируем высоту ячейки
    }
    else {
        // Если окно выше, чем ширина
        cellHeight = (float)clientHeight / GRID_SIZE;
        cellWidth = cellHeight * aspect; // Корректируем ширину ячейки
    }

    // Переводим координаты мыши в координаты сетки без инверсии Y
    int gridX = (int)(xPos / cellWidth);
    int gridY = (int)(yPos / cellHeight);

    // Проверяем, что координаты находятся в пределах сетки
    if (gridX >= 0 && gridX < GRID_SIZE && gridY >= 0 && gridY < GRID_SIZE)
    {
        // Изменяем состояние клетки
        grid[gridX][gridY] = !grid[gridX][gridY];

        // Перерисовываем окно, чтобы отобразить изменения
        InvalidateRect(hWnd, nullptr, TRUE);
    }
}
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK ChangeSizeDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_CHANGE_SIZE_BUTTON)
        {
            WCHAR buffer[10];
            GetDlgItemText(hDlg, IDC_SIZE_EDIT, buffer, 10);
            int newSize = _wtoi(buffer);
            if (newSize > 0)
            {
                ChangeGridSize(newSize);
                EndDialog(hDlg, LOWORD(wParam));
                return (INT_PTR)TRUE;
            }
        }
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDC_CHANGE_SIZE:
            KillTimer(hWnd, 1); // Останавливаем таймер
            DialogBox(hInst, MAKEINTRESOURCE(IDD_CHANGE_SIZE_DIALOG), hWnd, ChangeSizeDialogProc);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        case IDC_START:
            // Запуск игры
            SetTimer(hWnd, 1, 100, nullptr); // Включаем таймер
            break;
        case IDC_STOP:
            // Остановка игры
            KillTimer(hWnd, 1); // Останавливаем таймер
            break;
        case IDC_RANDOMIZE:
            // Случайное заполнение сетки
            InitializeGrid(); // Вызываем функцию для случайного заполнения
            InvalidateRect(hWnd, nullptr, TRUE); // Перерисовываем окно
            break;
        case IDC_CLEAR:
            // Случайное заполнение сетки
            ClearGrid(); // Вызываем функцию очистки сетки
            InvalidateRect(hWnd, nullptr, TRUE); // Перерисовываем окно
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;

    case WM_SIZE: // Обработка изменения размеров окна
    {
        windowWidth = LOWORD(lParam);  // Обновляем ширину окна
        windowHeight = HIWORD(lParam); // Обновляем высоту окна

        glViewport(0, 0, windowWidth, windowHeight);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        float aspect = (float)windowWidth / windowHeight;
        if (aspect > 1.0f) {
            //glOrtho(-aspect, aspect, -1.0, 1.0, -1.0, 1.0); // Для широкого окна
        }
        else {
            //glOrtho(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect, -1.0, 1.0); // Для высокого окна
        }

        glMatrixMode(GL_MODELVIEW);
        InvalidateRect(hWnd, nullptr, FALSE); // Перерисовываем окно
    }
    break;
    case WM_LBUTTONDOWN:
    {
        // Получаем координаты клика мыши
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);

        // Переводим координаты мыши в координаты сетки
        HandleMouseClick(hWnd, xPos, yPos);
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);
        DrawGrid(); // Отрисовка сетки и клеток
        EndPaint(hWnd, &ps);
    }
    break;

    case WM_TIMER:
        UpdateGrid(); // Обновление состояния сетки
        InvalidateRect(hWnd, nullptr, FALSE); // Перерисовка окна
        break;

    case WM_DESTROY:
        KillTimer(hWnd, 1);
        CleanupOpenGL(hWnd);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

