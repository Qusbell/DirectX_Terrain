#include "Window.h"
#include "../Input/InputManager.h"
#include <windowsx.h> // for GET_X_LPARAM, GET_Y_LPARAM

LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    InputManager* input = InputManager::GetInstance();

    switch (message)
    {
    // Keyboard Messages
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        if (wParam < 256)
            input->OnKeyDown(wParam);
        break;
    case WM_KEYUP:
    case WM_SYSKEYUP:
        if (wParam < 256)
            input->OnKeyUp(wParam);
        break;

    // Mouse Messages
    case WM_MOUSEMOVE:
        input->OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
    case WM_LBUTTONDOWN:
        input->OnMouseDown(VK_LBUTTON, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
    case WM_LBUTTONUP:
        input->OnMouseUp(VK_LBUTTON, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
    case WM_RBUTTONDOWN:
        input->OnMouseDown(VK_RBUTTON, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
    case WM_RBUTTONUP:
        input->OnMouseUp(VK_RBUTTON, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
    case WM_MBUTTONDOWN:
        input->OnMouseDown(VK_MBUTTON, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
    case WM_MBUTTONUP:
        input->OnMouseUp(VK_MBUTTON, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
    case WM_MOUSEWHEEL:
        input->OnMouseWheel(wParam);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

Window::Window(HINSTANCE hInstance, const std::wstring& title, int width, int height)
    : m_hInstance(hInstance), m_title(title), m_width(width), m_height(height), m_hWnd(nullptr)
{
}

Window::~Window()
{
    DestroyWindow(m_hWnd);
}

bool Window::Create()
{
    WNDCLASSEXW wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = Window::WndProc;
    wc.hInstance = m_hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = L"DirectXGameFramework";

    if (!RegisterClassExW(&wc))
    {
        MessageBoxW(nullptr, L"Window Registration Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }

    RECT wr = { 0, 0, m_width, m_height };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

    m_hWnd = CreateWindowExW(
        0,
        L"DirectXGameFramework",
        m_title.c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        wr.right - wr.left,
        wr.bottom - wr.top,
        nullptr,
        nullptr,
        m_hInstance,
        nullptr
    );

    if (!m_hWnd)
    {
        MessageBoxW(nullptr, L"Window Creation Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }

    ShowWindow(m_hWnd, SW_SHOW);
    UpdateWindow(m_hWnd);

    return true;
}

bool Window::ProcessMessages()
{
    MSG msg = { 0 };
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            return false;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return true;
}
