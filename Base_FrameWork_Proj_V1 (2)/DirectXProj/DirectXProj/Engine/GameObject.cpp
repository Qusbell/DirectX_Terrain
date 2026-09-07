#include "GameObject.h"
#include <algorithm>
#include "../Utils/IdGenerator.h"
#include "../Core/ObjectRegistry.h"
#include "ComponentFactory.h"
#include "Component.h"


GameObject::GameObject(const std::string& name)
    : m_id(IdGenerator::GenerateId()), m_name(name), m_parent(nullptr), m_isActive(true)
{
    ObjectRegistry::GetInstance()->RegisterGameObject(this);
    // Every GameObject has a Transform component by default.
    m_transform = std::make_unique<Transform>(this);
}

GameObject::GameObject(uint64_t id, const std::string& name)
    : m_id(id), m_name(name), m_parent(nullptr), m_isActive(true)
{
    ObjectRegistry::GetInstance()->RegisterGameObject(this);
    m_transform = std::make_unique<Transform>(this);
}

GameObject::~GameObject()
{
    ObjectRegistry::GetInstance()->UnregisterGameObject(m_id);
    // Components and children are owned by unique_ptr in Scene/GameObject,
    // so they will be cleaned up automatically.
    // Detach from parent upon destruction.
    if (m_parent)
    {
        m_parent->RemoveChild(this);
    }
}

void GameObject::Start()
{
    // Start self
    m_transform->Start();
    for (auto const& [type, comp_vec] : m_components)
    {
        for (const auto& component : comp_vec)
        {
            component->Start();
        }
    }

    // Start children
    for (auto* child : m_children)
    {
        child->Start();
    }
}

void GameObject::Update()
{
    if (!m_isActive) return;

    // Update self
    m_transform->Update();
    for (auto const& [type, comp_vec] : m_components)
    {
        for (const auto& component : comp_vec)
        {
            if (std::find(m_pendingRemoveComponents.begin(), m_pendingRemoveComponents.end(), component.get()) == m_pendingRemoveComponents.end())
            {
                component->Update();
            }
        }
    }

    // Update children
    for (auto* child : m_children)
    {
        child->Update();
    }
}

void GameObject::Render()
{
    if (!m_isActive) return;

    // Render self
    m_transform->Render();
    for (auto const& [type, comp_vec] : m_components)
    {
        for (const auto& component : comp_vec)
        {
            if (std::find(m_pendingRemoveComponents.begin(), m_pendingRemoveComponents.end(), component.get()) == m_pendingRemoveComponents.end())
            {
                component->Render();
            }
        }
    }

    // Render children
    for (auto* child : m_children)
    {
        child->Render();
    }
}

void GameObject::ProcessPendingChanges()
{
    // Remove components marked for deletion
    if (!m_pendingRemoveComponents.empty())
    {
        for (auto* compToRemove : m_pendingRemoveComponents)
        {
            const std::type_index type = typeid(*compToRemove);
            if (m_components.count(type))
            {
                auto& comp_vec = m_components.at(type);
                comp_vec.erase(
                    std::remove_if(comp_vec.begin(), comp_vec.end(),
                        [&](const auto& comp) { return comp.get() == compToRemove; }),
                    comp_vec.end());

                if (comp_vec.empty())
                {
                    m_components.erase(type);
                }
            }
        }
        m_pendingRemoveComponents.clear();
    }

    // Add new components
    if (!m_pendingAddComponents.empty())
    {
        for (auto& newComp : m_pendingAddComponents)
        {
            newComp->Start();
            const std::type_index type = typeid(*newComp);
            m_components[type].push_back(std::move(newComp));
        }
        m_pendingAddComponents.clear();
    }

    // Process children
    for (auto* child : m_children)
    {
        child->ProcessPendingChanges();
    }
}

void GameObject::Destroy()
{
    m_isActive = false;
    // Also mark all children for destruction
    for (auto* child : m_children)
    {
        child->Destroy();
    }
}

