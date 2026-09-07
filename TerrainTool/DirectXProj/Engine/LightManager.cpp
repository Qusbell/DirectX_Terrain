#include "../Core/stdafx.h"
#include "LightManager.h"
#include <algorithm>

LightManager* LightManager::GetInstance()
{
    static LightManager instance;
    return &instance;
}

void LightManager::RegisterLight(Light* light)
{
    m_lights.push_back(light);
}

void LightManager::UnregisterLight(Light* light)
{
    m_lights.erase(std::remove(m_lights.begin(), m_lights.end(), light), m_lights.end());
}

const std::vector<Light*>& LightManager::GetLights() const
{
    return m_lights;
}
