#include "Core/stdafx.h"
#include "DebugRenderer.h"
#include "Engine/Camera.h"
#include "Effects.h"
#include "DirectXHelpers.h"
#include "WICTextureLoader.h"
//#include "ResourceUploadBatch.h"

DebugRenderer* DebugRenderer::GetInstance()
{
    static DebugRenderer instance;
    return &instance;
}

void DebugRenderer::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
    m_device = device;
    m_context = context;

    // 3D rendering setup
    m_primitiveBatch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>(m_context.Get());
    m_basicEffect = std::make_unique<DirectX::BasicEffect>(m_device.Get());
    m_basicEffect->SetVertexColorEnabled(true);

    void const* shaderByteCode;
    size_t byteCodeLength;
    m_basicEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

    m_device->CreateInputLayout(
        DirectX::VertexPositionColor::InputElements,
        DirectX::VertexPositionColor::InputElementCount,
        shaderByteCode,
        byteCodeLength,
        m_inputLayout.ReleaseAndGetAddressOf()
    );

    // 2D rendering setup
    m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(m_context.Get());

    // Load the pre-compiled font. The user must generate this file.
    try
    {
        // The path should be relative to the executable's directory.
        m_spriteFont = std::make_unique<DirectX::SpriteFont>(m_device.Get(), L"Assets/Fonts/Arial16.spritefont");
    }
    catch (const std::exception& e)
    {
        // Log the error if the font file is missing.
        OutputDebugStringA("Error loading sprite font: ");
        OutputDebugStringA(e.what());
        OutputDebugStringA(""); 
        m_spriteFont = nullptr; 
    }

    // Text rendering is deferred due to the need for a content pipeline.
}

void DebugRenderer::Shutdown()
{
    m_primitiveBatch.reset();
    m_basicEffect.reset();
    m_inputLayout.Reset();
    m_spriteBatch.reset();
    m_spriteFont.reset(); // This will be null, but reset is safe
    m_lineVertices.clear();
    m_textCommands.clear();
}

void DebugRenderer::Render(Camera* camera)
{
    m_context->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
    m_context->OMSetDepthStencilState(nullptr, 0);
    m_context->RSSetState(nullptr);

    // 3D
    m_basicEffect->SetView(camera->GetViewMatrix());
    m_basicEffect->SetProjection(camera->GetProjectionMatrix());
    m_basicEffect->Apply(m_context.Get());
    m_context->IASetInputLayout(m_inputLayout.Get());

    m_primitiveBatch->Begin();
    if (!m_lineVertices.empty())
    {
        m_primitiveBatch->Draw(D3D_PRIMITIVE_TOPOLOGY_LINELIST, m_lineVertices.data(), m_lineVertices.size());
    }
    m_primitiveBatch->End();

    // 2D / Text
    m_spriteBatch->Begin();
    if (m_spriteFont)
    {
        for (const auto& command : m_textCommands)
        {
            m_spriteFont->DrawString(m_spriteBatch.get(), command.text.c_str(), command.position, command.color);
        }
    }
    m_spriteBatch->End();


    // Clear buffers for the next frame
    m_lineVertices.clear();
    m_textCommands.clear();
}

void DebugRenderer::DrawLine2D(const DirectX::SimpleMath::Vector2& start, const DirectX::SimpleMath::Vector2& end, const DirectX::SimpleMath::Color& color)
{
    // 2D lines are not directly supported by PrimitiveBatch in world space.
    // This would require a separate orthographic projection setup.
    // For now, we defer this implementation.
}

void DebugRenderer::DrawRect(const RECT& rect, const DirectX::SimpleMath::Color& color)
{
    DirectX::SimpleMath::Vector3 p1 = { (float)rect.left, (float)rect.top, 0.f };
    DirectX::SimpleMath::Vector3 p2 = { (float)rect.right, (float)rect.top, 0.f };
    DirectX::SimpleMath::Vector3 p3 = { (float)rect.right, (float)rect.bottom, 0.f };
    DirectX::SimpleMath::Vector3 p4 = { (float)rect.left, (float)rect.bottom, 0.f };

    DrawLine3D(p1, p2, color);
    DrawLine3D(p2, p3, color);
    DrawLine3D(p3, p4, color);
    DrawLine3D(p4, p1, color);
}

