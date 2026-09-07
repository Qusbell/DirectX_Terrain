#include "CollisionManager.h"
#include "Collider.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "Transform.h"
#include "PickingManager.h"
#include "Camera.h"
#include "../Core/CoreGraphicsManager.h"
#include "../Input/InputManager.h"
#include <algorithm>

namespace
{
    // Moller-Trumbore intersection algorithm
    bool RayTriangleIntersect(
        const DirectX::SimpleMath::Vector3& rayOrigin,
        const DirectX::SimpleMath::Vector3& rayVector,
        const DirectX::SimpleMath::Vector3& vertex0,
        const DirectX::SimpleMath::Vector3& vertex1,
        const DirectX::SimpleMath::Vector3& vertex2,
        float& t,
        float& u,
        float& v)
    {
        const float Epsilon = 1e-8f;
        DirectX::SimpleMath::Vector3 edge1 = vertex1 - vertex0;
        DirectX::SimpleMath::Vector3 edge2 = vertex2 - vertex0;
        DirectX::SimpleMath::Vector3 h = rayVector.Cross(edge2);
        float a = edge1.Dot(h);

        if (a > -Epsilon && a < Epsilon)
            return false; // This ray is parallel to this triangle.

        float f = 1.0f / a;
        DirectX::SimpleMath::Vector3 s = rayOrigin - vertex0;
        u = f * s.Dot(h);

        if (u < 0.0f || u > 1.0f)
            return false;

        DirectX::SimpleMath::Vector3 q = s.Cross(edge1);
        v = f * rayVector.Dot(q);

        if (v < 0.0f || u + v > 1.0f)
            return false;

        // At this stage we can compute t to find out where the intersection point is on the line.
        t = f * edge2.Dot(q);

        return t > Epsilon; // Ray intersection
    }
}

void CollisionManager::AddCollider(Collider* collider)
{
    mColliders.push_back(collider);
}

void CollisionManager::RemoveCollider(Collider* collider)
{
    mColliders.erase(std::remove(mColliders.begin(), mColliders.end(), collider), mColliders.end());
}

bool CollisionManager::Raycast(const DirectX::SimpleMath::Ray& ray, RaycastHit& hitInfo, bool preciseCheck)
{
    hitInfo.gameObject = nullptr;
    hitInfo.distance = FLT_MAX;

    GameObject* closestHitObject = nullptr;
    float closestDistance = FLT_MAX;

    // Broad phase: Check against all colliders
    for (Collider* collider : mColliders)
    {
        float distance;
        if (collider->GetOwner()->IsActive() && collider->Intersects(ray, distance))
        {
            if (distance < closestDistance)
            {
                closestDistance = distance;
                closestHitObject = collider->GetOwner();
            }
        }
    }

    if (!closestHitObject)
    {
        return false;
    }

    // If precise checking is not needed, we're done.
    if (!preciseCheck)
    {
        hitInfo.gameObject = closestHitObject;
        hitInfo.distance = closestDistance;
        hitInfo.point = ray.position + ray.direction * closestDistance;
        // Other fields in hitInfo remain default
        return true;
    }

    // Narrow phase: Triangle-level intersection for the closest object
    MeshRenderer* meshRenderer = closestHitObject->GetComponent<MeshRenderer>();
    Mesh* mesh = meshRenderer ? meshRenderer->GetMeshPoint() : nullptr;

    if (!mesh || mesh->GetVertices().empty() || mesh->GetIndices().empty())
    {
        // No mesh to check, so return the broad phase result.
        hitInfo.gameObject = closestHitObject;
        hitInfo.distance = closestDistance;
        hitInfo.point = ray.position + ray.direction * closestDistance;
        return true;
    }

    // Transform ray to model's local space
	DirectX::SimpleMath::Matrix worldmatrix = closestHitObject->GetTransform()->GetWorldMatrix();
    DirectX::SimpleMath::Matrix worldInv = worldmatrix.Invert(); // closestHitObject->GetTransform()->GetWorldMatrix().Invert();
    DirectX::SimpleMath::Ray localRay;
    localRay.position = DirectX::SimpleMath::Vector3::Transform(ray.position, worldInv);
    localRay.direction = DirectX::SimpleMath::Vector3::TransformNormal(ray.direction, worldInv);
    localRay.direction.Normalize();

    const auto& vertices = mesh->GetVertices();
    const auto& indices = mesh->GetIndices();
    float best_t = FLT_MAX;
    unsigned int best_tri_idx = -1;
    float best_u = 0, best_v = 0;

    for (size_t i = 0; i < indices.size(); i += 3)
    {
        const auto& v0 = vertices[indices[i]].pos;
        const auto& v1 = vertices[indices[i + 1]].pos;
        const auto& v2 = vertices[indices[i + 2]].pos;

        float t, u, v;
        if (RayTriangleIntersect(localRay.position, localRay.direction, v0, v1, v2, t, u, v))
        {
            if (t < best_t)
            {
                best_t = t;
                best_u = u;
                best_v = v;
                best_tri_idx = i / 3;
            }
        }
    }

    if (best_tri_idx != -1)
    {
        // An intersection was found, fill hitInfo
        hitInfo.gameObject = closestHitObject;
        hitInfo.distance = best_t;
        hitInfo.point = ray.position + ray.direction * best_t;
        hitInfo.triangleIndex = best_tri_idx;

        // Interpolate normal and UVs
        const auto& i0 = indices[best_tri_idx * 3];
        const auto& i1 = indices[best_tri_idx * 3 + 1];
        const auto& i2 = indices[best_tri_idx * 3 + 2];

        const auto& vert0 = vertices[i0];
        const auto& vert1 = vertices[i1];
        const auto& vert2 = vertices[i2];

        float w = 1.0f - best_u - best_v;
        DirectX::SimpleMath::Vector3 localNormal = w * vert0.normal + best_u * vert1.normal + best_v * vert2.normal;
        hitInfo.normal = DirectX::SimpleMath::Vector3::TransformNormal(localNormal, closestHitObject->GetTransform()->GetWorldMatrix());
        hitInfo.normal.Normalize();

        hitInfo.uv = w * vert0.uv + best_u * vert1.uv + best_v * vert2.uv;

        return true;
    }

    // No precise intersection found, but broad phase hit something.
    // This can happen if the collider is larger than the mesh.
    // We'll return the broad phase result.
    hitInfo.gameObject = closestHitObject;
    hitInfo.distance = closestDistance;
    hitInfo.point = ray.position + ray.direction * closestDistance;
    return true;
}

