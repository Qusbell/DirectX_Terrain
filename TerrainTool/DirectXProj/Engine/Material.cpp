#include "../Core/stdafx.h"
#include "Material.h"
#include "../Graphics/Texture.h"
#include "../Graphics/Shader.h"

Material::Material(ID3D11Device* device)
    : m_shader(nullptr),
    m_albedoTexture(nullptr),
    m_normalTexture(nullptr),
    m_metallicTexture(nullptr),
    m_isDirty(true)
{
    m_constants.tiling = { 1.0f, 1.0f };
    m_constants.offset = { 0.0f, 0.0f };

    // Create the constant buffer
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(MaterialConstants);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    HRESULT hr = device->CreateBuffer(&bd, nullptr, &m_materialConstantBuffer);
    // In a real engine, you would handle this failure more gracefully.
    if (FAILED(hr))
    {
        // Handle error
    }
}

void Material::Apply(ID3D11DeviceContext* context)
{
    if (m_isDirty)
    {
        UpdateConstantBuffer(context);
        m_isDirty = false;
    }

    if (m_shader)
    {
        m_shader->Set(context);
    }

    // Bind resources
    context->PSSetConstantBuffers(3, 1, m_materialConstantBuffer.GetAddressOf());

    if (m_albedoTexture)
    {
        m_albedoTexture->Set(context, 0); // t0
    }
    if (m_normalTexture)
    {
        m_normalTexture->Set(context, 1); // t1
    }
    if (m_metallicTexture)
    {
        m_metallicTexture->Set(context, 2); // t2
    }
}

void Material::UpdateConstantBuffer(ID3D11DeviceContext* context)
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = context->Map(m_materialConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (SUCCEEDED(hr))
    {
        memcpy(mappedResource.pData, &m_constants, sizeof(MaterialConstants));
        context->Unmap(m_materialConstantBuffer.Get(), 0);
    }
}

void Material::SetShader(Shader* shader)
{
    m_shader = shader;
}

void Material::SetAlbedoTexture(std::shared_ptr<Texture> texture)
{
    m_albedoTexture = texture;
}

void Material::SetNormalTexture(std::shared_ptr<Texture> texture)
{
    m_normalTexture = texture;
}

void Material::SetMetallicTexture(std::shared_ptr<Texture> texture)
{
    m_metallicTexture = texture;
}

void Material::SetTiling(const DirectX::XMFLOAT2& tiling)
{
    m_constants.tiling = tiling;
    m_isDirty = true;
}

void Material::SetOffset(const DirectX::XMFLOAT2& offset)
{
    m_constants.offset = offset;
    m_isDirty = true;
}
