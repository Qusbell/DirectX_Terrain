#pragma once
#include "Collider.h"

class SphereCollider : public Collider
{
public:
    SphereCollider(GameObject* owner);

    void Update() override;
    bool Intersects(const DirectX::SimpleMath::Ray& ray, float& distance) override;

    const DirectX::BoundingSphere& GetBoundingSphere() const { return mBoundingSphere; }

private:
    DirectX::BoundingSphere mBoundingSphere;
};