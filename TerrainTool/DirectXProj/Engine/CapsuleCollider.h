#pragma once
#include "Collider.h"

class CapsuleCollider : public Collider
{
public:
    CapsuleCollider(GameObject* owner);

    void Update() override;
    bool Intersects(const DirectX::SimpleMath::Ray& ray, float& distance) override;

    // Capsule is represented by two points and a radius
    DirectX::SimpleMath::Vector3 GetPointA() const { return mPointA; }
    DirectX::SimpleMath::Vector3 GetPointB() const { return mPointB; }
    float GetRadius() const { return mRadius; }

private:
    // For simplicity, we'll align the capsule with the Y-axis for now
    DirectX::SimpleMath::Vector3 mPointA;
    DirectX::SimpleMath::Vector3 mPointB;
    float mRadius;
    float mHeight;
};