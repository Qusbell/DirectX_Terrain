#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <vector>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include "SimpleMath.h"

// A simple vertex struct for 3D models
struct Vertex3D
{
public:
    DirectX::SimpleMath::Vector3 pos;
    DirectX::SimpleMath::Vector3 normal;
    DirectX::SimpleMath::Vector2 uv;
};

class Mesh
{
public:
    Mesh() = default;
    ~Mesh() = default;

    bool Create(
        ID3D11Device* device,
        const std::vector<Vertex3D>& vertices,
        const std::vector<UINT>& indices,
        bool dynamicVertices = false);

    bool UpdateVertices(ID3D11DeviceContext* context, const std::vector<Vertex3D>& vertices);

    ID3D11Buffer* GetVertexBuffer() const { return m_vertexBuffer.Get(); }
    ID3D11Buffer* GetIndexBuffer() const { return m_indexBuffer.Get(); }
    UINT GetIndexCount() const { return m_indexCount; }
    UINT GetVertexStride() const { return sizeof(Vertex3D); }
    const DirectX::BoundingBox& GetBoundingBox() const { return m_boundingBox; }
    const std::vector<Vertex3D>& GetVertices() const { return m_vertices; }
    const std::vector<UINT>& GetIndices() const { return m_indices; }

private:
    DirectX::BoundingBox m_boundingBox;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
    UINT m_indexCount = 0;

    // Store CPU-side copies of mesh data for picking
    std::vector<Vertex3D> m_vertices;
    std::vector<UINT> m_indices;
    bool m_dynamicVertices = false;
};
