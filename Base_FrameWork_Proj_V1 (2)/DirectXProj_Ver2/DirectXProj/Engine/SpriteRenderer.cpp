#include "SpriteRenderer.h"
#include "GameObject.h"
#include "Material.h"
#include "../Graphics/Texture.h"
#include "../Graphics/TextureManager.h"
#include "../Core/CoreGraphicsManager.h" // For ID3D11Device
#include <codecvt>
#include <locale>
#include <Windows.h>
#include "Component.h"


namespace {
    //// Helper to convert wstring to string for JSON
    //std::string to_string(const std::wstring& wstr)
    //{
    //    using convert_type = std::codecvt_utf8<wchar_t>;
    //    std::wstring_convert<convert_type, wchar_t> converter;
    //    return converter.to_bytes(wstr);
    //}
    //// Helper to convert string to wstring from JSON
    //std::wstring to_wstring(const std::string& str)
    //{
    //    using convert_type = std::codecvt_utf8<wchar_t>;
    //    std::wstring_convert<convert_type, wchar_t> converter;
    //    return converter.from_bytes(str);
    //}


    // https://chatgpt.com/c/68ae6b05-31e0-8321-940c-85f15b45889f
    // wstring → string (UTF-8)
    std::string to_string(const std::wstring& wstr)
    {
        if (wstr.empty()) return {};

        int sizeNeeded = WideCharToMultiByte(
            CP_UTF8, 0, wstr.c_str(), (int)wstr.size(),
            nullptr, 0, nullptr, nullptr);

        std::string result(sizeNeeded, 0);
        WideCharToMultiByte(
            CP_UTF8, 0, wstr.c_str(), (int)wstr.size(),
            &result[0], sizeNeeded, nullptr, nullptr);

        return result;
    }

    // string(UTF-8) → wstring
    std::wstring to_wstring(const std::string& str)
    {
        if (str.empty()) return {};

        int sizeNeeded = MultiByteToWideChar(
            CP_UTF8, 0, str.c_str(), (int)str.size(),
            nullptr, 0);

        std::wstring result(sizeNeeded, 0);
        MultiByteToWideChar(
            CP_UTF8, 0, str.c_str(), (int)str.size(),
            &result[0], sizeNeeded);

        return result;
    }
}



#include "../Core/CoreGraphicsManager.h"

SpriteRenderer::SpriteRenderer(GameObject* owner)
    : IRenderer(owner), m_graphics(CoreGraphicsManager::GetI()), m_material(nullptr)
{
}

SpriteRenderer::SpriteRenderer(GameObject* owner, std::shared_ptr<Material> material)
    : IRenderer(owner), m_graphics(CoreGraphicsManager::GetI()), m_material(material)
{
}

void SpriteRenderer::SetMaterial(std::shared_ptr<Material> material)
{
    m_material = material;
    // If we are already initialized, we might need to recreate buffers
    if (m_graphics)
    {
        CreateBuffers();
    }
}

void SpriteRenderer::Start()
{
    if (m_material && m_material->GetAlbedoTexture())
    {
        CreateBuffers();
    }
}

void SpriteRenderer::Render(Camera* camera)
{
    if (!m_vertexBuffer || !m_indexBuffer || !m_graphics || !m_material)
    {
        return;
    }

    // Pass this renderer and the camera to the graphics system to be drawn.
    m_graphics->Draw(this, camera);
}

void SpriteRenderer::CreateBuffers()
{
    if (!m_material || !m_graphics) return;
    auto texture = m_material->GetAlbedoTexture();
    if (!texture) return;

    ID3D11Device* device = m_graphics->GetDevice();
    if (!device) return;

    float halfWidth = 1.f;// static_cast<float>(texture->GetWidth()) / 2.0f;
    float halfHeight = 1.f;// static_cast<float>(texture->GetHeight()) / 2.0f;

    // Create vertex buffer
    Vertex_2DSPR vertices[] =
    {
        { DirectX::XMFLOAT3(-halfWidth, -halfHeight, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f) }, // Bottom-left
        { DirectX::XMFLOAT3(-halfWidth,  halfHeight, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f) }, // Top-left
        { DirectX::XMFLOAT3( halfWidth,  halfHeight, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f) }, // Top-right
        { DirectX::XMFLOAT3( halfWidth, -halfHeight, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f) }, // Bottom-right
    };

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(Vertex_2DSPR) * 4;
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
    if (m_material && m_material->GetAlbedoTexture())
    {
        j["texture_path"] = to_string(m_material->GetAlbedoTexture()->GetPath());
    }
}

void SpriteRenderer::FromJson(const json& j)
{
    Component::FromJson(j);
    if (j.contains("texture_path") && m_graphics)
    {
        std::wstring path = ::to_wstring(j["texture_path"]);
        auto texture = TextureManager::GetInstance()->LoadShared(m_graphics->GetDevice(), path);
        if (texture)
        {
            m_material = std::make_shared<Material>(m_graphics->GetDevice());
            m_material->SetAlbedoTexture(texture);
            // In a real engine, we'd also load the correct shader here.
            // For now, the Graphics class assigns a default sprite shader.
        }
    }
}

