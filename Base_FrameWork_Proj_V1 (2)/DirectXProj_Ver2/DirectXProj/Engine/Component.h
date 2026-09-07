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

    virtual void Awake() {}
    virtual void Start() {}
    virtual void Update() {}
	virtual void PreRender(class Camera* camera) {}
    virtual void Render(class Camera* camera) {}
	virtual void PostRender(class Camera* camera) {}
	virtual void OnGUIRender(class Camera* camera) {}
    virtual void OnDestroy() {}

    virtual void ToJson(json& j) const;
    virtual void FromJson(const json& j);

    GameObject* GetOwner() const { return m_owner; }
    uint64_t GetId() const { return m_id; }

    bool HasStarted() const { return m_hasStarted; }
    void SetStarted(bool started) { m_hasStarted = started; }

protected:
    GameObject* m_owner;
    const uint64_t m_id;
    bool m_hasStarted;
};
