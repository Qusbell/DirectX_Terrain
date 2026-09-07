#define NOMINMAX // std::max Error NotUse #include <windows.h> or DirectX header up position
#include "CapsuleCollider.h"
#include "GameObject.h"
#include "Transform.h"

CapsuleCollider::CapsuleCollider(GameObject* owner)
    : Collider(owner, eColliderType::Capsule), mRadius(0.5f), mHeight(2.0f)
{
    Update();
}

void CapsuleCollider::Update()
{
    const DirectX::SimpleMath::Vector3& pos = GetOwner()->GetTransform()->GetPosition();
    float halfHeight = mHeight / 2.0f;
    mPointA = pos + DirectX::SimpleMath::Vector3(0, halfHeight - mRadius, 0);
    mPointB = pos - DirectX::SimpleMath::Vector3(0, halfHeight - mRadius, 0);
}

// Helper function to find the closest point on a line segment to a given point.
static DirectX::SimpleMath::Vector3 ClosestPointOnLineSegment(const DirectX::SimpleMath::Vector3& p, const DirectX::SimpleMath::Vector3& a, const DirectX::SimpleMath::Vector3& b) {
    DirectX::SimpleMath::Vector3 ab = b - a;
    float t = (p - a).Dot(ab) / ab.LengthSquared();
    t = std::max(0.0f, std::min(1.0f, t)); // Clamp t to the range [0, 1]
    return a + t * ab;
}

// Ray-capsule intersection test.
bool CapsuleCollider::Intersects(const DirectX::SimpleMath::Ray& ray, float& distance)
{
    using namespace DirectX::SimpleMath;

    Vector3 cap_axis = mPointB - mPointA;
    Vector3 ray_origin_to_cap_start = mPointA - ray.position;

    float a = cap_axis.Dot(cap_axis);
    float b = cap_axis.Dot(ray.direction);
    float c = cap_axis.Dot(ray_origin_to_cap_start);
    float d = ray.direction.Dot(ray.direction);
    float e = ray.direction.Dot(ray_origin_to_cap_start);

    float denominator = a * d - b * b;
    float t_ray, t_capsule;

    if (std::abs(denominator) < 1e-6f) { // Parallel lines
        t_capsule = 0.0f;
        t_ray = -e / d;
    }
    else {
        t_capsule = (b * e - c * d) / denominator;
        t_ray = (a * e - b * c) / denominator;
    }

    // Clamp the intersection point to be on the capsule's line segment
    t_capsule = std::max(0.0f, std::min(1.0f, t_capsule));

    Vector3 closest_point_on_ray = ray.position + t_ray * ray.direction;
    Vector3 closest_point_on_capsule_axis = mPointA + t_capsule * cap_axis;

    float dist_sq = (closest_point_on_ray - closest_point_on_capsule_axis).LengthSquared();

    if (dist_sq <= mRadius * mRadius) {
        // Intersection exists. Now find the correct distance along the ray.
        Vector3 oc = ray.position - closest_point_on_capsule_axis;
        float l = ray.direction.Dot(oc);
        float delta = l * l - oc.LengthSquared() + mRadius * mRadius;

        if (delta >= 0) {
            distance = -l - sqrt(delta);
            return distance >= 0;
        }
    }

    return false;
}