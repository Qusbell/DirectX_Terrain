#include "../Core/stdafx.h"
#include "Camera.h"
#include "GameObject.h"
#include "Transform.h"
#include "../Input/InputManager.h"
#include <DirectXMath.h>

namespace {
    // Converts degrees to radians
    float ToRadians(float degrees)
    {
        return degrees * (DirectX::XM_PI / 180.0f);
    }

    // Calculates vertical FOV in radians from focal length and sensor height
    float FocalLengthToFov(float focalLength, float sensorHeight)
    {
        // Formula: fov = 2 * atan((0.5 * sensorHeight) / focalLength)
        if (focalLength <= 0 || sensorHeight <= 0) return ToRadians(60.0f); // Return a default FOV
        return 2.0f * atanf((0.5f * sensorHeight) / focalLength);
    }
}

Camera::Camera(GameObject* owner)
    : Component(owner),
      m_isProjectionDirty(true), // Start dirty to calculate initial projection
      m_projectionType(ProjectionType::Perspective),
      m_nearClipPlane(0.1f),
      m_farClipPlane(1000.0f),
      m_aspectRatio(16.0f / 9.0f),
      m_lensShift({0.0f, 0.0f}),
      m_fieldOfView(60.0f),
      m_focalLength(35.0f),
      m_sensorSize({36.0f, 24.0f}), // Corresponds to a full-frame sensor
      m_orthographicSize(5.0f),
      m_depth(0),
      m_viewportRect({0.0f, 0.0f, 1.0f, 1.0f}), // Fullscreen
      m_clearFlags(ClearFlags::SolidColor),
      m_backgroundColor({0.0f, 0.2f, 0.4f, 1.0f}) // Default blue
{
    m_viewMatrix = DirectX::XMMatrixIdentity();
    m_projectionMatrix = DirectX::XMMatrixIdentity();
}

void Camera::Update()
{
    // 1. Update view matrix (always, as transform can change every frame)
    Transform* transform = GetOwner()->GetTransform();
    DirectX::XMVECTOR eye = transform->GetPosition();
    //DirectX::XMVECTOR focus = eye + transform->GetForward();
    DirectX::XMVECTOR focus = DirectX::XMVectorAdd(eye, transform->GetForward());
    DirectX::XMVECTOR up = transform->GetUp();
    m_viewMatrix = DirectX::XMMatrixLookAtLH(eye, focus, up);



    // 2. Update projection matrix if properties have changed
    if (m_isProjectionDirty)
    {
        RecalculateProjectionMatrix();
        m_isProjectionDirty = false;
    }
}

void Camera::RecalculateProjectionMatrix()
{
    switch (m_projectionType)
    {
        case ProjectionType::Perspective:
        {
            // Use focal length to derive FOV if focal length is considered the primary property.
            // For now, we assume if the user sets FOV, they want that FOV.
            // A more robust system might have another enum for PerspectiveMode (FOV vs Physical).
            // Let's assume m_fieldOfView is the desired value in degrees.
            float fovRadians = ToRadians(m_fieldOfView);

            // If you wanted to strictly use physical properties, you would do this:
            // float fovRadians = FocalLengthToFov(m_focalLength, m_sensorSize.y);

            m_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fovRadians, m_aspectRatio, m_nearClipPlane, m_farClipPlane);
            break;
        }
        case ProjectionType::Orthographic:
        {
            float orthoHeight = m_orthographicSize;
            float orthoWidth = m_orthographicSize * m_aspectRatio;
            m_projectionMatrix = DirectX::XMMatrixOrthographicLH(orthoWidth, orthoHeight, m_nearClipPlane, m_farClipPlane);
            break;
        }
    }

    // Apply lens shift to the projection matrix
    // This is done by modifying the third row of the matrix
    DirectX::XMMATRIX shiftMatrix = DirectX::XMMatrixIdentity();
    shiftMatrix.r[2].m128_f32[0] = m_lensShift.x;
    shiftMatrix.r[2].m128_f32[1] = m_lensShift.y;

    m_projectionMatrix = DirectX::XMMatrixMultiply(m_projectionMatrix, shiftMatrix);
}


// --- Setters ---

void Camera::SetProjectionType(ProjectionType type)
{
    m_projectionType = type;
    m_isProjectionDirty = true;
}

void Camera::SetNearClipPlane(float nearPlane)
{
    m_nearClipPlane = nearPlane;
    m_isProjectionDirty = true;
}

void Camera::SetFarClipPlane(float farPlane)
{
    m_farClipPlane = farPlane;
    m_isProjectionDirty = true;
}

void Camera::SetAspectRatio(float aspectRatio)
{
    m_aspectRatio = aspectRatio;
    m_isProjectionDirty = true;
}

void Camera::SetLensShift(DirectX::XMFLOAT2 shift)
{
    m_lensShift = shift;
    m_isProjectionDirty = true;
}

void Camera::SetFieldOfView(float fov)
{
    m_fieldOfView = fov;
    m_isProjectionDirty = true;
}

void Camera::SetFocalLength(float length)
{
    m_focalLength = length;
    // When focal length is set, it should override the FOV
    m_fieldOfView = DirectX::XMConvertToDegrees(FocalLengthToFov(m_focalLength, m_sensorSize.y));
    m_isProjectionDirty = true;
}

void Camera::SetSensorSize(DirectX::XMFLOAT2 size)
{
    m_sensorSize = size;
    // When sensor size changes, it should also update the FOV if we are in physical mode
    m_fieldOfView = DirectX::XMConvertToDegrees(FocalLengthToFov(m_focalLength, m_sensorSize.y));
    m_isProjectionDirty = true;
}

void Camera::SetOrthographicSize(float size)
{
    m_orthographicSize = size;
    m_isProjectionDirty = true;
}

void Camera::SetDepth(int depth)
{
    m_depth = depth;
}

void Camera::SetViewportRect(const DirectX::XMFLOAT4& rect)
{
    m_viewportRect = rect;
}

void Camera::SetClearFlags(ClearFlags flags)
{
    m_clearFlags = flags;
}

void Camera::SetBackgroundColor(const DirectX::XMFLOAT4& color)
{
    m_backgroundColor = color;
}

DirectX::SimpleMath::Ray Camera::ScreenPointToRay(const DirectX::SimpleMath::Vector2& screenPos) const
{
    InputManager* inputManager = InputManager::GetInstance();
    int screenWidth = inputManager->GetScreenWidth();
    int screenHeight = inputManager->GetScreenHeight();


    // Convert screen coordinates to normalized device coordinates (NDC)
    float x = (2.0f * screenPos.x) / screenWidth - 1.0f;
    float y = 1.0f - (2.0f * screenPos.y) / screenHeight;

    // Create a ray in NDC space
    DirectX::XMVECTOR ray_nds = DirectX::XMVectorSet(x, y, 1.0f, 1.0f);

    // Unproject the ray from view/projection space to world space
    DirectX::XMMATRIX viewProjInv = DirectX::XMMatrixInverse(nullptr, m_viewMatrix * m_projectionMatrix);

    // Transform the ray origin and a point on the ray to world space
	DirectX::SimpleMath::Vector3 ray_origin = GetOwner( )->GetTransform( )->GetPosition( );
	DirectX::SimpleMath::Vector3 ray_end_ws = DirectX::XMVector3TransformCoord(ray_nds, viewProjInv);

    // The ray direction is from the camera to the unprojected point
    DirectX::XMVECTOR ray_dir = DirectX::XMVector3Normalize(ray_end_ws - ray_origin);

    return DirectX::SimpleMath::Ray(ray_origin, ray_dir);
}
