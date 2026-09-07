#include "SphereCollider.h"
#include "GameObject.h"
#include "Transform.h"

SphereCollider::SphereCollider(GameObject* owner)
    : Collider(owner, eColliderType::Sphere)
{
    //mBoundingSphere.Center = owner->GetTransform()->GetPosition();
    DirectX::XMStoreFloat3(&mBoundingSphere.Center, owner->GetTransform()->GetPosition());
    mBoundingSphere.Radius = 0.5f;
}

void SphereCollider::Update()
{
    //mBoundingSphere.Center = GetOwner()->GetTransform()->GetPosition();
    DirectX::XMStoreFloat3(&mBoundingSphere.Center, GetOwner()->GetTransform()->GetPosition());
}

bool SphereCollider::Intersects(const DirectX::SimpleMath::Ray& ray, float& distance)
{
    return mBoundingSphere.Intersects(ray.position, ray.direction, distance);
}