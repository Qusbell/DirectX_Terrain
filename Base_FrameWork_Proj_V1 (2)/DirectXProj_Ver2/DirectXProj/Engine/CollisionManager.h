#pragma once
#include <vector>
#include <DirectXMath.h>
#include "../Core/SingletonT.h"
#include <DirectXCollision.h>
#include "SimpleMath.h"


class Collider;
class GameObject;

struct RaycastHit
{
    GameObject* gameObject = nullptr;
    float distance = 0.0f;
    DirectX::SimpleMath::Vector3 point;
    DirectX::SimpleMath::Vector3 normal;
    DirectX::SimpleMath::Vector2 uv;
    unsigned int triangleIndex = -1;
};

class CollisionManager : public SingletonT<CollisionManager>
{
public:
    void AddCollider(Collider* collider);
    void RemoveCollider(Collider* collider);

    bool Raycast(const DirectX::SimpleMath::Ray& ray, RaycastHit& hitInfo, bool preciseCheck = false);
    bool Raycast(const DirectX::SimpleMath::Vector2& screenPos, RaycastHit& hitInfo, class Camera* camera, bool preciseCheck = false);


private:
    friend class SingletonT<CollisionManager>;
    CollisionManager() = default;
    ~CollisionManager() = default;

    std::vector<Collider*> mColliders;
};