bool CollisionManager::Raycast(const DirectX::SimpleMath::Vector2& screenPos, RaycastHit& hitInfo, Camera* camera, bool preciseCheck)
{
    if (!camera)
    {
        return false;
    }

    // Use PickingManager to get the GameObject
    GameObject* pickedObject = PickingManager::GetI()->Pick(screenPos);

    if (!pickedObject)
    {
        return false;
    }

    if (!preciseCheck)
    {
        hitInfo.gameObject = pickedObject;
        // Cannot determine distance or point without a ray, returning true with just the object.
        return true;
    }

    // Create a ray from the screen position
    DirectX::SimpleMath::Ray ray = camera->ScreenPointToRay(screenPos);

    // Now perform a precise raycast on this object only
    MeshRenderer* meshRenderer = pickedObject->GetComponent<MeshRenderer>();
    Mesh* mesh = meshRenderer ? meshRenderer->GetMeshPoint() : nullptr;

    if (!mesh || mesh->GetVertices().empty() || mesh->GetIndices().empty())
    {
        // No mesh to check, but we did hit the object's collider (conceptually)
        hitInfo.gameObject = pickedObject;
        return true;
    }

    // Transform ray to model's local space
	DirectX::SimpleMath::Matrix worldmat = pickedObject->GetTransform( )->GetWorldMatrix( );
    DirectX::SimpleMath::Matrix worldInv = worldmat.Invert();
    DirectX::SimpleMath::Ray localRay;
    localRay.position = DirectX::SimpleMath::Vector3::Transform(ray.position, worldInv);
    localRay.direction = DirectX::SimpleMath::Vector3::TransformNormal(ray.direction, worldInv);
    localRay.direction.Normalize();

    const auto& vertices = mesh->GetVertices();
    const auto& indices = mesh->GetIndices();
    float best_t = FLT_MAX;
    unsigned int best_tri_idx = -1;
    float best_u = 0, best_v = 0;

    for (size_t i = 0; i < indices.size(); i += 3)
    {
        const auto& v0 = vertices[indices[i]].pos;
        const auto& v1 = vertices[indices[i + 1]].pos;
        const auto& v2 = vertices[indices[i + 2]].pos;

        float t, u, v;
        if (RayTriangleIntersect(localRay.position, localRay.direction, v0, v1, v2, t, u, v))
        {
            if (t < best_t)
            {
                best_t = t;
                best_u = u;
                best_v = v;
                best_tri_idx = i / 3;
            }
        }
    }

    if (best_tri_idx != -1)
    {
        hitInfo.gameObject = pickedObject;
        hitInfo.distance = best_t;
        hitInfo.point = ray.position + ray.direction * best_t;
        hitInfo.triangleIndex = best_tri_idx;

        const auto& i0 = indices[best_tri_idx * 3];
        const auto& i1 = indices[best_tri_idx * 3 + 1];
        const auto& i2 = indices[best_tri_idx * 3 + 2];

        const auto& vert0 = vertices[i0];
        const auto& vert1 = vertices[i1];
        const auto& vert2 = vertices[i2];

        float w = 1.0f - best_u - best_v;
        DirectX::SimpleMath::Vector3 localNormal = w * vert0.normal + best_u * vert1.normal + best_v * vert2.normal;
        hitInfo.normal = DirectX::SimpleMath::Vector3::TransformNormal(localNormal, pickedObject->GetTransform()->GetWorldMatrix());
        hitInfo.normal.Normalize();
        hitInfo.uv = w * vert0.uv + best_u * vert1.uv + best_v * vert2.uv;
        return true;
    }

    return false;
}