#pragma once
#include <vector>
#include "Light.h"

class LightManager
{
public:
    static LightManager* GetInstance();

    void RegisterLight(Light* light);
    void UnregisterLight(Light* light);
    const std::vector<Light*>& GetLights() const;

private:
    LightManager() = default;
    ~LightManager() = default;
    LightManager(const LightManager&) = delete;
    LightManager& operator=(const LightManager&) = delete;

    std::vector<Light*> m_lights;
};
