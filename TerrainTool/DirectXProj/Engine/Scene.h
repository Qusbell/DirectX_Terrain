#pragma once
#include <vector>
#include <memory>
#include <string>
#include "GameObject.h"

class CoreGraphicsManager; // Forward Declaration
class Camera;   // Forward Declaration

class Scene
{
public:
    Scene(const std::string& name);
    ~Scene();

    void Start();
    void Update();
	void PreRender();
    void Render();
	void PostRender( );
	void OnGUIRender( );
    void OnDestroy();
    void ProcessPendingChanges();

    GameObject* AddGameObject(const std::string& name = "GameObject");
    void RemoveGameObject(GameObject* gameObject);
    GameObject* FindGameObject(const std::string& name);
    const std::vector<std::unique_ptr<GameObject>>& GetGameObjects() const { return m_gameObjects; }

    void SetMainCamera(Camera* camera) { m_mainCamera = camera; }
    Camera* GetMainCamera() const { return m_mainCamera; }

    bool Save(const std::string& path);
    bool Load(const std::string& path, CoreGraphicsManager* graphics);

    template<typename T>
    std::vector<T*> GetAllComponents() const;

private:
    std::string m_name;
    std::vector<std::unique_ptr<GameObject>> m_gameObjects;
    std::vector<std::unique_ptr<GameObject>> m_pendingAddGameObjects;
    Camera* m_mainCamera;
};

template<typename T>
std::vector<T*> Scene::GetAllComponents() const
{
    std::vector<T*> components;
    for (const auto& go : m_gameObjects)
    {
        auto foundComponents = go->GetComponents<T>();
        if (!foundComponents.empty())
        {
            components.insert(components.end(), foundComponents.begin(), foundComponents.end());
        }
    }
    return components;
}
