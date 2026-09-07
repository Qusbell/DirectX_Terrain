#pragma once
#include "Component.h"
#include <DirectXMath.h>

#define CAMERAROOTPOS 10.f

class InputManager;

class InputComponent : public Component
{
public:
    InputComponent(GameObject* owner);
    ~InputComponent() override = default;

    void Start() override;
    void Update() override;


protected:
    void UpdateKeyboard();

private:
    // Mouse state
    DirectX::XMFLOAT2 m_lastMousePosition;
    float m_yaw;
    float m_pitch;

    // Movement speeds
    float m_moveSpeed = 10.0f;
    float m_rotationSpeed = 0.1f; // Adjusted for sensitivity
    float m_zoomSpeed = 0.3f;
    float m_ShiftzoomSpeed = 6.0f;

    float m_CameraRootDistance = 10.f;

    InputManager* m_Input = nullptr;

    float m_ShiftWeightVal = 5.f;
};
