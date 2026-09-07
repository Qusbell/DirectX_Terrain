#include "../Core/stdafx.h"
#include "InputComponent.h"
#include "../Input/InputManager.h"
#include "GameObject.h"
#include "Transform.h"
#include "../Core/TimeManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include <DirectXCollision.h>
#include <algorithm> // For std::clamp
#include <cmath> // For sin, asin, atan2
#include <Windows.h> // For OutputDebugStringA
#include <cfloat> // For FLT_MAX
#include "Mesh.h"
#include "CollisionManager.h"
#include "PickingManager.h"

InputComponent::InputComponent(GameObject* owner)
    : Component(owner), m_lastMousePosition{ 0.0f, 0.0f }, m_yaw(0.0f), m_pitch(0.0f)
{
}

void InputComponent::Start()
{
    // Initialize last mouse position
    m_Input = InputManager::GetInstance();
    InputManager* input = InputManager::GetInstance();
    m_lastMousePosition.x = static_cast<float>(m_Input->GetMouseX());
    m_lastMousePosition.y = static_cast<float>(m_Input->GetMouseY());

    

    // TODO: Initialize yaw and pitch from the transform's initial rotation
    // This is a bit complex, so for now we assume we start with no rotation.
    // A more robust solution would be to decompose the initial quaternion.
}

void InputComponent::Update()
{
    //InputManager* input = InputManager::GetInstance();
    Transform* transform = GetOwner()->GetTransform();
    float deltaTime = TimeManager::GetInstance()->GetDeltaTime();

    // --- Mouse position ---
    float currentMouseX = static_cast<float>(m_Input->GetMouseX());
    float currentMouseY = static_cast<float>(m_Input->GetMouseY());

    float deltaX = currentMouseX - m_lastMousePosition.x;
    float deltaY = currentMouseY - m_lastMousePosition.y;



    // --- Right-click to rotate (FPS-style) ---
    if (m_Input->IsKeyPressed(VK_RBUTTON))
    {
        m_yaw += deltaX * m_rotationSpeed;
        m_pitch += deltaY * m_rotationSpeed;

        // Clamp pitch to prevent flipping upside down
        m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);

        DirectX::XMVECTOR rotation = DirectX::XMQuaternionRotationRollPitchYaw(
            DirectX::XMConvertToRadians(m_pitch),
            DirectX::XMConvertToRadians(m_yaw),
            0
        );
        transform->SetLocalRotation(rotation);
    }

    // --- Middle-click to Pan ---
    if (m_Input->IsKeyPressed(VK_MBUTTON))
    {
        // Panning moves the camera opposite to the mouse direction
        DirectX::XMVECTOR right = transform->GetRight();
        DirectX::XMVECTOR up = transform->GetUp();

        float panSpeed = 0.005f; // Sensitivity for panning
        DirectX::XMVECTOR offset = DirectX::XMVectorScale(right, -deltaX * m_moveSpeed * panSpeed);
        offset = DirectX::XMVectorAdd(offset, DirectX::XMVectorScale(up, deltaY * m_moveSpeed * panSpeed));

        transform->SetLocalPosition(DirectX::XMVectorAdd(transform->GetLocalPosition(), offset));
    }

    // --- Mouse Wheel to Zoom ---
    int wheelDelta = m_Input->GetMouseWheelDelta();
    if (wheelDelta != 0)
    {
        DirectX::XMVECTOR forward = transform->GetForward();
        // Normalize the delta (standard is 120 per notch) and apply speed
        float zoomAmount = static_cast<float>(wheelDelta) / WHEEL_DELTA;
        zoomAmount = zoomAmount * m_zoomSpeed;
        if (m_Input->IsKeyPressed(VK_SHIFT))
            zoomAmount *= m_ShiftzoomSpeed;

        DirectX::XMVECTOR offset = DirectX::XMVectorScale(forward, zoomAmount);
        transform->SetLocalPosition(DirectX::XMVectorAdd(transform->GetLocalPosition(), offset));

        m_CameraRootDistance = DirectX::XMVector3Length(offset).m128_f32[0];
    }

    // --- Alt + Left-click to Orbit ---
    else if (m_Input->IsKeyPressed(VK_MENU) && m_Input->IsKeyPressed(VK_LBUTTON))
    {
        // 1. Define pivot point 100 units in front of the camera
        DirectX::XMVECTOR position = transform->GetPosition();
        DirectX::XMVECTOR forward = transform->GetForward();
        DirectX::XMVECTOR pivot = DirectX::XMVectorAdd(position, DirectX::XMVectorScale(forward, m_CameraRootDistance));

        // 2. Calculate Y-axis rotation based on horizontal mouse delta
        float orbitSpeed = m_rotationSpeed;
        DirectX::XMVECTOR yawRotation = DirectX::XMQuaternionRotationAxis(
            DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), // World Y-axis
            DirectX::XMConvertToRadians(-deltaX * orbitSpeed)
        );

        // 3. Rotate the camera's position vector around the new pivot
        DirectX::XMVECTOR pivotToCam = DirectX::XMVectorSubtract(position, pivot);
        pivotToCam = DirectX::XMVector3Rotate(pivotToCam, yawRotation);
        DirectX::XMVECTOR newPosition = DirectX::XMVectorAdd(pivot, pivotToCam);
        transform->SetLocalPosition(newPosition);

        // 4. Update rotation to always look at the pivot
        DirectX::XMVECTOR worldUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH(newPosition, pivot, worldUp);
        DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixInverse(nullptr, viewMatrix);
        DirectX::XMVECTOR scale, rot, pos;
        DirectX::XMMatrixDecompose(&scale, &rot, &pos, worldMatrix);
        transform->SetLocalRotation(rot);

        // 5. Update internal yaw and pitch to prevent snapping when switching modes
        DirectX::XMVECTOR newForward = DirectX::XMVector3Rotate(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), rot);
        m_pitch = DirectX::XMConvertToDegrees(asinf(-DirectX::XMVectorGetY(newForward)));
        m_yaw = DirectX::XMConvertToDegrees(atan2f(DirectX::XMVectorGetX(newForward), DirectX::XMVectorGetZ(newForward)));
    }

    // --- Left-click to Select Object (Picking) ---
    if (m_Input->IsKeyDown(VK_LBUTTON) )
    {
        // 1. Get required data
        Camera* camera = GetOwner()->GetComponent<Camera>();
        if (camera)
        {
            float screenX = (float)m_Input->GetMouseX();
            float screenY = (float)m_Input->GetMouseY();
            int screenW = m_Input->GetScreenWidth();
            int screenH = m_Input->GetScreenHeight();

            // 2. Create Ray from mouse coordinates
            DirectX::SimpleMath::Vector2 screenPos(screenX, screenY);

            // 3. Intersect with objects using CollisionManager
            RaycastHit hitInfo;
            if (CollisionManager::GetI()->Raycast(screenPos, hitInfo, camera, true)) // Precise check enabled
            {
                char buffer[256];
                sprintf_s(buffer, "Selected: %s\n - Point: (%.2f, %.2f, %.2f)\n - Normal: (%.2f, %.2f, %.2f)\n - UV: (%.2f, %.2f)\n - Tri Idx: %u\n",
                    hitInfo.gameObject->GetName().c_str(),
                    hitInfo.point.x, hitInfo.point.y, hitInfo.point.z,
                    hitInfo.normal.x, hitInfo.normal.y, hitInfo.normal.z,
                    hitInfo.uv.x, hitInfo.uv.y,
                    hitInfo.triangleIndex);
                OutputDebugStringA(buffer);
            }
            else
            {
                OutputDebugStringA("Selected nothing.\n");
            }
        }
    }


    // Update last mouse position for the next frame
    m_lastMousePosition = { currentMouseX, currentMouseY };

    // --- Debug Controls ---
    if (m_Input->IsKeyDown(VK_F1))
    {
        PickingManager::GetI()->ToggleDebugThumbnail();
    }
    if (m_Input->IsKeyDown('P'))
    {
        PickingManager::GetI()->SavePickingTextureToFile(L"debug_picking_buffer.png");
    }


    UpdateKeyboard();
}

