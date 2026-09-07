#pragma once
#include "Scene.h"
#include <memory>

class Graphics; // Forward Declaration

class SceneManager
{
public:
    static SceneManager* GetInstance();

    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;

    void Initialize(Graphics* graphics);
    void LoadScene(const std::string& name);
    void Update();
    void Render();
    void OnDestroy();
    void ProcessPendingChanges();

    Scene* GetActiveScene() const { return m_activeScene.get(); }

private:
    SceneManager();
    ~SceneManager();

    static SceneManager* s_instance;
    std::unique_ptr<Scene> m_activeScene;
};
