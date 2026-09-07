#include "ObjectRegistry.h"
#include "../Engine/GameObject.h"
#include "../Engine/Component.h"

ObjectRegistry* ObjectRegistry::s_instance = nullptr;

ObjectRegistry* ObjectRegistry::GetInstance()
{
    if (s_instance == nullptr)
    {
        s_instance = new ObjectRegistry();
    }
    return s_instance;
}

ObjectRegistry::ObjectRegistry() {}
ObjectRegistry::~ObjectRegistry() {}

void ObjectRegistry::RegisterGameObject(GameObject* go)
{
    if (go)
    {
        m_gameObjectMap[go->GetId()] = go;
    }
}

void ObjectRegistry::UnregisterGameObject(uint64_t id)
{
    m_gameObjectMap.erase(id);
}

GameObject* ObjectRegistry::FindGameObject(uint64_t id)
{
    auto it = m_gameObjectMap.find(id);
    return (it != m_gameObjectMap.end()) ? it->second : nullptr;
}

void ObjectRegistry::RegisterComponent(Component* comp)
{
    if (comp)
    {
        m_componentMap[comp->GetId()] = comp;
    }
}

void ObjectRegistry::UnregisterComponent(uint64_t id)
{
    m_componentMap.erase(id);
}

Component* ObjectRegistry::FindComponent(uint64_t id)
{
    auto it = m_componentMap.find(id);
    return (it != m_componentMap.end()) ? it->second : nullptr;
}