void DebugRenderer::DrawCircle2D(const DirectX::SimpleMath::Vector2& center, float radius, const DirectX::SimpleMath::Color& color, int segments)
{
    // 2D circles are not directly supported. Deferring.
}

void DebugRenderer::DrawText_M(const std::wstring& text, const DirectX::SimpleMath::Vector2& position, const DirectX::SimpleMath::Color& color)
{
    if (m_spriteFont)
    {
        m_textCommands.push_back({ text, position, color });
    }
}

void DebugRenderer::DrawLine3D(const DirectX::SimpleMath::Vector3& start, const DirectX::SimpleMath::Vector3& end, const DirectX::SimpleMath::Color& color)
{
    m_lineVertices.emplace_back(start, color);
    m_lineVertices.emplace_back(end, color);
}

void DebugRenderer::DrawBox(const DirectX::SimpleMath::Vector3& center, const DirectX::SimpleMath::Vector3& extents, const DirectX::SimpleMath::Color& color)
{
    using namespace DirectX::SimpleMath;

    Vector3 corners[8];
    corners[0] = center + Vector3(-extents.x, -extents.y, -extents.z);
    corners[1] = center + Vector3(extents.x, -extents.y, -extents.z);
    corners[2] = center + Vector3(extents.x, -extents.y, extents.z);
    corners[3] = center + Vector3(-extents.x, -extents.y, extents.z);
    corners[4] = center + Vector3(-extents.x, extents.y, -extents.z);
    corners[5] = center + Vector3(extents.x, extents.y, -extents.z);
    corners[6] = center + Vector3(extents.x, extents.y, extents.z);
    corners[7] = center + Vector3(-extents.x, extents.y, extents.z);

    DrawLine3D(corners[0], corners[1], color);
    DrawLine3D(corners[1], corners[2], color);
    DrawLine3D(corners[2], corners[3], color);
    DrawLine3D(corners[3], corners[0], color);

    DrawLine3D(corners[4], corners[5], color);
    DrawLine3D(corners[5], corners[6], color);
    DrawLine3D(corners[6], corners[7], color);
    DrawLine3D(corners[7], corners[4], color);

    DrawLine3D(corners[0], corners[4], color);
    DrawLine3D(corners[1], corners[5], color);
    DrawLine3D(corners[2], corners[6], color);
    DrawLine3D(corners[3], corners[7], color);
}

void DebugRenderer::DrawSphere(const DirectX::SimpleMath::Vector3& center, float radius, const DirectX::SimpleMath::Color& color, int segments)
{
    using namespace DirectX::SimpleMath;

    float angleStep = DirectX::XM_2PI / segments;

    // Draw circles along X, Y, Z axes
    for (int i = 0; i < segments; ++i)
    {
        float angle1 = i * angleStep;
        float angle2 = (i + 1) * angleStep;

        // YZ plane (X-axis circle)
        Vector3 p1_x = center + Vector3(0, cos(angle1) * radius, sin(angle1) * radius);
        Vector3 p2_x = center + Vector3(0, cos(angle2) * radius, sin(angle2) * radius);
        DrawLine3D(p1_x, p2_x, color);

        // XZ plane (Y-axis circle)
        Vector3 p1_y = center + Vector3(cos(angle1) * radius, 0, sin(angle1) * radius);
        Vector3 p2_y = center + Vector3(cos(angle2) * radius, 0, sin(angle2) * radius);
        DrawLine3D(p1_y, p2_y, color);

        // XY plane (Z-axis circle)
        Vector3 p1_z = center + Vector3(cos(angle1) * radius, sin(angle1) * radius, 0);
        Vector3 p2_z = center + Vector3(cos(angle2) * radius, sin(angle2) * radius, 0);
        DrawLine3D(p1_z, p2_z, color);
    }
}

