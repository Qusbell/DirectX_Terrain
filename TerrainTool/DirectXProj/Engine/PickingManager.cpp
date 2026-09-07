#include "PickingManager.h"
#include "../Core/CoreGraphicsManager.h"
#include "GameObject.h"
#include "Scene.h"
#include "Camera.h"
#include "../Graphics/Shader.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "Transform.h"
#include "UIImage.h"
#include "Material.h"
#include "../Graphics/Texture.h"
#include "ScreenGrab.h"
#include "WICTextureLoader.h"
#include <wincodec.h>

struct PickingConstants
{
    DirectX::XMMATRIX world;
    DirectX::XMFLOAT4 id;
};

PickingManager::PickingManager()
    : m_graphicsManager(nullptr),
      m_nextId(1), // Start IDs from 1, 0 can be "nothing picked"
      m_showDebugThumbnail(false),
      m_debugThumbnailGO(nullptr),
      m_debugThumbnailImage(nullptr)
{
}

PickingManager::~PickingManager()
{
}

bool PickingManager::Initialize(CoreGraphicsManager* graphicsManager, Scene* scene)
{
    m_graphicsManager = graphicsManager;
    ID3D11Device* device = m_graphicsManager->GetDevice();

    // Create the picking shader
    m_pickingShader = std::make_unique<Shader>();
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = ARRAYSIZE(layout);
    if (!m_pickingShader->Load(device, L"Shaders/PickingVS.hlsl", L"Shaders/PickingPS.hlsl", layout, numElements))
    {
        MessageBox(nullptr, L"Failed to create picking shader", L"Error", MB_OK);
        return false;
    }

    // Create the render target texture
    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = m_graphicsManager->GetWindowWidth();
    texDesc.Height = m_graphicsManager->GetWindowHeight();
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    HRESULT hr = device->CreateTexture2D(&texDesc, nullptr, &m_pickingTexture);
    if (FAILED(hr)) return false;

    // Create the render target view
    hr = device->CreateRenderTargetView(m_pickingTexture.Get(), nullptr, &m_pickingRTV);
    if (FAILED(hr)) return false;

    // Create the shader resource view
    hr = device->CreateShaderResourceView(m_pickingTexture.Get(), nullptr, &m_pickingSRV);
    if (FAILED(hr)) return false;

    // Create the depth stencil texture
    D3D11_TEXTURE2D_DESC depthDesc = {};
    depthDesc.Width = texDesc.Width;
    depthDesc.Height = texDesc.Height;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
    hr = device->CreateTexture2D(&depthDesc, nullptr, &depthStencilBuffer);
    if (FAILED(hr)) return false;

    // Create the depth stencil view
    hr = device->CreateDepthStencilView(depthStencilBuffer.Get(), nullptr, &m_pickingDSV);
    if (FAILED(hr)) return false;

    // Create a CPU-readable texture for picking
    texDesc.Usage = D3D11_USAGE_STAGING;
    texDesc.BindFlags = 0;
    texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    hr = device->CreateTexture2D(&texDesc, nullptr, &m_cpuReadableTexture);
    if (FAILED(hr)) return false;

    // Create the picking constant buffer
    D3D11_BUFFER_DESC cbd = {};
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbd.ByteWidth = sizeof(PickingConstants);
    hr = device->CreateBuffer(&cbd, nullptr, &m_pickingConstantBuffer);
    if (FAILED(hr)) return false;

    // Create debug thumbnail resources
    m_debugThumbnailGO = scene->AddGameObject("Picking_Debug_Thumbnail");
    m_debugThumbnailImage = m_debugThumbnailGO->AddComponent<UIImage>();
	//m_debugThumbnailGO->GetTransform( )->SetLocalPosition(1.5f, 0.0f, 0.0f);

    m_debugMaterial = std::make_shared<Material>(device);
    m_debugMaterial->SetShader(graphicsManager->GetUIShader());

    m_debugTexture = std::make_shared<Texture>();
    m_debugTexture->Initialize(m_pickingSRV.Get());

    m_debugMaterial->SetAlbedoTexture(m_debugTexture);
    m_debugThumbnailImage->SetMaterial(m_debugMaterial);
    //m_debugThumbnailImage->Initialize(graphicsManager); // No longer needed, handled by constructor
    //m_debugThumbnailGO->SetActive(false); // Start disabled


    return true;
}

void PickingManager::OnDestroy()
{
}

