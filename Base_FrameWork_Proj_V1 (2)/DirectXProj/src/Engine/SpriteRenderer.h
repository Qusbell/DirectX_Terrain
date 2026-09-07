#pragma once
#include "Component.h"
#include "../Graphics/Texture.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <string>

// Forward declare to avoid including Graphics.h
class Graphics;

struct Vertex
{
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT2 uv;
};

class SpriteRenderer : public Component
{
    friend class Graphics; // Allow Graphics to access private members

public:
    SpriteRenderer(GameObject* owner);
    SpriteRenderer(GameObject* owner, const std::wstring& texturePath);
    ~SpriteRenderer();

    void SetTexturePath(const std::wstring& path) { m_texturePath = path; }
    void Initialize(Graphics* graphics);
    virtual void Start() override;
    virtual void Render() override;
    virtual void ToJson(json& j) const override;
    virtual void FromJson(const json& j) override;

private:
    void CreateBuffers();

    Graphics* m_graphics; // Need access to the device
    Texture* m_texture;
    std::wstring m_texturePath;

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
};
