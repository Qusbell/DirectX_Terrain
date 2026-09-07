#include "InputManager.h"

InputManager* InputManager::s_instance = nullptr;

InputManager* InputManager::GetInstance()
{
    if (s_instance == nullptr)
    {
        s_instance = new InputManager();
    }
    return s_instance;
}

InputManager::InputManager() : m_mouseX(0), m_mouseY(0)
{
}

InputManager::~InputManager()
{
}

void InputManager::Update()
{
    for (auto& pair : m_keyStates)
    {
        if (pair.second == KeyState::Down)
        {
            pair.second = KeyState::Pressed;
        }
        else if (pair.second == KeyState::Up)
        {
            pair.second = KeyState::None;
        }
    }
}

void InputManager::OnKeyDown(WPARAM wParam)
{
    if (m_keyStates[wParam] == KeyState::None || m_keyStates[wParam] == KeyState::Up)
    {
        m_keyStates[wParam] = KeyState::Down;
    }
}

void InputManager::OnKeyUp(WPARAM wParam)
{
    m_keyStates[wParam] = KeyState::Up;
}

void InputManager::OnMouseMove(int x, int y)
{
    m_mouseX = x;
    m_mouseY = y;
}

void InputManager::OnMouseDown(WPARAM wParam, int x, int y)
{
    // For now, we just handle it like a key. Can be extended for specific buttons.
    OnKeyDown(wParam);
    m_mouseX = x;
    m_mouseY = y;
}

void InputManager::OnMouseUp(WPARAM wParam, int x, int y)
{
    OnKeyUp(wParam);
    m_mouseX = x;
    m_mouseY = y;
}

bool InputManager::IsKeyDown(int key)
{
    return m_keyStates[key] == KeyState::Down;
}

bool InputManager::IsKeyUp(int key)
{
    return m_keyStates[key] == KeyState::Up;
}

bool InputManager::IsKeyPressed(int key)
{
    return m_keyStates[key] == KeyState::Pressed || m_keyStates[key] == KeyState::Down;
}
