#pragma once
#include <Windows.h>
#include <memory>
#include <vector>
#include <string>
#include <d3d11.h>
#include <DirectXMath.h>
#include "SimpleMath.h"
#include "PrimitiveBatch.h"
#include "VertexTypes.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "Effects.h"

#include <wrl/client.h>


// lib load
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "DirectXTK.lib")


class DirectX::BasicEffect;
class Camera;


class DebugRenderer
{
public:
    // Singleton access
    static DebugRenderer* GetInstance();

    // Initialization and shutdown
    void Initialize(ID3D11Device* device, ID3D11DeviceContext* context);
    void Shutdown();

    // Rendering
    void Render(Camera* camera);

    // 2D Drawing
    void DrawLine2D(const DirectX::SimpleMath::Vector2& start, const DirectX::SimpleMath::Vector2& end, const DirectX::SimpleMath::Color& color);
    void DrawRect(const RECT& rect, const DirectX::SimpleMath::Color& color);
    void DrawCircle2D(const DirectX::SimpleMath::Vector2& center, float radius, const DirectX::SimpleMath::Color& color, int segments = 16);
    void DrawText_M(const std::wstring& text, const DirectX::SimpleMath::Vector2& position, const DirectX::SimpleMath::Color& color);

    // 3D Drawing
    void DrawLine3D(const DirectX::SimpleMath::Vector3& start, const DirectX::SimpleMath::Vector3& end, const DirectX::SimpleMath::Color& color);
    void DrawBox(const DirectX::SimpleMath::Vector3& center, const DirectX::SimpleMath::Vector3& extents, const DirectX::SimpleMath::Color& color);
    void DrawSphere(const DirectX::SimpleMath::Vector3& center, float radius, const DirectX::SimpleMath::Color& color, int segments = 16);
    void DrawCylinder(const DirectX::SimpleMath::Vector3& base, const DirectX::SimpleMath::Vector3& top, float radius, const DirectX::SimpleMath::Color& color, int segments = 16);
    void DrawCapsule(const DirectX::SimpleMath::Vector3& start, const DirectX::SimpleMath::Vector3& end, float radius, const DirectX::SimpleMath::Color& color, int segments = 16);
    void DrawArrow(const DirectX::SimpleMath::Vector3& start, const DirectX::SimpleMath::Vector3& end, float tipSize, const DirectX::SimpleMath::Color& color);


private:
    DebugRenderer() = default;
    ~DebugRenderer() = default;
    DebugRenderer(const DebugRenderer&) = delete;
    DebugRenderer& operator=(const DebugRenderer&) = delete;

    struct DebugVertex
    {
        DirectX::SimpleMath::Vector3 position;
        DirectX::SimpleMath::Color color;
    };

    struct TextCommand
    {
        std::wstring text;
        DirectX::SimpleMath::Vector2 position;
        DirectX::SimpleMath::Color color;
    };

    // D3D Resources
    Microsoft::WRL::ComPtr<ID3D11Device> m_device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;

    // 3D Rendering
    std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_primitiveBatch;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
    std::unique_ptr<DirectX::BasicEffect> m_basicEffect;

    // 2D Rendering
    std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
    std::unique_ptr<DirectX::SpriteFont> m_spriteFont;

    // Data buffers
    std::vector<DirectX::VertexPositionColor> m_lineVertices;
    std::vector<TextCommand> m_textCommands;
};

