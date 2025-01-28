// Window.cpp
#include "framework.h"
#include "Window.h"
#include "GameOfLife.h"
#include "Renderer.h"

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

int windowWidth = 1000;  // Ширина окна
int windowHeight = 1000; // Высота окна

// Функция для регистрации класса окна
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

// Функция для инициализации экземпляра окна
BOOL InitializeWindow(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной
    // Загрузка строковых ресурсов
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LIFE2, szWindowClass, MAX_LOADSTRING);


    if (!MyRegisterClass(hInstance)) {
        MessageBox(nullptr, L"Не удалось зарегистрировать класс окна!", L"Ошибка", MB_ICONERROR | MB_OK);
        return FALSE;
    }

    // Создание окна
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, windowWidth, windowHeight, nullptr, nullptr, hInstance, nullptr);
    if (!hWnd)
    {
        // Если создание окна не удалось, выводим сообщение об ошибке
        MessageBox(nullptr, L"Не удалось создать окно!", L"Ошибка", MB_ICONERROR | MB_OK);
        return FALSE;
    }

    // Инициализация OpenGL
    InitOpenGL(hWnd);

    // Инициализация игры
    InitializeGrid();

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

// Обработчик сообщений окна
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Анализ меню выбора:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        case IDC_START:
            SetTimer(hWnd, 1, 100, nullptr); // Включаем таймер
            break;
        case IDC_STOP:
            KillTimer(hWnd, 1); // Останавливаем таймер
            break;
        case IDC_RANDOMIZE:
            InitializeGrid(); // Вызываем функцию для случайного заполнения
            InvalidateRect(hWnd, nullptr, TRUE); // Перерисовываем окно
            break;
        case IDC_CLEAR:
            ClearGrid(); // Добавленная функция для очистки сетки
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_SIZE:
    {
        windowWidth = LOWORD(lParam);  // Обновляем ширину окна
        windowHeight = HIWORD(lParam); // Обновляем высоту окна

        glViewport(0, 0, windowWidth, windowHeight);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        float aspect = (float)windowWidth / windowHeight;
        if (aspect > 1.0f) {
            //glOrtho(-aspect, aspect, -1.0, 1.0, -1.0, 1.0);
        }
        else {
            //glOrtho(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect, -1.0, 1.0);
        }

        glMatrixMode(GL_MODELVIEW);
        InvalidateRect(hWnd, nullptr, FALSE); // Перерисовываем окно
    }
    break;
    case WM_LBUTTONDOWN:
    {
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
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

// Обработчик диалога "О программе"
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

//void HandleMouseClick(HWND hWnd, int xPos, int yPos)
//{
//    // Получаем размеры клиентской области окна
//    RECT clientRect;
//    GetClientRect(hWnd, &clientRect);
//    int clientWidth = clientRect.right - clientRect.left;
//    int clientHeight = clientRect.bottom - clientRect.top;
//
//    // Вычисляем аспектное соотношение
//    float aspect = (float)clientWidth / clientHeight;
//
//    // Вычисляем размер ячейки в пикселях, учитывая аспектное соотношение
//    float cellWidth, cellHeight;
//    if (aspect > 1.0f) {
//        // Если окно шире, чем высота
//        cellWidth = (float)clientWidth / GRID_SIZE;
//        cellHeight = cellWidth / aspect; // Корректируем высоту ячейки
//    }
//    else {
//        // Если окно выше, чем ширина
//        cellHeight = (float)clientHeight / GRID_SIZE;
//        cellWidth = cellHeight * aspect; // Корректируем ширину ячейки
//    }
//
//    // Переводим координаты мыши в координаты сетки без инверсии Y
//    int gridX = (int)(xPos / cellWidth);
//    int gridY = (int)(yPos / cellHeight);
//
//    // Проверяем, что координаты находятся в пределах сетки
//    if (gridX >= 0 && gridX < GRID_SIZE && gridY >= 0 && gridY < GRID_SIZE)
//    {
//        // Изменяем состояние клетки
//        grid[gridX][gridY] = !grid[gridX][gridY];
//
//        // Перерисовываем окно, чтобы отобразить изменения
//        InvalidateRect(hWnd, nullptr, TRUE);
//    }
//}