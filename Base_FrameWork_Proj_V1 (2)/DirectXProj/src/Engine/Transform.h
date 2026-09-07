#pragma once
#include "Component.h"
#include <DirectXMath.h>

class Transform : public Component
{
public:
    Transform(GameObject* owner);
    ~Transform();

    // World space getters
    DirectX::XMVECTOR GetPosition() const;
    DirectX::XMVECTOR GetRotation() const;
    DirectX::XMVECTOR GetScale() const;
    const DirectX::XMMATRIX& GetWorldMatrix();

    // Local space getters
    DirectX::XMVECTOR GetLocalPosition() const { return m_localPosition; }
    DirectX::XMVECTOR GetLocalRotation() const { return m_localRotation; }
    DirectX::XMVECTOR GetLocalScale() const { return m_localScale; }

    // Local space setters
    void SetLocalPosition(const DirectX::XMVECTOR& position);
    void SetLocalPosition(float x, float y, float z);
    void SetLocalRotation(const DirectX::XMVECTOR& rotation);
    void SetLocalScale(const DirectX::XMVECTOR& scale);
    void SetLocalScale(float x, float y, float z);

    void SetDirty();

    virtual void ToJson(json& j) const override;
    virtual void FromJson(const json& j) override;

private:
    void UpdateWorldMatrix();
    void SetChildrenDirty();

    // Local space values
    DirectX::XMVECTOR m_localPosition;
    DirectX::XMVECTOR m_localRotation; // Quaternion
    DirectX::XMVECTOR m_localScale;

    // World space cache
    DirectX::XMMATRIX m_worldMatrix;
    bool m_isDirty;
};
