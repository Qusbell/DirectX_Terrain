#include "Component.h"
#include <typeinfo>

Component::Component(GameObject* owner)
    : m_owner(owner), m_id(IdGenerator::GenerateId()), m_hasStarted(false)
{
    ObjectRegistry::GetInstance()->RegisterComponent(this);
}

Component::Component(GameObject* owner, uint64_t id)
    : m_owner(owner), m_id(id), m_hasStarted(false)
{
    ObjectRegistry::GetInstance()->RegisterComponent(this);
}

void Component::ToJson(json& j) const
{
    j["id"] = GetId();
    j["type"] = typeid(*this).name();
}

void Component::FromJson(const json& j)
{
    // Base implementation is empty. ID is set in constructor.
}
