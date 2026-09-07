#include "Scene.h"
#include "IRenderer.h"
#include "SpriteRenderer.h" // Needed for initialization
#include "../Core/CoreGraphicsManager.h" // Needed for initialization
#include "../Utils/IdGenerator.h"
#include "PickingManager.h"
#include <fstream>
#include <unordered_map>

Scene::Scene(const std::string& name) : m_name(name), m_mainCamera(nullptr)
{
}

bool Scene::Load(const std::string& path, CoreGraphicsManager* graphics)
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
    // 1. Add new GameObjects
    if (!m_pendingAddGameObjects.empty())
    {
        for (auto& newGO : m_pendingAddGameObjects)
        {
            // Call Awake on all components of the new GameObject
            newGO->ProcessPendingChanges(true); // true to force Awake call
            // Register with PickingManager
            PickingManager::GetI()->RegisterObject(newGO.get());
            // Move to the main list
            m_gameObjects.push_back(std::move(newGO));
        }
        m_pendingAddGameObjects.clear();
    }

    // 2. Process component changes for existing game objects
    for (auto& go : m_gameObjects)
    {
        if (go && go->GetParent() == nullptr)
        {
            go->ProcessPendingChanges(false); // false, Awake has been called
        }
    }

    // 3. Remove destroyed GameObjects
    m_gameObjects.erase(
        std::remove_if(m_gameObjects.begin(), m_gameObjects.end(),
            [](const std::unique_ptr<GameObject>& go) {
                if (go->IsDestroyed())
                {
                    PickingManager::GetI()->UnregisterObject(go.get());
                    return true;
                }
                return false;
            }),
        m_gameObjects.end());
}

Scene::~Scene()
{
    // GameObjects are owned by unique_ptr, they will be cleaned up automatically.
}

void Scene::Start()
{
    for (auto& go : m_gameObjects)
    {
        if (go && go->IsActive() && go->GetParent() == nullptr)
        {
            // Call Start on all components that haven't been started yet
            auto components = go->GetComponents<Component>();
            for (auto* comp : components)
            {
                if (!comp->HasStarted())
                {
                    comp->Start();
                    comp->SetStarted(true);
                }
            }
        }
    }
}

void Scene::Update()
{
    for (auto& go : m_gameObjects)
    {
        if (go && go->IsActive() && go->GetParent() == nullptr)
        {
            go->Update();
        }
    }
}

void Scene::PreRender( )
{
	if ( !m_mainCamera ) return; // Can't render without a camera

	for ( auto& go : m_gameObjects )
	{
		if ( go && go->IsActive( ) && go->GetParent( ) == nullptr )
		{
			go->PreRender(m_mainCamera);
		}
	}
}
void Scene::PostRender( )
{
	if ( !m_mainCamera ) return; // Can't render without a camera

	for ( auto& go : m_gameObjects )
	{
		if ( go && go->IsActive( ) && go->GetParent( ) == nullptr )
		{
			go->PostRender(m_mainCamera);
		}
	}
}
void Scene::OnGUIRender( )
{
	if ( !m_mainCamera ) return; // Can't render without a camera

	for ( auto& go : m_gameObjects )
	{
		if ( go && go->IsActive( ) && go->GetParent( ) == nullptr )
		{
			go->OnGUIRender(m_mainCamera);
		}
	}
}
void Scene::Render()
{
    if (!m_mainCamera) return; // Can't render without a camera

    for (auto& go : m_gameObjects)
    {
        if (go && go->IsActive() && go->GetParent() == nullptr)
        {
            go->Render(m_mainCamera);
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
    m_pendingAddGameObjects.push_back(std::move(newGameObject));
    return rawPtr;
}

void Scene::RemoveGameObject(GameObject* gameObject)
{
    if (gameObject)
    {
        gameObject->Destroy();
    }
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
