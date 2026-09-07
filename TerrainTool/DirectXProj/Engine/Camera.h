#pragma once
#include "Component.h"
#include <DirectXMath.h>
#include "SimpleMath.h"

class Camera : public Component
{
public:
    enum class ProjectionType
    {
        Perspective,
        Orthographic
    };

    enum class ClearFlags
    {
        SolidColor,
        DepthOnly,
        DontClear
    };

    Camera(GameObject* owner);
    ~Camera() override = default;

    void Update() override;

    // --- Getters ---
    const DirectX::XMMATRIX& GetViewMatrix() const { return m_viewMatrix; }
    const DirectX::XMMATRIX& GetProjectionMatrix() const { return m_projectionMatrix; }
    ProjectionType GetProjectionType() const { return m_projectionType; }
    float GetNearClipPlane() const { return m_nearClipPlane; }
    float GetFarClipPlane() const { return m_farClipPlane; }
    float GetAspectRatio() const { return m_aspectRatio; }
    DirectX::XMFLOAT2 GetLensShift() const { return m_lensShift; }
    float GetFieldOfView() const { return m_fieldOfView; }
    float GetFocalLength() const { return m_focalLength; }
    DirectX::XMFLOAT2 GetSensorSize() const { return m_sensorSize; }
    float GetOrthographicSize() const { return m_orthographicSize; }
    int GetDepth() const { return m_depth; }
    DirectX::XMFLOAT4 GetViewportRect() const { return m_viewportRect; }
    ClearFlags GetClearFlags() const { return m_clearFlags; }
    DirectX::XMFLOAT4 GetBackgroundColor() const { return m_backgroundColor; }

    DirectX::SimpleMath::Ray ScreenPointToRay(const DirectX::SimpleMath::Vector2& screenPos) const;

    // --- Setters ---
    void SetProjectionType(ProjectionType type);
    void SetNearClipPlane(float nearPlane);
    void SetFarClipPlane(float farPlane);
    void SetAspectRatio(float aspectRatio);
    void SetLensShift(DirectX::XMFLOAT2 shift);
    void SetFieldOfView(float fov);      // In degrees
    void SetFocalLength(float length);   // In mm
    void SetSensorSize(DirectX::XMFLOAT2 size); // In mm
    void SetOrthographicSize(float size);
    void SetDepth(int depth);
    void SetViewportRect(const DirectX::XMFLOAT4& rect);
    void SetClearFlags(ClearFlags flags);
    void SetBackgroundColor(const DirectX::XMFLOAT4& color);

private:
    void RecalculateProjectionMatrix();

    // Matrices
    DirectX::XMMATRIX m_viewMatrix;
    DirectX::XMMATRIX m_projectionMatrix;
    bool m_isProjectionDirty;

    // Common Properties
    ProjectionType m_projectionType;
    float m_nearClipPlane;
    float m_farClipPlane;
    float m_aspectRatio;
    DirectX::XMFLOAT2 m_lensShift;

    // Perspective Properties
    float m_fieldOfView; // In degrees
    float m_focalLength; // In mm

    // Physical Camera Properties
    DirectX::XMFLOAT2 m_sensorSize; // In mm, x is width, y is height

    // Orthographic Properties
    float m_orthographicSize;

    // Rendering Order & Clearing
    int m_depth;
    DirectX::XMFLOAT4 m_viewportRect; // x, y, width, height (0-1)
    ClearFlags m_clearFlags;
    DirectX::XMFLOAT4 m_backgroundColor;
};
