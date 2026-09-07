#include "../Core/stdafx.h"
#include "Mesh.h"

bool Mesh::Create(
    ID3D11Device* device,
    const std::vector<Vertex3D>& vertices,
    const std::vector<UINT>& indices)
{
    if (!device || vertices.empty() || indices.empty())
    {
        return false;
    }

    m_vertices = vertices;
    m_indices = indices;
    m_indexCount = static_cast<UINT>(indices.size());

    // Create Vertex Buffer
    D3D11_BUFFER_DESC vbd = {};
    vbd.Usage = D3D11_USAGE_IMMUTABLE; // The mesh data won't change
    vbd.ByteWidth = sizeof(Vertex3D) * static_cast<UINT>(vertices.size());
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA vsd = {};
    vsd.pSysMem = vertices.data();

    HRESULT hr = device->CreateBuffer(&vbd, &vsd, &m_vertexBuffer);
    if (FAILED(hr))
    {
        // In a real engine, log the error
        return false;
    }

    // Create Index Buffer
    D3D11_BUFFER_DESC ibd = {};
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(UINT) * m_indexCount;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA isd = {};
    isd.pSysMem = indices.data();

    hr = device->CreateBuffer(&ibd, &isd, &m_indexBuffer);
    if (FAILED(hr))
    {
        // In a real engine, log the error
        return false;
    }

    // Create the Bounding Box from the mesh vertices
    DirectX::BoundingBox::CreateFromPoints(m_boundingBox, vertices.size(), &vertices[0].pos, sizeof(Vertex3D));

    return true;
}
