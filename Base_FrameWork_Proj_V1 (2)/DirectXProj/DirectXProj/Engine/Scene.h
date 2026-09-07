#pragma once
#include <vector>
#include <memory>
#include <string>
#include "GameObject.h"

class Graphics; // Forward Declaration

class Scene
{
public:
    Scene(const std::string& name);
    ~Scene();

    void Initialize(Graphics* graphics);
    void Start();
    void Update();
    void Render();
    void OnDestroy();
    void ProcessPendingChanges();

    GameObject* AddGameObject(const std::string& name = "GameObject");
    GameObject* FindGameObject(const std::string& name);

    bool Save(const std::string& path);
    bool Load(const std::string& path, Graphics* graphics);

private:
    std::string m_name;
    std::vector<std::unique_ptr<GameObject>> m_gameObjects;
};
