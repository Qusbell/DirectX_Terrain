#include "Transform.h"
#include "GameObject.h"

namespace {
    void SerializeVector(json& j, const DirectX::XMVECTOR& vec) {
        j = { DirectX::XMVectorGetX(vec), DirectX::XMVectorGetY(vec), DirectX::XMVectorGetZ(vec), DirectX::XMVectorGetW(vec) };
    }
    DirectX::XMVECTOR DeserializeVector(const json& j) {
        return DirectX::XMVectorSet(j[0], j[1], j[2], j[3]);
    }
}

Transform::Transform(GameObject* owner)
    : Component(owner), m_isDirty(true)
{
    m_localPosition = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
    m_localRotation = DirectX::XMQuaternionIdentity();
    m_localScale = DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
    m_worldMatrix = DirectX::XMMatrixIdentity();
}

Transform::~Transform()
{
}

void Transform::UpdateWorldMatrix()
{
    DirectX::XMMATRIX localMatrix = DirectX::XMMatrixScalingFromVector(m_localScale) *
                                    DirectX::XMMatrixRotationQuaternion(m_localRotation) *
                                    DirectX::XMMatrixTranslationFromVector(m_localPosition);

    GameObject* parent = GetOwner()->GetParent();
    if (parent)
    {
        m_worldMatrix = localMatrix * parent->GetTransform()->GetWorldMatrix();
    }
    else
    {
        m_worldMatrix = localMatrix;
    }

    m_isDirty = false;
}

const DirectX::XMMATRIX& Transform::GetWorldMatrix()
{
    if (m_isDirty)
    {
        UpdateWorldMatrix();
    }
    return m_worldMatrix;
}

DirectX::XMVECTOR Transform::GetPosition() const
{
    // To be correct, this should decompose the world matrix
    // For now, this is a simplification.
    DirectX::XMMATRIX world = const_cast<Transform*>(this)->GetWorldMatrix();
    return world.r[3];
}

DirectX::XMVECTOR Transform::GetRotation() const
{
    DirectX::XMMATRIX world = const_cast<Transform*>(this)->GetWorldMatrix();
    DirectX::XMVECTOR scale, rot, pos;
    DirectX::XMMatrixDecompose(&scale, &rot, &pos, world);
    return rot;
}

DirectX::XMVECTOR Transform::GetScale() const
{
    DirectX::XMMATRIX world = const_cast<Transform*>(this)->GetWorldMatrix();
    DirectX::XMVECTOR scale, rot, pos;
    DirectX::XMMatrixDecompose(&scale, &rot, &pos, world);
    return scale;
}

DirectX::XMVECTOR Transform::GetForward() const
{
    DirectX::XMMATRIX world = const_cast<Transform*>(this)->GetWorldMatrix();
    return DirectX::XMVector3Normalize(world.r[2]);
}

DirectX::XMVECTOR Transform::GetUp() const
{
    DirectX::XMMATRIX world = const_cast<Transform*>(this)->GetWorldMatrix();
    return DirectX::XMVector3Normalize(world.r[1]);
}

DirectX::XMVECTOR Transform::GetRight() const
{
    DirectX::XMMATRIX world = const_cast<Transform*>(this)->GetWorldMatrix();
    return DirectX::XMVector3Normalize(world.r[0]);
}

void Transform::SetLocalPosition(const DirectX::XMVECTOR& position)
{
    m_localPosition = position;
    SetDirty();
}

void Transform::SetLocalPosition(float x, float y, float z)
{
    m_localPosition = DirectX::XMVectorSet(x, y, z, 1.0f);
    SetDirty();
}

void Transform::SetLocalRotation(const DirectX::XMVECTOR& rotation)
{
    m_localRotation = rotation;
    SetDirty();
}

void Transform::SetLocalRotation(float x, float y, float z)
{
    //m_localRotation = DirectX::XMVectorSet(x, y, z, 1.0f);
    //DirectX::XMVECTOR rotationThisFrame = DirectX::XMQuaternionRotationAxis(
    //    DirectX::XMVectorSet(0.0f, 1.0f, 1.0f, 0.0f),
    //    rotationSpeed * deltaTime
    //);

    m_localRotation = DirectX::XMQuaternionRotationRollPitchYaw(x, y, z);
    SetDirty();
}

void Transform::SetLocalScale(const DirectX::XMVECTOR& scale)
{
    m_localScale = scale;
    SetDirty();
}

void Transform::SetLocalScale(float x, float y, float z)
{
    m_localScale = DirectX::XMVectorSet(x, y, z, 1.0f);
    SetDirty();
}

void Transform::SetDirty()
{
    m_isDirty = true;
    SetChildrenDirty();
}

void Transform::SetChildrenDirty()
{
    for (auto* child : GetOwner()->GetChildren())
    {
        child->GetTransform()->SetDirty();
    }
}

void Transform::ToJson(json& j) const
{
    Component::ToJson(j); // Call base class method
    json pos, rot, scl;
    SerializeVector(pos, m_localPosition);
    SerializeVector(rot, m_localRotation);
    SerializeVector(scl, m_localScale);
    j["position"] = pos;
    j["rotation"] = rot;
    j["scale"] = scl;
}

void Transform::FromJson(const json& j)
{
    Component::FromJson(j);
    SetLocalPosition(DeserializeVector(j["position"]));
    SetLocalRotation(DeserializeVector(j["rotation"]));
    SetLocalScale(DeserializeVector(j["scale"]));
}
