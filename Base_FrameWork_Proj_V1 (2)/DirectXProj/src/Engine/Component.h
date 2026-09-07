#pragma once
#include <cstdint>
#include "../Utils/IdGenerator.h"
#include "../Core/ObjectRegistry.h"
#include "../Utils/json.hpp"

using json = nlohmann::json;

class GameObject; // Forward declaration

class Component
{
public:
    Component(GameObject* owner);
    Component(GameObject* owner, uint64_t id);

    virtual ~Component()
    {
        ObjectRegistry::GetInstance()->UnregisterComponent(m_id);
    }

    virtual void Start() {}
    virtual void Update() {}
    virtual void Render() {}
    virtual void OnDestroy() {}

    virtual void ToJson(json& j) const;
    virtual void FromJson(const json& j);

    GameObject* GetOwner() const { return m_owner; }
    uint64_t GetId() const { return m_id; }

protected:
    GameObject* m_owner;
    const uint64_t m_id;
};
