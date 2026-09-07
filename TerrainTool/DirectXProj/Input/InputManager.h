#pragma once
#include <Windows.h>
#include <unordered_map>

enum class KeyState
{
    None,   // Not pressed
    Down,   // Just pressed
    Pressed,// Held down
    Up      // Just released
};

class InputManager
{
public:
    static InputManager* GetInstance();

    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;

    void Initialize(int screenWidth, int screenHeight);
    void Update();
    void OnKeyDown(WPARAM wParam);
    void OnKeyUp(WPARAM wParam);
    void OnMouseMove(int x, int y);
    void OnMouseDown(WPARAM wParam, int x, int y);
    void OnMouseUp(WPARAM wParam, int x, int y);
    void OnMouseWheel(WPARAM wParam);

    bool IsKeyDown(int key);
    bool IsKeyUp(int key);
    bool IsKeyPressed(int key);

    int GetMouseX() const { return m_mouseX; }
    int GetMouseY() const { return m_mouseY; }
    int GetMouseWheelDelta() const { return m_mouseWheelDelta; }
    int GetScreenWidth() const { return m_screenWidth; }
    int GetScreenHeight() const { return m_screenHeight; }

private:
    InputManager();
    ~InputManager();

    static InputManager* s_instance;

    std::unordered_map<int, KeyState> m_keyStates;
    int m_mouseX;
    int m_mouseY;
    int m_mouseWheelDelta;
    int m_screenWidth;
    int m_screenHeight;
};
