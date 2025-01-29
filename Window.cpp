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

int currentSpeed = 100; // ��������� �������� � �������������

int windowWidth = 1000;  // ������ ����
int windowHeight = 1000; // ������ ����

// ������� ��� ����������� ������ ����
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;// | CS_DROPSHADOW;
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

    // ������� RECT ��� �������� ����
    RECT windowRect = { 0, 0, windowWidth, windowHeight };

    // ������������ ������� � ������ �����, ��������� � ����
    AdjustWindowRectEx(&windowRect, WS_OVERLAPPEDWINDOW | WS_VISIBLE, TRUE, 0);

    // �������� ���� � ������ ����������������� ��������
    hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr, nullptr, hInstance, nullptr);
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
            SetTimer(hWnd, 1, currentSpeed, nullptr); // �������� ������
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
            InvalidateRect(hWnd, nullptr, TRUE); // �������������� ����
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    //case WM_MOUSEMOVE:
    //{
        //int xPos = GET_X_LPARAM(lParam);
        //int yPos = GET_Y_LPARAM(lParam);
        // ������� ��� ����������� ��������� ������ ��� ��������
        //UpdateMouseHighlight(hWnd, xPos, yPos);
        //InvalidateRect(hWnd, nullptr, FALSE);
    //}
    break;
    case WM_SIZE:
    {
        windowWidth = LOWORD(lParam);
        windowHeight = HIWORD(lParam);

        glViewport(0, 0, windowWidth, windowHeight);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        float aspect = (float)windowWidth / windowHeight;
        if (aspect > 1.0f) {
            glOrtho(-aspect, aspect, -1.0, 1.0, -1.0, 1.0);
        }
        else {
            glOrtho(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect, -1.0, 1.0);
        }

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
        SwapBuffers(GetDC(hWnd));
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
    {
        // ������������� ����� ������� � �������� ����������
        SendDlgItemMessage(hDlg, IDC_SIZE_EDIT, EM_SETLIMITTEXT, (WPARAM)5, 0); // ����������� ����� ��� �������
        SendDlgItemMessage(hDlg, IDC_SPEED_EDIT, EM_SETLIMITTEXT, (WPARAM)5, 0); // ����������� ����� ��� ��������

        WCHAR sizeBuffer[10];
        WCHAR speedBuffer[10];

        // ��������� �������� ������� �����
        swprintf_s(sizeBuffer, 10, L"%d", GRID_SIZE);
        SetDlgItemText(hDlg, IDC_SIZE_EDIT, sizeBuffer);

        // ��������� ������� �������� (������������, ��� ������� �������� �������� � ���������� ���������� `currentSpeed`)
        swprintf_s(speedBuffer, 10, L"%d", currentSpeed);
        SetDlgItemText(hDlg, IDC_SPEED_EDIT, speedBuffer);

        return (INT_PTR)TRUE;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_CHANGE_SIZE_BUTTON:
        {
            WCHAR newSizeBuffer[10];
            WCHAR newSpeedBuffer[10];
            GetDlgItemText(hDlg, IDC_SIZE_EDIT, newSizeBuffer, 10);
            GetDlgItemText(hDlg, IDC_SPEED_EDIT, newSpeedBuffer, 10);
            int newSize = _wtoi(newSizeBuffer);
            currentSpeed = _wtoi(newSpeedBuffer);
            ChangeGridSize(newSize, hWnd);
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;

        }
        break;
        case IDOK:
        case IDCANCEL:
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}