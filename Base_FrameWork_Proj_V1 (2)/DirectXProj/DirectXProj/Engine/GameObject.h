#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include "Component.h"
#include "Transform.h"

class ObjectRegistry; // Forward Declaration

class GameObject
{
public:
    GameObject(const std::string& name = "GameObject");
    GameObject(uint64_t id, const std::string& name = "GameObject");
    ~GameObject();

    void Start();
    void Update();
    void Render();
    void OnDestroy();
    void ProcessPendingChanges();

    void SetParent(GameObject* parent);
    GameObject* GetParent() const { return m_parent; }
    const std::vector<GameObject*>& GetChildren() const { return m_children; }

    template<typename T>
    T* GetComponent() const;

    template<typename T>
    std::vector<T*> GetComponents() const;

    template<typename T, typename... TArgs>
    T* AddComponent(TArgs&&... args);

    template<typename T>
    void RemoveComponent();

    const std::string& GetName() const { return m_name; }
    Transform* GetTransform() const { return m_transform.get(); }
    uint64_t GetId() const { return m_id; }

    bool IsActive() const { return m_isActive; }
    void Destroy(); // Mark for destruction

private:
    friend class Scene; // Allow Scene to access component map for serialization and raw parent setting
    void AddChild(GameObject* child);
    void RemoveChild(GameObject* child);
    void SetParentRaw(GameObject* parent); // For loading from scene file

    void ToJson(json& j) const;
    void FromJson(const json& j, Scene* scene);

    const uint64_t m_id;
    std::string m_name;
    bool m_isActive;
    std::unique_ptr<Transform> m_transform;

    // Component Management
    std::unordered_map<std::type_index, std::vector<std::unique_ptr<Component>>> m_components;
    std::vector<std::unique_ptr<Component>> m_pendingAddComponents;
    std::vector<Component*> m_pendingRemoveComponents;

    // Hierarchy
    GameObject* m_parent;
    std::vector<GameObject*> m_children;
};


// Template implementations must be in the header file
template<typename T>
T* GameObject::GetComponent() const
{
    auto it = m_components.find(typeid(T));
    if (it != m_components.end() && !it->second.empty())
    {
        // Return the first component of this type
        return dynamic_cast<T*>(it->second[0].get());
    }
    // Also check pending components
    for (const auto& comp : m_pendingAddComponents) {
        if (auto casted = dynamic_cast<T*>(comp.get())) {
            return casted;
        }
    }
    return nullptr;
}

template<typename T>
std::vector<T*> GameObject::GetComponents() const
{
    std::vector<T*> result;
    auto it = m_components.find(typeid(T));
    if (it != m_components.end())
    {
        for (const auto& comp : it->second)
        {
            result.push_back(dynamic_cast<T*>(comp.get()));
        }
    }
    // Also check pending components
    for (const auto& comp : m_pendingAddComponents) {
        if (auto casted = dynamic_cast<T*>(comp.get())) {
            result.push_back(casted);
        }
    }
    return result;
}


template<typename T, typename... TArgs>
T* GameObject::AddComponent(TArgs&&... args)
{
    auto newComponent = std::make_unique<T>(this, std::forward<TArgs>(args)...);
    T* rawPtr = newComponent.get();
    m_pendingAddComponents.push_back(std::move(newComponent));
    return rawPtr;
}

template<typename T>
void GameObject::RemoveComponent()
{
    auto it = m_components.find(typeid(T));
    if (it != m_components.end() && !it->second.empty())
    {
        // Mark the first component of this type for removal
        m_pendingRemoveComponents.push_back(it->second[0].get());
    }
}