void DebugRenderer::DrawCylinder(const DirectX::SimpleMath::Vector3& base, const DirectX::SimpleMath::Vector3& top, float radius, const DirectX::SimpleMath::Color& color, int segments)
{
     using namespace DirectX::SimpleMath;

    Vector3 axis = top - base;
    Vector3 normal = axis;
    normal.Normalize();

    Vector3 u, v;
    if (abs(normal.x) > 0.1f)
        u = Vector3::UnitY.Cross(normal);
    else
        u = Vector3::UnitX.Cross(normal);
    u.Normalize();
    v = normal.Cross(u);

    float angleStep = DirectX::XM_2PI / segments;
    for (int i = 0; i < segments; ++i)
    {
        float angle1 = i * angleStep;
        float angle2 = (i + 1) * angleStep;

        Vector3 c1 = cos(angle1) * u + sin(angle1) * v;
        Vector3 c2 = cos(angle2) * u + sin(angle2) * v;

        // Base circle
        DrawLine3D(base + c1 * radius, base + c2 * radius, color);
        // Top circle
        DrawLine3D(top + c1 * radius, top + c2 * radius, color);
        // Connecting lines
        if (i % (segments / 4) == 0) { // Draw a few connecting lines
             DrawLine3D(base + c1 * radius, top + c1 * radius, color);
        }
    }
}

void DebugRenderer::DrawCapsule(const DirectX::SimpleMath::Vector3& start, const DirectX::SimpleMath::Vector3& end, float radius, const DirectX::SimpleMath::Color& color, int segments)
{
    using namespace DirectX::SimpleMath;

    Vector3 axis = end - start;
    float height = axis.Length();
    if (height < 0.001f) {
        DrawSphere(start, radius, color, segments);
        return;
    }

    Vector3 normal = axis / height;
    Vector3 u, v;
    if (abs(normal.x) > 0.1f)
        u = Vector3::UnitY.Cross(normal);
    else
        u = Vector3::UnitX.Cross(normal);
    u.Normalize();
    v = normal.Cross(u);

    // Draw cylinder sides
    DrawLine3D(start + u * radius, end + u * radius, color);
    DrawLine3D(start - u * radius, end - u * radius, color);
    DrawLine3D(start + v * radius, end + v * radius, color);
    DrawLine3D(start - v * radius, end - v * radius, color);

    // Draw end caps
    float angleStep = DirectX::XM_2PI / segments;
    for (int i = 0; i < segments; ++i) {
        float a1 = i * angleStep;
        float a2 = (i + 1) * angleStep;

        Vector3 p1_start, p2_start, p1_end, p2_end;

        // XY plane caps
        p1_start = start + (cos(a1) * u + sin(a1) * v) * radius;
        p2_start = start + (cos(a2) * u + sin(a2) * v) * radius;
        DrawLine3D(p1_start, p2_start, color);

        p1_end = end + (cos(a1) * u + sin(a2) * v) * radius;
        p2_end = end + (cos(a2) * u + sin(a2) * v) * radius;
        DrawLine3D(p1_end, p2_end, color);

        // Half-sphere caps
        if (i < segments / 2) {
             // Start cap
             Vector3 p1_cap_start = start + (sin(a1) * normal + cos(a1) * v) * radius;
             Vector3 p2_cap_start = start + (sin(a2) * normal + cos(a2) * v) * radius;
             DrawLine3D(p1_cap_start, p2_cap_start, color);
             // End cap
             Vector3 p1_cap_end = end - (sin(a1) * normal - cos(a1) * v) * radius;
             Vector3 p2_cap_end = end - (sin(a2) * normal - cos(a2) * v) * radius;
             DrawLine3D(p1_cap_end, p2_cap_end, color);
        }
    }
}

void DebugRenderer::DrawArrow(const DirectX::SimpleMath::Vector3& start, const DirectX::SimpleMath::Vector3& end, float tipSize, const DirectX::SimpleMath::Color& color)
{
    using namespace DirectX::SimpleMath;

    DrawLine3D(start, end, color);

    Vector3 dir = end - start;
    dir.Normalize();

    Vector3 up = (abs(dir.y) < 0.99f) ? Vector3::UnitY : Vector3::UnitX;
    Vector3 right = dir.Cross(up);
    right.Normalize();
    up = right.Cross(dir);

    Vector3 tipBase = end - dir * tipSize;

    Vector3 p1 = tipBase + right * tipSize * 0.5f;
    Vector3 p2 = tipBase - right * tipSize * 0.5f;
    Vector3 p3 = tipBase + up * tipSize * 0.5f;
    Vector3 p4 = tipBase - up * tipSize * 0.5f;

    DrawLine3D(end, p1, color);
    DrawLine3D(end, p2, color);
    DrawLine3D(end, p3, color);
    DrawLine3D(end, p4, color);
}