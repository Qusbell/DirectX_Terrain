#include "stdafx.h"
#include "Game.h"
#include "Engine/GameObject.h"
#include "Engine/SpriteRenderer.h"
#include "Engine/ComponentFactory.h"
#include "Engine/Transform.h"
#ifdef HOT_RELOAD_ENABLED
#include "Graphics/ShaderManager.h"
#endif

Game::Game(HINSTANCE hInstance)
    : m_hInstance(hInstance), m_inputManager(nullptr), m_sceneManager(nullptr), m_timeManager(nullptr)
{
}

Game::~Game()
{
    Shutdown();
}

bool Game::Initialize(const std::wstring& title, int width, int height)
{
    // Create Window
    m_window = std::make_unique<Window>(m_hInstance, title, width, height);
    if (!m_window->Create()) return false;

    // Create Graphics
    m_graphics = std::make_unique<Graphics>();
    if (!m_graphics->Initialize(m_window->GetHWND(), width, height)) return false;

    // Get Managers
    m_inputManager = InputManager::GetInstance();
    m_sceneManager = SceneManager::GetInstance();
    m_timeManager = TimeManager::GetInstance();
    m_timeManager->Initialize();

    // Register components with the factory
    auto* factory = ComponentFactory::GetInstance();
    factory->Register<Transform>(typeid(Transform).name());
    factory->Register<SpriteRenderer>(typeid(SpriteRenderer).name());

    // Load initial scene
    m_sceneManager->LoadScene("Default Scene");

    // --- Create a test sprite ---
    // NOTE: User must provide a 'Textures/test.png' file for this to work.
    auto go = m_sceneManager->GetActiveScene()->AddGameObject("Test Sprite");
    go->AddComponent<SpriteRenderer>(L"Textures/test.png");
    go->GetTransform()->SetLocalPosition(0, 0, 0);

    // Initialize components that need graphics resources
    m_sceneManager->Initialize(m_graphics.get());

    // Start all game objects and components
    m_sceneManager->GetActiveScene()->Start();

    return true;
}

void Game::Run()
{
    GameLoop();
}

void Game::Shutdown()
{
    if (m_sceneManager)
    {
        m_sceneManager->OnDestroy();
    }
    if (m_graphics)
    {
        m_graphics->Shutdown();
    }
}

void Game::GameLoop()
{
    while (m_window->ProcessMessages())
    {
        // 0. Update Time
        m_timeManager->Update();

        // 1. Update Input
        m_inputManager->Update();

#ifdef HOT_RELOAD_ENABLED
        ShaderManager::GetInstance()->Update();
#endif

        // 2. Update Game Logic
        m_sceneManager->Update();

        // 3. Render
        m_graphics->BeginFrame();
        m_sceneManager->Render(); // This will call render on all components
        m_graphics->EndFrame();

        // 4. Process pending component/object changes
        m_sceneManager->ProcessPendingChanges();
    }
}
