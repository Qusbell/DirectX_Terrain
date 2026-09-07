#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include "../Graphics/Shader.h"
#include <memory>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

class SpriteRenderer; // Forward declaration

struct ConstantBufferWVP
{
    DirectX::XMMATRIX wvp;
};

class Graphics
{
public:
    Graphics();
    ~Graphics();

    bool Initialize(HWND hWnd, int width, int height);
    void Shutdown();

    void BeginFrame();
    void EndFrame();
    void Draw(SpriteRenderer* renderer);

    ID3D11Device* GetDevice() const { return m_device.Get(); }
    ID3D11DeviceContext* GetContext() const { return m_context.Get(); }

private:
    Microsoft::WRL::ComPtr<ID3D11Device> m_device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;

    // Sprite Rendering Resources
    std::unique_ptr<Shader> m_spriteShader;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerState;

    DirectX::XMMATRIX m_viewMatrix;
    DirectX::XMMATRIX m_projectionMatrix;
};
