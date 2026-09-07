#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include "../Graphics/Shader.h"
#include <memory>
#include "../Engine/Camera.h"
#include "../Engine/LightManager.h"
#include "SingletonT.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

class SpriteRenderer; // Forward declaration
class MeshRenderer;   // Forward declaration
class Camera;           // Forward declaration
class Scene;            // Forward declaration

struct BaseConstants
{
	DirectX::XMFLOAT4 Timer;
};

// Constants that are updated once per frame
struct FrameConstants
{
    DirectX::XMMATRIX view;
    DirectX::XMMATRIX projection;
};

// Constants that are updated per-object
struct ObjectConstants
{
    DirectX::XMMATRIX world;
};

// Must match the cbuffer in the PBR pixel shader
struct LightConstants
{
    DirectX::XMFLOAT3 LightDirection;
    float  _padding1;
    DirectX::XMFLOAT4 LightColor;
    DirectX::XMFLOAT3 CameraPosition;
    float  _padding2;
};



class CoreGraphicsManager : public SingletonT<CoreGraphicsManager>
{
    friend class SingletonT<CoreGraphicsManager>;
private:
    CoreGraphicsManager();
    ~CoreGraphicsManager();

public:
    bool Initialize(HWND hWnd, int width, int height);
    void Shutdown();

    void SetViewport(const D3D11_VIEWPORT& viewport);
    void Clear(Camera::ClearFlags flags, const DirectX::XMFLOAT4& color, float depth = 1.0f, UINT8 stencil = 0);
    void BeginFrame(Camera* camera);
    void UpdateLights(Camera* camera);
    void EndFrame();
    void Draw(SpriteRenderer* renderer, Camera* camera);
    void DrawMesh(MeshRenderer* renderer, Camera* camera);
    void DrawUI(class UIImage* image);

    ID3D11Device* GetDevice() const { return m_device.Get(); }
    ID3D11DeviceContext* GetContext() const { return m_context.Get(); }
    Shader* GetSpriteShader() const { return m_spriteShader.get(); }
    Shader* GetMeshShader() const { return m_meshShader.get(); }
    Shader* GetUIShader() const { return m_uiShader.get(); }
    ID3D11Buffer* GetFrameConstantBuffer() const { return m_frameConstantBuffer.Get(); }
    ID3D11RenderTargetView* GetRenderTargetView() const { return m_renderTargetView.Get(); }
    ID3D11DepthStencilView* GetDepthStencilView() const { return m_depthStencilView.Get(); }
    int GetWindowWidth() const { return m_windowWidth; }
    int GetWindowHeight() const { return m_windowHeight; }

private:
    int m_windowWidth;
    int m_windowHeight;
    Microsoft::WRL::ComPtr<ID3D11Device> m_device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;

    // Sprite Rendering Resources
    std::unique_ptr<Shader> m_spriteShader;
    std::unique_ptr<Shader> m_meshShader;
    std::unique_ptr<Shader> m_uiShader;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_baseConstantBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_frameConstantBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_objectConstantBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_lightConstantBuffer;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilStateUI;
    Microsoft::WRL::ComPtr<ID3D11BlendState> m_alphaBlendState;

    DirectX::XMMATRIX m_orthoProjectionMatrix;
};
