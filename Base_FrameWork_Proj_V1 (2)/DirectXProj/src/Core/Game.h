#pragma once
#include "stdafx.h"
#include <memory>
#include "Window.h"
#include "Graphics.h"
#include "../Input/InputManager.h"
#include "../Engine/SceneManager.h"
#include "TimeManager.h"

class Game
{
public:
    Game(HINSTANCE hInstance);
    ~Game();

    bool Initialize(const std::wstring& title, int width, int height);
    void Run();
    void Shutdown();

private:
    void GameLoop();

    HINSTANCE m_hInstance;
    std::unique_ptr<Window> m_window;
    std::unique_ptr<Graphics> m_graphics;
    InputManager* m_inputManager;
    SceneManager* m_sceneManager;
    TimeManager* m_timeManager;
};
