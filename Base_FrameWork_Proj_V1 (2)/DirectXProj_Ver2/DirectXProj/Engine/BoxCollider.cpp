#include "BoxCollider.h"
#include "GameObject.h"
#include "Transform.h"

BoxCollider::BoxCollider(GameObject* owner)
    : Collider(owner, eColliderType::Box)
{
    // Initialize with default size, can be adjusted later
    //mBoundingBox.Center = owner->GetTransform()->GetPosition();
    DirectX::XMStoreFloat3( &mBoundingBox.Center, owner->GetTransform()->GetPosition());
    mBoundingBox.Extents = DirectX::SimpleMath::Vector3(0.5f, 0.5f, 0.5f);
}

void BoxCollider::Update()
{
    // Update the bounding box position with the transform
    //mBoundingBox.Center = GetOwner()->GetTransform()->GetPosition();
    DirectX::XMStoreFloat3(&mBoundingBox.Center, GetOwner()->GetTransform()->GetPosition());
}

bool BoxCollider::Intersects(const DirectX::SimpleMath::Ray& ray, float& distance)
{
    return mBoundingBox.Intersects(ray.position, ray.direction, distance);
}