#include "SceneManager.h"

SceneManager* SceneManager::s_instance = nullptr;

SceneManager* SceneManager::GetInstance()
{
    if (s_instance == nullptr)
    {
        s_instance = new SceneManager();
    }
    return s_instance;
}

SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::Initialize(Graphics* graphics)
{
    if (m_activeScene)
    {
        m_activeScene->Initialize(graphics);
    }
}

void SceneManager::LoadScene(const std::string& name)
{
    m_activeScene = std::make_unique<Scene>(name);
    // Start is now called after initialization
}

void SceneManager::Update()
{
    if (m_activeScene)
    {
        m_activeScene->Update();
    }
}

void SceneManager::Render()
{
    if (m_activeScene)
    {
        m_activeScene->Render();
    }
}

void SceneManager::OnDestroy()
{
    if (m_activeScene)
    {
        m_activeScene->OnDestroy();
    }
}

void SceneManager::ProcessPendingChanges()
{
    if (m_activeScene)
    {
        m_activeScene->ProcessPendingChanges();
    }
}
