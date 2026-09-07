#include "../Core/stdafx.h"
#include "Light.h"
#include "LightManager.h"

Light::Light(GameObject* owner)
    : Component(owner),
    m_type(LightType::Directional),
    m_color({ 1.0f, 1.0f, 1.0f, 1.0f }), // Default white light
    m_intensity(1.0f)
{
    LightManager::GetInstance()->RegisterLight(this);
}

Light::~Light()
{
    LightManager::GetInstance()->UnregisterLight(this);
}

void Light::SetType(LightType type)
{
    m_type = type;
}

void Light::SetColor(const DirectX::XMFLOAT4& color)
{
    m_color = color;
}

void Light::SetIntensity(float intensity)
{
    m_intensity = intensity;
}