void InputComponent::UpdateKeyboard()
{
    //InputManager* input = InputManager::GetInstance();
    Transform* transform = GetOwner()->GetTransform();
    float deltaTime = TimeManager::GetInstance()->GetDeltaTime();


    float deltaX = 0.f;
    float deltaY = 0.f;

    bool isflag = false;

    if ( m_Input->IsKeyPressed(VK_RIGHT)
        || m_Input->IsKeyPressed('D')
        || m_Input->IsKeyPressed('d')
        )
    {
        deltaX = 1.f;
        isflag = true;
    }
    if (m_Input->IsKeyPressed(VK_LEFT)
        || m_Input->IsKeyPressed('A')
        || m_Input->IsKeyPressed('a'))
    {
        deltaX = -1.f;
        isflag = true;
    }
    if (m_Input->IsKeyPressed(VK_UP)
        || m_Input->IsKeyPressed('W')
        || m_Input->IsKeyPressed('w'))
    {
        deltaY = 1.f;
        isflag = true;
    }
    if (m_Input->IsKeyPressed(VK_DOWN) 
        || m_Input->IsKeyPressed('S')
        || m_Input->IsKeyPressed('s'))
    {
        deltaY = -1.f;
        isflag = true;
    }

    if (m_Input->IsKeyPressed(VK_SHIFT))
    {
        deltaX *= m_ShiftWeightVal;
        deltaY *= m_ShiftWeightVal;
    }


    if (isflag)
    {
        // Panning moves the camera opposite to the mouse direction
        DirectX::XMVECTOR right = transform->GetRight();
        DirectX::XMVECTOR up = transform->GetUp();

        float panSpeed = 0.005f; // Sensitivity for panning
        DirectX::XMVECTOR offset = DirectX::XMVectorScale(right, deltaX * m_moveSpeed * panSpeed);
        offset = DirectX::XMVectorAdd(offset, DirectX::XMVectorScale(up, deltaY * m_moveSpeed * panSpeed));

        transform->SetLocalPosition(DirectX::XMVectorAdd(transform->GetLocalPosition(), offset));
    }

    
}
