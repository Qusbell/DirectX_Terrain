#pragma once
#include <memory>
#include <DirectXMath.h>
#include <d3d11.h>
#include "../Engine/Mesh.h" // For Mesh and Vertex3D

class Mesh; // Forward declaration
class MeshFactory
{
public:
    // Prevent instantiation
    MeshFactory() = delete;

    static std::shared_ptr<Mesh> CreateBox(
        ID3D11Device* device,
        const DirectX::XMFLOAT3& size = { 1.0f, 1.0f, 1.0f });

    static std::shared_ptr<Mesh> CreateSphere(
        ID3D11Device* device,
        float diameter = 1.0f,
        size_t tessellation = 16);

    static std::shared_ptr<Mesh> CreateQuad(
        ID3D11Device* device,
        const DirectX::XMFLOAT2& size = { 1.0f, 1.0f });

    static std::shared_ptr<Mesh> CreatePlane(
        ID3D11Device* device,
        const DirectX::XMFLOAT2& size = { 10.0f, 10.0f },
        const DirectX::XMUINT2& tessellation = { 10, 10 });
};