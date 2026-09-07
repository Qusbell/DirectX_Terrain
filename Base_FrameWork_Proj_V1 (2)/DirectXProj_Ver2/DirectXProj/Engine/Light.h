#pragma once
#include "Component.h"
#include <DirectXMath.h>

class Light : public Component
{
public:
    enum class LightType
    {
        Directional,
        Point,
        Spot
    };

    Light(GameObject* owner);
    ~Light() override;

    // Getters
    LightType GetType() const { return m_type; }
    const DirectX::XMFLOAT4& GetColor() const { return m_color; }
    float GetIntensity() const { return m_intensity; }

    // Setters
    void SetType(LightType type);
    void SetColor(const DirectX::XMFLOAT4& color);
    void SetIntensity(float intensity);

private:
    LightType m_type;
    DirectX::XMFLOAT4 m_color;
    float m_intensity;
};
