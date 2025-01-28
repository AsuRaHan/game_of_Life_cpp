// Window.cpp
#include "framework.h"
#include "Window.h"
#include "GameOfLife.h"
#include "Renderer.h"

// ���������� ����������:
HINSTANCE hInst;                                // ������� ���������
WCHAR szTitle[MAX_LOADSTRING];                  // ����� ������ ���������
WCHAR szWindowClass[MAX_LOADSTRING];            // ��� ������ �������� ����
HWND hWnd;

int windowWidth = 1000;  // ������ ����
int windowHeight = 1000; // ������ ����

// ������� ��� ����������� ������ ����
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

// ������� ��� ������������� ���������� ����
BOOL InitializeWindow(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // ��������� ������ ���������� � ���������� ����������
    // �������� ��������� ��������
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LIFE2, szWindowClass, MAX_LOADSTRING);

    if (!MyRegisterClass(hInstance)) {
        MessageBox(nullptr, L"�� ������� ���������������� ����� ����!", L"������", MB_ICONERROR | MB_OK);
        return FALSE;
    }

    // �������� ����
    hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, windowWidth, windowHeight, nullptr, nullptr, hInstance, nullptr);
    if (!hWnd)
    {
        // ���� �������� ���� �� �������, ������� ��������� �� ������
        MessageBox(nullptr, L"�� ������� ������� ����!", L"������", MB_ICONERROR | MB_OK);
        return FALSE;
    }

    // ������������� OpenGL
    InitOpenGL(hWnd);

    // ������������� ����
    InitializeGrid();

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

// ���������� ��������� ����
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // ������ ���� ������:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDC_CHANGE_SIZE:
            KillTimer(hWnd, 1); // ������������� ������
            DialogBox(hInst, MAKEINTRESOURCE(IDD_CHANGE_SIZE_DIALOG), hWnd, ChangeSizeDialogProc);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        case IDC_START:
            SetTimer(hWnd, 1, 100, nullptr); // �������� ������
            break;
        case IDC_STOP:
            KillTimer(hWnd, 1); // ������������� ������
            break;
        case IDC_RANDOMIZE:
            InitializeGrid(); // �������� ������� ��� ���������� ����������
            InvalidateRect(hWnd, nullptr, TRUE); // �������������� ����
            break;
        case IDC_CLEAR:
            ClearGrid(); // ����������� ������� ��� ������� �����
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_SIZE:
    {
        windowWidth = LOWORD(lParam);  // ��������� ������ ����
        windowHeight = HIWORD(lParam); // ��������� ������ ����

        glViewport(0, 0, windowWidth, windowHeight);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        //float aspect = (float)windowWidth / windowHeight;
        //if (aspect > 1.0f) {
        //    glOrtho(-aspect, aspect, -1.0, 1.0, -1.0, 1.0);
        //}
        //else {
        //    glOrtho(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect, -1.0, 1.0);
        //}

        glMatrixMode(GL_MODELVIEW);
        InvalidateRect(hWnd, nullptr, FALSE); // �������������� ����
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
        DrawGrid(); // ��������� ����� � ������
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_TIMER:
        UpdateGrid(); // ���������� ��������� �����
        InvalidateRect(hWnd, nullptr, FALSE); // ����������� ����
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

// ���������� ������� "� ���������"
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
                ChangeGridSize(newSize, hWnd);
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