#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include "Component.h"
#include "Transform.h"

class ObjectRegistry; // Forward Declaration
class Camera;

class GameObject
{
public:
    GameObject(const std::string& name = "GameObject");
    GameObject(uint64_t id, const std::string& name = "GameObject");
    ~GameObject();

    void Start();
    void Update();
    void Render(Camera* camera);
	void PreRender(Camera* camera);
	void PostRender(Camera* camera);
	void OnGUIRender(Camera* camera);

    void OnDestroy();
    void ProcessPendingChanges(bool isNewObject = false);

    void SetParent(GameObject* parent);
    GameObject* GetParent() const { return m_parent; }
    const std::vector<GameObject*>& GetChildren() const { return m_children; }

    template<typename T>
    T* GetComponent() const;
	template<typename T>
	T* GetComponentType( ) const;

    template<typename T>
    std::vector<T*> GetComponents() const;

    template<typename T, typename... TArgs>
    T* AddComponent(TArgs&&... args);

    template<typename T>
    void RemoveComponent();

    template<typename T>
    T* GetComponentInChildren() const;

    template<typename T>
    std::vector<T*> GetComponentsInChildren() const;

    const std::string& GetName() const { return m_name; }
    Transform* GetTransform() const { return m_transform.get(); }
    uint64_t GetId() const { return m_id; }
    unsigned int GetPickingID() const { return m_pickingId; }
    void SetPickingID(unsigned int id) { m_pickingId = id; }

    bool IsActive() const { return m_isActive; }
    void SetActive(bool active);
    bool IsDestroyed() const { return m_isDestroyed; }
    void Destroy(); // Mark for destruction

private:
    friend class Scene; // Allow Scene to access component map for serialization and raw parent setting
    void AddChild(GameObject* child);
    void RemoveChild(GameObject* child);
    void SetParentRaw(GameObject* parent); // For loading from scene file

    void ToJson(json& j) const;
    void FromJson(const json& j, Scene* scene);

    const uint64_t m_id;
    unsigned int m_pickingId;
    std::string m_name;
    bool m_isActive;
    bool m_isDestroyed;
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
    // Search existing components
    for (const auto& pair : m_components)
    {
        for (const auto& comp : pair.second)
        {
            if (T* casted = dynamic_cast<T*>(comp.get()))
            {
                return casted;
            }
        }
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
inline T* GameObject::GetComponentType( ) const
{
	std::vector<T*> result;
	auto it = m_components.find(typeid( T ));
	if ( it != m_components.end( ) )
	{
		for ( const auto& comp : it->second )
		{
			result.push_back(dynamic_cast< T* >( comp.get( ) ));
		}
	}
	// Also check pending components
	for ( const auto& comp : m_pendingAddComponents ) {
		if ( auto casted = dynamic_cast< T* >( comp.get( ) ) ) {
			result.push_back(casted);
		}
	}
	return result;
}

template<typename T>
std::vector<T*> GameObject::GetComponents() const
{
    std::vector<T*> result;
    // Search existing components
    for (const auto& pair : m_components)
    {
        for (const auto& comp : pair.second)
        {
            if (T* casted = dynamic_cast<T*>(comp.get()))
            {
                result.push_back(casted);
            }
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

template<typename T>
T* GameObject::GetComponentInChildren() const
{
    // First, check the current GameObject
    T* component = GetComponent<T>();
    if (component)
    {
        return component;
    }

    // If not found, recurse through children
    for (const auto* child : m_children)
    {
        component = child->GetComponentInChildren<T>();
        if (component)
        {
            return component;
        }
    }

    return nullptr;
}

template<typename T>
std::vector<T*> GameObject::GetComponentsInChildren() const
{
    std::vector<T*> result;

    // Get components from the current GameObject
    std::vector<T*> components = GetComponents<T>();
    result.insert(result.end(), components.begin(), components.end());

    // Recurse through children and get their components
    for (const auto* child : m_children)
    {
        std::vector<T*> childComponents = child->GetComponentsInChildren<T>();
        result.insert(result.end(), childComponents.begin(), childComponents.end());
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
