#pragma once
#include "Component.h"
#include <DirectXCollision.h>
#include "SimpleMath.h"

enum class eColliderType
{
    None,
    Box,
    Sphere,
    Capsule
};

class Collider : public Component
{
public:
    Collider(GameObject* owner, eColliderType type);
    ~Collider() override;

    virtual bool Intersects(const DirectX::SimpleMath::Ray& ray, float& distance) = 0;

    eColliderType GetType() const { return mType; }

protected:
    eColliderType mType;
};