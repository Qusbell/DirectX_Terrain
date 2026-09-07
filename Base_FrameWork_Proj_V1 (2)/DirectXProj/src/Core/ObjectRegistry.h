#pragma once
#include <cstdint>
#include <unordered_map>

// Forward declarations
class GameObject;
class Component;

class ObjectRegistry
{
public:
    static ObjectRegistry* GetInstance();

    ObjectRegistry(const ObjectRegistry&) = delete;
    ObjectRegistry& operator=(const ObjectRegistry&) = delete;

    void RegisterGameObject(GameObject* go);
    void UnregisterGameObject(uint64_t id);
    GameObject* FindGameObject(uint64_t id);

    void RegisterComponent(Component* comp);
    void UnregisterComponent(uint64_t id);
    Component* FindComponent(uint64_t id);

private:
    ObjectRegistry();
    ~ObjectRegistry();

    static ObjectRegistry* s_instance;

    std::unordered_map<uint64_t, GameObject*> m_gameObjectMap;
    std::unordered_map<uint64_t, Component*> m_componentMap;
};
