#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include <memory>

class Component;
class GameObject;

class ComponentFactory
{
public:
    using CreateComponentFunc = std::function<std::unique_ptr<Component>(GameObject*)>;

    static ComponentFactory* GetInstance();

    ComponentFactory(const ComponentFactory&) = delete;
    ComponentFactory& operator=(const ComponentFactory&) = delete;

    template<typename T>
    void Register(const std::string& name);

    std::unique_ptr<Component> Create(const std::string& name, GameObject* owner);

private:
    ComponentFactory();
    ~ComponentFactory();

    static ComponentFactory* s_instance;

    std::unordered_map<std::string, CreateComponentFunc> m_creators;
};

// Template implementation must be in the header
template<typename T>
void ComponentFactory::Register(const std::string& name)
{
    m_creators[name] = [](GameObject* owner) -> std::unique_ptr<Component> {
        // This is tricky because AddComponent returns a raw pointer and expects a unique_ptr
        // The factory should create the component, but the GameObject should own it.
        // Let's rethink. The factory should just create it, and the GameObject::AddComponent will take ownership.
        // But we can't do that with the deferred system.

        // The factory will create the component, and the caller (Scene::Load) will add it.
        // The problem is constructor arguments for components like SpriteRenderer.
        // The FromJson method will handle setting the properties.
        return std::make_unique<T>(owner);
    };
}
