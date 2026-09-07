#pragma once
#include "Collider.h"

class BoxCollider : public Collider
{
public:
    BoxCollider(GameObject* owner);

    void Update() override;
    bool Intersects(const DirectX::SimpleMath::Ray& ray, float& distance) override;

    const DirectX::BoundingBox& GetBoundingBox() const { return mBoundingBox; }

private:
    DirectX::BoundingBox mBoundingBox;
};