#include "stdafx.h"
#include "CoreGraphicsManager.h"
#include "../Engine/SpriteRenderer.h"
#include "../Engine/MeshRenderer.h"
#include "../Engine/UIImage.h"
#include "../Engine/Material.h"
#include "../Graphics/Texture.h"
#include "../Graphics/Vertex.h"
#include "../Engine/GameObject.h"
#include "../Engine/Camera.h"
#include "../Engine/Mesh.h"
#include "../Engine/Scene.h"
#include "../Engine/Light.h"
#include "../Graphics/ShaderManager.h"

#include "TimeManager.h"

#include <cassert>

CoreGraphicsManager::CoreGraphicsManager()
{
}

CoreGraphicsManager::~CoreGraphicsManager()
{
    // ComPtr will auto-release
}

bool CoreGraphicsManager::Initialize(HWND hWnd, int width, int height)
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

	m_windowHeight = height;
	m_windowWidth = width;

    // --- Create Depth Stencil Buffer ---
    D3D11_TEXTURE2D_DESC depthStencilDesc = {};
    depthStencilDesc.Width = width;
    depthStencilDesc.Height = height;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
    hr = m_device->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencilBuffer);
    if (FAILED(hr)) { MessageBox(hWnd, L"Failed to create depth stencil buffer.", L"Error", MB_OK); return false; }

    hr = m_device->CreateDepthStencilView(depthStencilBuffer.Get(), nullptr, &m_depthStencilView);
    if (FAILED(hr)) { MessageBox(hWnd, L"Failed to create depth stencil view.", L"Error", MB_OK); return false; }

    m_context->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

    // Set a default viewport that covers the whole window
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    m_context->RSSetViewports(1, &vp);

    // --- New Sprite Rendering Stuff ---
    // --- Define Input Layouts ---
    // Layout for 2D sprites
    D3D11_INPUT_ELEMENT_DESC spriteLayout[] =
    {
        { "POSITION",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",     0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",   0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "BITANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numSpriteElements = ARRAYSIZE(spriteLayout);

    // Layout for UI
    D3D11_INPUT_ELEMENT_DESC uiLayout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numUIElements = ARRAYSIZE(uiLayout);

    // Layout for 3D meshes
    D3D11_INPUT_ELEMENT_DESC meshLayout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numMeshElements = ARRAYSIZE(meshLayout);

    // --- Load Shaders ---
    m_spriteShader = std::make_unique<Shader>();
    if (!m_spriteShader->Load(m_device.Get(), L"Shaders/SpriteVS.hlsl", L"Shaders/SpritePS.hlsl", spriteLayout, numSpriteElements)) {
        MessageBox(hWnd, L"Failed to compile/load sprite shaders.", L"Error", MB_OK);
        return false;
    }
    m_meshShader = std::make_unique<Shader>();
    if (!m_meshShader->Load(m_device.Get(), L"Shaders/Basic3D_VS.hlsl", L"Shaders/Basic3D_PS.hlsl", meshLayout, numMeshElements)) {
        MessageBox(hWnd, L"Failed to compile/load mesh shaders.", L"Error", MB_OK);
        return false;
    }
    m_uiShader = std::make_unique<Shader>();
    if (!m_uiShader->Load(m_device.Get(), L"Shaders/UIShaderVS.hlsl", L"Shaders/UIShaderPS.hlsl", uiLayout, numUIElements)) {
        MessageBox(hWnd, L"Failed to compile/load UI shaders.", L"Error", MB_OK);
        return false;
    }

#ifdef HOT_RELOAD_ENABLED
    // Create vectors from the layout arrays to store in the manager
    std::vector<D3D11_INPUT_ELEMENT_DESC> spriteLayoutVec(spriteLayout, spriteLayout + numSpriteElements);
    std::vector<D3D11_INPUT_ELEMENT_DESC> meshLayoutVec(meshLayout, meshLayout + numMeshElements);

    ShaderManager::GetInstance()->RegisterShader(m_spriteShader.get(), spriteLayoutVec);
    ShaderManager::GetInstance()->RegisterShader(m_meshShader.get(), meshLayoutVec);
#endif

    // Create Constant Buffers
    D3D11_BUFFER_DESC cbd = {};
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;


    // Timer Base Buffer
    cbd.ByteWidth = sizeof(BaseConstants);
    hr = m_device->CreateBuffer(&cbd, nullptr, &m_baseConstantBuffer);
    if (FAILED(hr)) { MessageBox(hWnd, L"Failed to BaseConstants frame constant buffer.", L"Error", MB_OK); return false; }

    // Frame CBuffer
    cbd.ByteWidth = sizeof(FrameConstants);
    hr = m_device->CreateBuffer(&cbd, nullptr, &m_frameConstantBuffer);
    if (FAILED(hr)) { MessageBox(hWnd, L"Failed to create frame constant buffer.", L"Error", MB_OK); return false; }

    // Object CBuffer
    cbd.ByteWidth = sizeof(ObjectConstants);
    hr = m_device->CreateBuffer(&cbd, nullptr, &m_objectConstantBuffer);
    if (FAILED(hr)) { MessageBox(hWnd, L"Failed to create object constant buffer.", L"Error", MB_OK); return false; }

    // Light CBuffer
    cbd.ByteWidth = sizeof(LightConstants);
    hr = m_device->CreateBuffer(&cbd, nullptr, &m_lightConstantBuffer);
    if (FAILED(hr)) { MessageBox(hWnd, L"Failed to create light constant buffer.", L"Error", MB_OK); return false; }

    // Create Sampler State
    D3D11_SAMPLER_DESC sd = {};
    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;// D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sd.MinLOD = 0;
    sd.MaxLOD = D3D11_FLOAT32_MAX;
    hr = m_device->CreateSamplerState(&sd, &m_samplerState);
    if (FAILED(hr)) { MessageBox(hWnd, L"Failed to create sampler state.", L"Error", MB_OK); return false; }

    // Create depth stencil state for UI (depth disabled)
    D3D11_DEPTH_STENCIL_DESC dsDescUI = {};
    dsDescUI.DepthEnable = false;
    dsDescUI.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDescUI.DepthFunc = D3D11_COMPARISON_LESS;
    dsDescUI.StencilEnable = false;
    hr = m_device->CreateDepthStencilState(&dsDescUI, &m_depthStencilStateUI);
    if (FAILED(hr)) { MessageBox(hWnd, L"Failed to create UI depth stencil state.", L"Error", MB_OK); return false; }

    // Create orthographic projection matrix for UI
    m_orthoProjectionMatrix = DirectX::XMMatrixOrthographicLH((float)width, (float)height, 0.0f, 1.0f);

    // Create blend state for UI
    D3D11_BLEND_DESC blendDesc = {};
    blendDesc.RenderTarget[0].BlendEnable = true;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    hr = m_device->CreateBlendState(&blendDesc, &m_alphaBlendState);
    if (FAILED(hr)) { MessageBox(hWnd, L"Failed to create alpha blend state.", L"Error", MB_OK); return false; }


    return true;
}

void CoreGraphicsManager::Shutdown()
{
    // Resources are released by ComPtr when this object is destroyed.
}

void CoreGraphicsManager::SetViewport(const D3D11_VIEWPORT& viewport)
{
    m_context->RSSetViewports(1, &viewport);
}

void CoreGraphicsManager::Clear(Camera::ClearFlags flags, const DirectX::XMFLOAT4& color, float depth, UINT8 stencil)
{
    if (flags == Camera::ClearFlags::SolidColor)
    {
        m_context->ClearRenderTargetView(m_renderTargetView.Get(), &color.x);
        m_context->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, stencil);
    }
    else if (flags == Camera::ClearFlags::DepthOnly)
    {
        m_context->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, stencil);
    }
    // If DontClear, do nothing.
}

