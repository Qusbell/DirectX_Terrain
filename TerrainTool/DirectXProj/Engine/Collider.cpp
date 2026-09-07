#include "Collider.h"
#include "CollisionManager.h"
#include "GameObject.h"

Collider::Collider(GameObject* owner, eColliderType type)
    : Component(owner), mType(type)
{
    CollisionManager::GetI()->AddCollider(this);
}

Collider::~Collider()
{
    CollisionManager::GetI()->RemoveCollider(this);
}