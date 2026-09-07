#pragma once
#include <memory>
#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl/client.h>

// Forward declarations
class Texture;
class Shader;

// This struct must match the layout of the cbuffer in the pixel shader
struct MaterialConstants
{
    DirectX::XMFLOAT2 tiling;
    DirectX::XMFLOAT2 offset;
};

class Material
{
public:
    Material(ID3D11Device* device);
    ~Material() = default;

    void Apply(ID3D11DeviceContext* context);

    // --- Setters ---
    void SetShader(Shader* shader);
    void SetAlbedoTexture(std::shared_ptr<Texture> texture);
    void SetNormalTexture(std::shared_ptr<Texture> texture);
    void SetMetallicTexture(std::shared_ptr<Texture> texture);
    void SetTiling(const DirectX::XMFLOAT2& tiling);
    void SetOffset(const DirectX::XMFLOAT2& offset);

    // --- Getters ---
    std::shared_ptr<Texture> GetAlbedoTexture() const { return m_albedoTexture; }

private:
    void UpdateConstantBuffer(ID3D11DeviceContext* context);

    Shader* m_shader;

    // Textures
    std::shared_ptr<Texture> m_albedoTexture;
    std::shared_ptr<Texture> m_normalTexture;
    std::shared_ptr<Texture> m_metallicTexture;

    // Data to be sent to GPU
    MaterialConstants m_constants;
    bool m_isDirty;

    // D3D Resources
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_materialConstantBuffer;
};
