#pragma once
#include <Windows.h>
#include <string>

class Window
{
public:
    Window(HINSTANCE hInstance, const std::wstring& title, int width, int height);
    ~Window();

    bool Create();
    bool ProcessMessages();
    HWND GetHWND() const { return m_hWnd; }

private:
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    HINSTANCE m_hInstance;
    HWND m_hWnd;
    std::wstring m_title;
    int m_width;
    int m_height;
};
