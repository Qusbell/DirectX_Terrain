#include "stdafx.h"
#include "Graphics.h"
#include "../Engine/SpriteRenderer.h"
#include "../Engine/GameObject.h"
#include "../Graphics/ShaderManager.h"
#include <cassert>

Graphics::Graphics()
{
}

Graphics::~Graphics()
{
    // ComPtr will auto-release
}

bool Graphics::Initialize(HWND hWnd, int width, int height)
{
    // --- Basic D3D11 setup (same as before) ---
    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferCount = 1;
    scd.BufferDesc.Width = width;
    scd.BufferDesc.Height = height;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferDesc.RefreshRate.Numerator = 60;
    scd.BufferDesc.RefreshRate.Denominator = 1;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hWnd;
    scd.SampleDesc.Count = 1;
    scd.SampleDesc.Quality = 0;
    scd.Windowed = TRUE;
    scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, nullptr, 0, D3D11_SDK_VERSION, &scd, &m_swapChain, &m_device, nullptr, &m_context);
    if (FAILED(hr)) { MessageBox(hWnd, L"D3D11CreateDeviceAndSwapChain Failed.", L"Error", MB_OK); return false; }

    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBuffer);
    if (FAILED(hr)) { MessageBox(hWnd, L"Failed to get back buffer.", L"Error", MB_OK); return false; }

    hr = m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_renderTargetView);
    if (FAILED(hr)) { MessageBox(hWnd, L"Failed to create render target view.", L"Error", MB_OK); return false; }

    m_context->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), nullptr);

    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    m_context->RSSetViewports(1, &vp);

    // --- New Sprite Rendering Stuff ---
    // Load Shaders
    m_spriteShader = std::make_unique<Shader>();
    if (!m_spriteShader->Load(m_device.Get(), L"Shaders/SpriteVS.hlsl", L"Shaders/SpritePS.hlsl")) {
        MessageBox(hWnd, L"Failed to compile/load shaders.", L"Error", MB_OK);
        return false;
    }
#ifdef HOT_RELOAD_ENABLED
    ShaderManager::GetInstance()->RegisterShader(m_spriteShader.get());
#endif

    // Create Constant Buffer
    D3D11_BUFFER_DESC cbd = {};
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.ByteWidth = sizeof(ConstantBufferWVP);
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    hr = m_device->CreateBuffer(&cbd, nullptr, &m_constantBuffer);
    if (FAILED(hr)) { MessageBox(hWnd, L"Failed to create constant buffer.", L"Error", MB_OK); return false; }

    // Create Sampler State
    D3D11_SAMPLER_DESC sd = {};
    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sd.MinLOD = 0;
    sd.MaxLOD = D3D11_FLOAT32_MAX;
    hr = m_device->CreateSamplerState(&sd, &m_samplerState);
    if (FAILED(hr)) { MessageBox(hWnd, L"Failed to create sampler state.", L"Error", MB_OK); return false; }

    // Create Camera Matrices
    m_viewMatrix = DirectX::XMMatrixLookAtLH(
        DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f), // Eye position
        DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),  // Focus point
        DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)   // Up direction
    );
    m_projectionMatrix = DirectX::XMMatrixOrthographicLH((float)width, (float)height, 0.1f, 100.0f);

    return true;
}

void Graphics::Shutdown()
{
    // Resources are released by ComPtr when this object is destroyed.
}

void Graphics::BeginFrame()
{
    const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
    m_context->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);
}

void Graphics::EndFrame()
{
    m_swapChain->Present(1, 0);
}

void Graphics::Draw(SpriteRenderer* renderer)
{
    if (!renderer) return;

    // Get data from the renderer
    auto texture = renderer->m_texture;
    auto vertexBuffer = renderer->m_vertexBuffer;
    auto indexBuffer = renderer->m_indexBuffer;
    if (!texture || !vertexBuffer || !indexBuffer) return;

    // Set shaders
    m_spriteShader->Set(m_context.Get());

    // Update constant buffer
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    m_context->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    ConstantBufferWVP* dataPtr = (ConstantBufferWVP*)mappedResource.pData;
    DirectX::XMMATRIX world = renderer->GetOwner()->GetTransform()->GetWorldMatrix();
    dataPtr->wvp = DirectX::XMMatrixTranspose(world * m_viewMatrix * m_projectionMatrix);
    m_context->Unmap(m_constantBuffer.Get(), 0);

    // Set resources
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    m_context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
    m_context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
    texture->Set(m_context.Get(), 0);
    m_context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());

    // Draw
    m_context->DrawIndexed(6, 0, 0);
}
