#include "Scene.h"
#include "SpriteRenderer.h" // Needed for initialization
#include "../Core/Graphics.h" // Needed for initialization
#include "../Utils/IdGenerator.h"
#include <fstream>
#include <unordered_map>

Scene::Scene(const std::string& name) : m_name(name)
{
}

bool Scene::Load(const std::string& path, Graphics* graphics)
{
    // 1. Clear current scene
    m_gameObjects.clear();

    // 2. Read and parse file
    std::ifstream file(path);
    if (!file.is_open()) return false;
    json scene_json;
    file >> scene_json;
    file.close();

    m_name = scene_json["name"];

    std::unordered_map<uint64_t, GameObject*> id_map;
    uint64_t max_id = 0;

    // 3. First Pass: Create GameObjects and Components
    for (const auto& go_json : scene_json["game_objects"])
    {
        uint64_t id = go_json["id"];
        std::string name = go_json["name"];

        auto new_go = std::make_unique<GameObject>(id, name);
        new_go->FromJson(go_json, this);

        id_map[id] = new_go.get();
        if (id > max_id) max_id = id;

        m_gameObjects.push_back(std::move(new_go));
    }

    // 4. Second Pass: Re-establish hierarchy
    for (const auto& go_json : scene_json["game_objects"])
    {
        uint64_t id = go_json["id"];
        uint64_t parent_id = go_json["parent_id"];

        if (parent_id != 0)
        {
            GameObject* child = id_map[id];
            GameObject* parent = id_map[parent_id];
            if (child && parent)
            {
                child->SetParentRaw(parent);
            }
        }
    }

    // 5. Finalization
    IdGenerator::SetCounter(max_id);
    Initialize(graphics);
    Start();

    return true;
}

bool Scene::Save(const std::string& path)
{
    json scene_json;
    scene_json["name"] = m_name;

    json gameobjects_json = json::array();
    for (const auto& go : m_gameObjects)
    {
        json go_json;
        go->ToJson(go_json);
        gameobjects_json.push_back(go_json);
    }
    scene_json["game_objects"] = gameobjects_json;

    try
    {
        std::ofstream file(path);
        file << scene_json.dump(4); // pretty print with 4 spaces
        file.close();
        return true;
    }
    catch (const std::exception&)
    {
        return false;
    }
}

void Scene::ProcessPendingChanges()
{
    // First, process component changes for all game objects
    for (auto& go : m_gameObjects)
    {
        if (go->GetParent() == nullptr)
        {
            go->ProcessPendingChanges();
        }
    }

    // Then, remove inactive game objects from the scene
    m_gameObjects.erase(
        std::remove_if(m_gameObjects.begin(), m_gameObjects.end(),
            [](const std::unique_ptr<GameObject>& go) {
                return !go->IsActive();
            }),
        m_gameObjects.end());
}

void Scene::Initialize(Graphics* graphics)
{
    // This is a simple way to initialize components that need it.
    // A more robust system might use component masks or a messaging system.
    for (auto& go : m_gameObjects)
    {
        // A bit of a hacky way to find the right component type.
        // A better system would use RTTI or a component ID system.
        auto renderers = go->GetComponents<SpriteRenderer>();
        for (auto* renderer : renderers)
        {
            renderer->Initialize(graphics);
        }
    }
}

Scene::~Scene()
{
    // GameObjects are owned by unique_ptr, they will be cleaned up automatically.
}

void Scene::Start()
{
    for (auto& go : m_gameObjects)
    {
        if (go->GetParent() == nullptr)
        {
            go->Start();
        }
    }
}

void Scene::Update()
{
    for (auto& go : m_gameObjects)
    {
        if (go->GetParent() == nullptr)
        {
            go->Update();
        }
    }
}

void Scene::Render()
{
    for (auto& go : m_gameObjects)
    {
        if (go->GetParent() == nullptr)
        {
            go->Render();
        }
    }
}

void Scene::OnDestroy()
{
    for (auto& go : m_gameObjects)
    {
        if (go->GetParent() == nullptr)
        {
            go->OnDestroy();
        }
    }
}

GameObject* Scene::AddGameObject(const std::string& name)
{
    auto newGameObject = std::make_unique<GameObject>(name);
    GameObject* rawPtr = newGameObject.get();
    m_gameObjects.push_back(std::move(newGameObject));
    return rawPtr;
}

GameObject* Scene::FindGameObject(const std::string& name)
{
    for (const auto& go : m_gameObjects)
    {
        if (go->GetName() == name)
        {
            return go.get();
        }
    }
    return nullptr;
}
