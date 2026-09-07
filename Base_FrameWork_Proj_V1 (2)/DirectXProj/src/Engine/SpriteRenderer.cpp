#include "SpriteRenderer.h"
#include "GameObject.h"
#include "../Graphics/TextureManager.h"
#include "../Core/Graphics.h" // For ID3D11Device
#include <codecvt>
#include <locale>

namespace {
    // Helper to convert wstring to string for JSON
    std::string to_string(const std::wstring& wstr)
    {
        using convert_type = std::codecvt_utf8<wchar_t>;
        std::wstring_convert<convert_type, wchar_t> converter;
        return converter.to_bytes(wstr);
    }
    // Helper to convert string to wstring from JSON
    std::wstring to_wstring(const std::string& str)
    {
        using convert_type = std::codecvt_utf8<wchar_t>;
        std::wstring_convert<convert_type, wchar_t> converter;
        return converter.from_bytes(str);
    }
}

SpriteRenderer::SpriteRenderer(GameObject* owner)
    : Component(owner), m_graphics(nullptr), m_texture(nullptr)
{
}

SpriteRenderer::SpriteRenderer(GameObject* owner, const std::wstring& texturePath)
    : SpriteRenderer(owner)
{
    m_texturePath = texturePath;
}

SpriteRenderer::~SpriteRenderer()
{
}

void SpriteRenderer::Initialize(Graphics* graphics)
{
    m_graphics = graphics;
    m_texture = TextureManager::GetInstance()->Load(graphics->GetDevice(), m_texturePath);
}

void SpriteRenderer::Start()
{
    if (m_texture)
    {
        CreateBuffers();
    }
}

void SpriteRenderer::Render()
{
    if (!m_vertexBuffer || !m_indexBuffer || !m_graphics)
    {
        return;
    }

    // Pass this renderer to the graphics system to be drawn.
    m_graphics->Draw(this);
}

void SpriteRenderer::CreateBuffers()
{
    ID3D11Device* device = m_graphics->GetDevice();
    if (!device) return;

    float halfWidth = static_cast<float>(m_texture->GetWidth()) / 2.0f;
    float halfHeight = static_cast<float>(m_texture->GetHeight()) / 2.0f;

    // Create vertex buffer
    Vertex vertices[] =
    {
        { DirectX::XMFLOAT3(-halfWidth, -halfHeight, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f) }, // Bottom-left
        { DirectX::XMFLOAT3(-halfWidth,  halfHeight, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f) }, // Top-left
        { DirectX::XMFLOAT3( halfWidth,  halfHeight, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f) }, // Top-right
        { DirectX::XMFLOAT3( halfWidth, -halfHeight, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f) }, // Bottom-right
    };

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(Vertex) * 4;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA sd = {};
    sd.pSysMem = vertices;

    device->CreateBuffer(&bd, &sd, &m_vertexBuffer);

    // Create index buffer
    unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(unsigned int) * 6;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;

    sd.pSysMem = indices;

    device->CreateBuffer(&bd, &sd, &m_indexBuffer);
}

void SpriteRenderer::ToJson(json& j) const
{
    Component::ToJson(j);
    j["texture_path"] = to_string(m_texturePath);
}

void SpriteRenderer::FromJson(const json& j)
{
    Component::FromJson(j);
    if (j.contains("texture_path"))
    {
        SetTexturePath(to_wstring(j["texture_path"]));
    }
}
