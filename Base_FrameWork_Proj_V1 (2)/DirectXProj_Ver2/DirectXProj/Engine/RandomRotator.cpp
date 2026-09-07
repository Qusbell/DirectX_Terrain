#include "../Core/stdafx.h"
#include "RandomRotator.h"
#include "GameObject.h"
#include "Transform.h"
#include "../Core/TimeManager.h"
#include <random>

namespace {
    // Helper to generate a random float in a range
    float RandomFloat(float min, float max) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(min, max);
        return dis(gen);
    }
}

RandomRotator::RandomRotator(GameObject* owner)
    : Component(owner)
{
}

void RandomRotator::Update()
{
    float deltaTime = TimeManager::GetInstance()->GetDeltaTime();

    // Rotate by a random amount between 45 and 135 degrees per second
    float rotationSpeed = RandomFloat(DirectX::XM_PIDIV4, DirectX::XM_PI * 3.0f / 4.0f);

    Transform* transform = GetOwner()->GetTransform();

    // Get current rotation
    DirectX::XMVECTOR currentRotation = transform->GetLocalRotation();

    // Create a rotation quaternion for this frame's rotation
    // We rotate around the Z-axis for 2D sprites
    DirectX::XMVECTOR rotationThisFrame = DirectX::XMQuaternionRotationAxis(
        DirectX::XMVectorSet(0.0f, 1.0f, 1.0f, 0.0f),
        rotationSpeed * deltaTime
    );

    // Combine the new rotation with the existing rotation
    DirectX::XMVECTOR newRotation = DirectX::XMQuaternionMultiply(currentRotation, rotationThisFrame);

    // Apply the new rotation
    transform->SetLocalRotation(newRotation);
}