void PickingManager::RenderPickingPass(Scene* scene, Camera* camera)
{
    ID3D11DeviceContext* context = m_graphicsManager->GetContext();

    // Set render target to the picking texture
    context->OMSetRenderTargets(1, m_pickingRTV.GetAddressOf(), m_pickingDSV.Get());

    // Clear the views
    const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; // Clear to black (ID 0)
    context->ClearRenderTargetView(m_pickingRTV.Get(), clearColor);
    context->ClearDepthStencilView(m_pickingDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    // Set the picking shader
    m_pickingShader->Set(context);

    // Update and set the frame constant buffer
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    context->Map(m_graphicsManager->GetFrameConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    FrameConstants* dataPtr = (FrameConstants*)mappedResource.pData;
    dataPtr->view = DirectX::XMMatrixTranspose(camera->GetViewMatrix());
    dataPtr->projection = DirectX::XMMatrixTranspose(camera->GetProjectionMatrix());
    context->Unmap(m_graphicsManager->GetFrameConstantBuffer(), 0);

	auto* frameConstantBuffer = m_graphicsManager->GetFrameConstantBuffer( );
    context->VSSetConstantBuffers(0, 1, &frameConstantBuffer );


    // Render all mesh renderers in the scene
    for (auto& go : scene->GetGameObjects())
    {
        if (go->IsActive())
        {
            MeshRenderer* renderer = go->GetComponent<MeshRenderer>();
            if (renderer)
            {
                // Set vertex and index buffers
                Mesh* mesh = renderer->GetMesh().get();
                if (mesh)
                {
                    UINT stride = mesh->GetVertexStride();
                    UINT offset = 0;
                    auto vertexBuffer = mesh->GetVertexBuffer();
                    context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
                    context->IASetIndexBuffer(mesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
                    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

                    // Update constant buffer with world matrix and ID
                    D3D11_MAPPED_SUBRESOURCE mappedResource;
                    context->Map(m_pickingConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
                    PickingConstants* constants = (PickingConstants*)mappedResource.pData;
                    constants->world = DirectX::XMMatrixTranspose(go->GetTransform()->GetWorldMatrix());
                    unsigned int id = go->GetPickingID();
                    float r = (id & 0xFF) / 255.0f;
                    float g = ((id >> 8) & 0xFF) / 255.0f;
                    float b = ((id >> 16) & 0xFF) / 255.0f;
                    constants->id = { r, g, b, 1.0f };
                    context->Unmap(m_pickingConstantBuffer.Get(), 0);

                    context->VSSetConstantBuffers(1, 1, m_pickingConstantBuffer.GetAddressOf());
                    context->PSSetConstantBuffers(1, 1, m_pickingConstantBuffer.GetAddressOf());

                    // Draw the object
                    context->DrawIndexed(mesh->GetIndexCount(), 0, 0);
                }
            }
        }
    }

    // Restore the original render target and depth stencil
    ID3D11RenderTargetView* mainRTV = m_graphicsManager->GetRenderTargetView();
    ID3D11DepthStencilView* mainDSV = m_graphicsManager->GetDepthStencilView();
    context->OMSetRenderTargets(1, &mainRTV, mainDSV);
}

GameObject* PickingManager::Pick(const DirectX::SimpleMath::Vector2& screenPos)
{
    ID3D11DeviceContext* context = m_graphicsManager->GetContext();

    // Copy the picking texture to the CPU-readable texture
    context->CopyResource(m_cpuReadableTexture.Get(), m_pickingTexture.Get());

    // Map the texture to read the pixel
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = context->Map(m_cpuReadableTexture.Get(), 0, D3D11_MAP_READ, 0, &mappedResource);
    if (FAILED(hr))
    {
        return nullptr;
    }

    const unsigned char* pPixels = reinterpret_cast<const unsigned char*>(mappedResource.pData);
    int pitch = mappedResource.RowPitch;
    int x = static_cast<int>(screenPos.x);
    int y = static_cast<int>(screenPos.y);

    // Get the color of the pixel at the specified coordinates
    unsigned char r = pPixels[y * pitch + x * 4];
    unsigned char g = pPixels[y * pitch + x * 4 + 1];
    unsigned char b = pPixels[y * pitch + x * 4 + 2];

    context->Unmap(m_cpuReadableTexture.Get(), 0);

    // Convert the color back to an ID
    unsigned int pickedID = r | (g << 8) | (b << 16);

    // Find the game object with this ID
    auto it = m_idToObjectMap.find(pickedID);
    if (it != m_idToObjectMap.end())
    {
        return it->second;
    }

    return nullptr;
}

void PickingManager::RegisterObject(GameObject* obj)
{
    if (obj)
    {
        unsigned int id = m_nextId++;
        obj->SetPickingID(id);
        m_idToObjectMap[id] = obj;
    }
}

void PickingManager::UnregisterObject(GameObject* obj)
{
    if (obj)
    {
        m_idToObjectMap.erase(obj->GetPickingID());
    }
}

void PickingManager::Update()
{
    if (!m_debugThumbnailGO) return;

    if (m_showDebugThumbnail)
    {
        if (!m_debugThumbnailGO->IsActive())
        {
           m_debugThumbnailGO->SetActive(true);
        }

        int screenWidth = m_graphicsManager->GetWindowWidth();
        int screenHeight = m_graphicsManager->GetWindowHeight();

        float thumbWidth = screenWidth / 4.0f;
        float thumbHeight = screenHeight / 4.0f;

        Transform* transform = m_debugThumbnailGO->GetTransform();
        // The ortho matrix has (0,0) at the center.
        // To place the thumbnail at the bottom-right, we need to calculate its center position relative to the screen center.
        float posX = (screenWidth / 2.0f) - (thumbWidth / 2.0f);
        float posY = (-screenHeight / 2.0f) + (thumbHeight / 2.0f);
        transform->SetLocalPosition(posX, posY, 0);
        transform->SetLocalScale(thumbWidth, thumbHeight, 1.0f);
    }
    else
    {
        if (m_debugThumbnailGO->IsActive())
        {
           m_debugThumbnailGO->SetActive(false);
        }
    }
}

void PickingManager::SavePickingTextureToFile(const std::wstring& filePath)
{
    ID3D11DeviceContext* context = m_graphicsManager->GetContext();

    // Copy the picking texture to the CPU-readable texture first
    context->CopyResource(m_cpuReadableTexture.Get(), m_pickingTexture.Get());

    HRESULT hr = DirectX::SaveWICTextureToFile(context, m_cpuReadableTexture.Get(), GUID_ContainerFormatPng, filePath.c_str());
    if (FAILED(hr))
    {
        OutputDebugStringA("Failed to save picking texture to file.\n");
    }
}