void GameObject::ToJson(json& j) const
{
    j["id"] = GetId();
    j["name"] = GetName();
    j["parent_id"] = (GetParent() ? GetParent()->GetId() : 0); // 0 indicates no parent

    json components_json = json::array();

    // Serialize transform separately as it's special
    json transform_json;
    GetTransform()->ToJson(transform_json);
    components_json.push_back(transform_json);

    // Serialize other components
    for (const auto& pair : m_components)
    {
        for (const auto& comp : pair.second)
        {
            json component_json;
            comp->ToJson(component_json);
            components_json.push_back(component_json);
        }
    }
    j["components"] = components_json;
}

void GameObject::FromJson(const json& j, Scene* scene)
{
    // Name is read-only after construction, but we can set it here
    m_name = j["name"];

    if (j.contains("components"))
    {
        for (const auto& comp_json : j["components"])
        {
            std::string type_name = comp_json["type"];

            // Handle Transform separately
            if (type_name == typeid(Transform).name())
            {
                m_transform->FromJson(comp_json);
                continue;
            }

            // Create other components with the factory
            auto new_comp = ComponentFactory::GetInstance()->Create(type_name, this);
            if (new_comp)
            {
                new_comp->FromJson(comp_json);
                // Add component directly, bypassing deferred system for loading
                const std::type_index type = typeid(*new_comp);
                m_components[type].push_back(std::move(new_comp));
            }
        }
    }
}

void GameObject::OnDestroy()
{
    // Destroy self
    m_transform->OnDestroy();
    for (const auto& component : m_components)
    {
        //component->OnDestroy();
        for(const auto& comp : component.second)
        {
            comp->OnDestroy();
		}
    }

    // Destroy children
    for (auto* child : m_children)
    {
        child->OnDestroy();
    }
}

void GameObject::SetParent(GameObject* parent)
{
    // --- Reparenting logic to keep world transform constant ---
    // 1. Get current world matrix
    Transform* myTransform = GetTransform();
    DirectX::XMMATRIX worldMatrix = myTransform->GetWorldMatrix();

    SetParentRaw(parent);

    // If we have a new parent, attach to it
    if (m_parent)
    {
        // 2. Get inverse of new parent's world matrix
        DirectX::XMMATRIX parentWorldInverse = DirectX::XMMatrixInverse(nullptr, m_parent->GetTransform()->GetWorldMatrix());

        // 3. Calculate new local matrix
        DirectX::XMMATRIX newLocalMatrix = worldMatrix * parentWorldInverse;

        // 4. Decompose and set new local transform
        DirectX::XMVECTOR scale, rot, pos;
        DirectX::XMMatrixDecompose(&scale, &rot, &pos, newLocalMatrix);
        myTransform->SetLocalPosition(pos);
        myTransform->SetLocalRotation(rot);
        myTransform->SetLocalScale(scale);
    }
    else // If we are becoming a root object
    {
        // Our new local transform is just our old world transform
        DirectX::XMVECTOR scale, rot, pos;
        DirectX::XMMatrixDecompose(&scale, &rot, &pos, worldMatrix);
        myTransform->SetLocalPosition(pos);
        myTransform->SetLocalRotation(rot);
        myTransform->SetLocalScale(scale);
    }

    // Changing parent always dirties the transform
    myTransform->SetDirty();
}

void GameObject::SetParentRaw(GameObject* parent)
{
    // If we have an old parent, detach from it
    if (m_parent)
    {
        m_parent->RemoveChild(this);
    }

    m_parent = parent;

    // If we have a new parent, attach to it
    if (m_parent)
    {
        m_parent->AddChild(this);
    }
}

void GameObject::AddChild(GameObject* child)
{
    m_children.push_back(child);
}

void GameObject::RemoveChild(GameObject* child)
{
    m_children.erase(std::remove(m_children.begin(), m_children.end(), child), m_children.end());
}
