#pragma once
#include "stdafx.h"
#include <memory>
#include "Window.h"
#include "CoreGraphicsManager.h"
#include "../Input/InputManager.h"
#include "../Engine/SceneManager.h"
#include "TimeManager.h"
#include "../Engine/CollisionManager.h"
#include "../Engine/PickingManager.h"

//class DebugRenderer;
class DebugManager;

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
    CoreGraphicsManager* m_graphics;
    InputManager* m_inputManager;
    SceneManager* m_sceneManager;
    TimeManager* m_timeManager;
    CollisionManager* m_collisionManager;
    PickingManager* m_pickingManager;
	//DebugRenderer* m_DebugRenderer;
	DebugManager* m_DebugManager;
};
