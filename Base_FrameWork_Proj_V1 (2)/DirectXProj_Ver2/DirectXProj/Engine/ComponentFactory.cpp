#include "ComponentFactory.h"

ComponentFactory* ComponentFactory::s_instance = nullptr;

ComponentFactory* ComponentFactory::GetInstance()
{
    if (s_instance == nullptr)
    {
        s_instance = new ComponentFactory();
    }
    return s_instance;
}

ComponentFactory::ComponentFactory() {}
ComponentFactory::~ComponentFactory() {}

std::unique_ptr<Component> ComponentFactory::Create(const std::string& name, GameObject* owner)
{
    auto it = m_creators.find(name);
    if (it != m_creators.end())
    {
        return it->second(owner); // Call the creation lambda
    }
    return nullptr;
}