void CoreGraphicsManager::BeginFrame(Camera* camera)
{
    if (!camera) return;


    // Timer Base Buffer
    D3D11_MAPPED_SUBRESOURCE mappedResource2;
    m_context->Map(m_baseConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource2);
    BaseConstants* baseptr = (BaseConstants*)mappedResource2.pData;
    baseptr->Timer = { fmodf( TimeManager::GetInstance()->GetTotalTime(), 1.f )
        , TimeManager::GetInstance()->GetDeltaTime()
        , 0
        , 0 };
    m_context->Unmap(m_baseConstantBuffer.Get(), 0);
    m_context->VSSetConstantBuffers(2, 1, m_baseConstantBuffer.GetAddressOf());
    //m_context->PSSetConstantBuffers(0, 1, m_frameConstantBuffer.GetAddressOf());



    // Update and set the frame constant buffer
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    m_context->Map(m_frameConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    FrameConstants* dataPtr = (FrameConstants*)mappedResource.pData;
    dataPtr->view = DirectX::XMMatrixTranspose(camera->GetViewMatrix());
    dataPtr->projection = DirectX::XMMatrixTranspose(camera->GetProjectionMatrix());
    m_context->Unmap(m_frameConstantBuffer.Get(), 0);

    m_context->VSSetConstantBuffers(0, 1, m_frameConstantBuffer.GetAddressOf());
}

void CoreGraphicsManager::UpdateLights(Camera* camera)
{
    // For now, find the first directional light and use it.
    // A more advanced system would handle multiple lights.
    const auto& lights = LightManager::GetInstance()->GetLights();
    Light* mainLight = nullptr;
    for (Light* light : lights)
    {
        if (light->GetType() == Light::LightType::Directional)
        {
            mainLight = light;
            break;
        }
    }

    LightConstants lightData = {};
    if (mainLight)
    {
        DirectX::XMStoreFloat3(&lightData.LightDirection, mainLight->GetOwner()->GetTransform()->GetForward());
        lightData.LightColor = mainLight->GetColor();
        // You might multiply color by intensity here
    }
    else
    {
        // Default light if none is in the scene
        lightData.LightDirection = { 0.0f, -1.0f, 0.0f };
        lightData.LightColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    }

    if (camera)
    {
        DirectX::XMStoreFloat3(&lightData.CameraPosition, camera->GetOwner()->GetTransform()->GetPosition());
    }

    // Update the constant buffer
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    m_context->Map(m_lightConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    memcpy(mappedResource.pData, &lightData, sizeof(LightConstants));
    m_context->Unmap(m_lightConstantBuffer.Get(), 0);
}

void CoreGraphicsManager::EndFrame()
{
    m_swapChain->Present(1, 0);
}

void CoreGraphicsManager::Draw(SpriteRenderer* renderer, Camera* camera)
{
    if (!renderer || !camera) return;

    // Get data from the renderer
    auto material = renderer->GetMaterial();
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer = renderer->GetVertexBuffer();
    Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer = renderer->GetIndexBuffer();

    if (!material || !vertexBuffer || !indexBuffer) return;

    // Set the material, which sets the shader, textures, etc.
    material->Apply(m_context.Get());

    // Update object constant buffer
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    m_context->Map(m_objectConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    ObjectConstants* dataPtr = (ObjectConstants*)mappedResource.pData;
    dataPtr->world = DirectX::XMMatrixTranspose(renderer->GetOwner()->GetTransform()->GetWorldMatrix());
    m_context->Unmap(m_objectConstantBuffer.Get(), 0);

    // Set resources
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    m_context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
    m_context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_context->VSSetConstantBuffers(1, 1, m_objectConstantBuffer.GetAddressOf());
    m_context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());

    // Draw
    m_context->DrawIndexed(6, 0, 0);
}

void CoreGraphicsManager::DrawMesh(MeshRenderer* renderer, Camera* camera)
{
    if (!renderer || !camera) return;

    // Get data from the renderer
    auto mesh = renderer->GetMesh();
    if (!mesh) return;

    auto vertexBuffer = mesh->GetVertexBuffer();
    auto indexBuffer = mesh->GetIndexBuffer();
    if (!vertexBuffer || !indexBuffer) return;

    // Get the material from the renderer
    auto material = renderer->GetMaterial();
    if (material)
    {
        material->Apply(m_context.Get());
    }
    else
    {
        // If no material, maybe use a default? For now, just set the shader.
        m_meshShader->Set(m_context.Get());
    }

    // Update object constant buffer
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    m_context->Map(m_objectConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    ObjectConstants* dataPtr = (ObjectConstants*)mappedResource.pData;
    dataPtr->world = DirectX::XMMatrixTranspose(renderer->GetOwner()->GetTransform()->GetWorldMatrix());
    m_context->Unmap(m_objectConstantBuffer.Get(), 0);

    // Set resources
    UINT stride = mesh->GetVertexStride();
    UINT offset = 0;
    m_context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    m_context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    m_context->VSSetConstantBuffers(1, 1, m_objectConstantBuffer.GetAddressOf());
    m_context->PSSetConstantBuffers(2, 1, m_lightConstantBuffer.GetAddressOf());
    m_context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());

    m_context->PSSetConstantBuffers(3, 1, m_baseConstantBuffer.GetAddressOf());

    // Draw
    m_context->DrawIndexed(mesh->GetIndexCount(), 0, 0);
}

void CoreGraphicsManager::DrawUI(UIImage* image)
{
    if (!image) return;

    // Get data from the renderer
    auto material = image->GetMaterial();
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer = image->GetVertexBuffer();
    Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer = image->GetIndexBuffer();

    if (!material || !vertexBuffer || !indexBuffer) return;

    // Set the material, which sets the shader, textures, etc.
    material->Apply(m_context.Get());

    // --- Switch to UI rendering mode ---
    // Set blend state for alpha blending
    float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    m_context->OMSetBlendState(m_alphaBlendState.Get(), blendFactor, 0xffffffff);
    // Disable depth testing
    m_context->OMSetDepthStencilState(m_depthStencilStateUI.Get(), 1);

    // Update frame constant buffer with ortho projection
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    m_context->Map(m_frameConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    FrameConstants* frameDataPtr = (FrameConstants*)mappedResource.pData;
    frameDataPtr->view = DirectX::XMMatrixIdentity(); // No view matrix for screen space UI
    frameDataPtr->projection = DirectX::XMMatrixTranspose(m_orthoProjectionMatrix);
    m_context->Unmap(m_frameConstantBuffer.Get(), 0);

    // Update object constant buffer
    m_context->Map(m_objectConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    ObjectConstants* objectDataPtr = (ObjectConstants*)mappedResource.pData;
    objectDataPtr->world = DirectX::XMMatrixTranspose(image->GetOwner()->GetTransform()->GetWorldMatrix());
    m_context->Unmap(m_objectConstantBuffer.Get(), 0);

    // Set resources
    UINT stride = sizeof(UIVertex);
    UINT offset = 0;
    m_context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
    m_context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_context->VSSetConstantBuffers(0, 1, m_frameConstantBuffer.GetAddressOf());
    m_context->VSSetConstantBuffers(1, 1, m_objectConstantBuffer.GetAddressOf());
    m_context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());

    // Draw
    m_context->DrawIndexed(6, 0, 0);

    // --- Restore default rendering mode ---
    // Re-enable depth testing
    m_context->OMSetDepthStencilState(nullptr, 1);
    // Restore default blend state
    m_context->OMSetBlendState(nullptr, blendFactor, 0xffffffff);
}
