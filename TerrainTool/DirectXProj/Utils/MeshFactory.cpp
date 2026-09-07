#include "../Core/stdafx.h"
#include "MeshFactory.h"
#include <vector>
#include <stdexcept>

// --- Existing CreateBox and CreateSphere implementations ---
std::shared_ptr<Mesh> MeshFactory::CreateBox(ID3D11Device* device, const DirectX::XMFLOAT3& size)
{
    // A box has 8 vertices and 6 faces. Each face has 2 triangles, so 12 triangles total.
    // Each triangle has 3 vertices, so 36 vertices in total if we want unique normals per face.
    // However, we can use 24 vertices and an index buffer.

    std::vector<Vertex3D> vertices;
    vertices.resize(24);

    float w = 0.5f * size.x;
    float h = 0.5f * size.y;
    float d = 0.5f * size.z;

    // Fill in the front face vertex data.
    vertices[0] = { { -w, -h, -d }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f } };
    vertices[1] = { { -w, +h, -d }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f } };
    vertices[2] = { { +w, +h, -d }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f } };
    vertices[3] = { { +w, -h, -d }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f } };

    // Fill in the back face vertex data.
    vertices[4] = { { -w, -h, +d }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } };
    vertices[5] = { { +w, -h, +d }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } };
    vertices[6] = { { +w, +h, +d }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } };
    vertices[7] = { { -w, +h, +d }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } };

    // Fill in the top face vertex data.
    vertices[8] = { { -w, +h, -d }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f } };
    vertices[9] = { { -w, +h, +d }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } };
    vertices[10] = { { +w, +h, +d }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } };
    vertices[11] = { { +w, +h, -d }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f } };

    // Fill in the bottom face vertex data.
    vertices[12] = { { -w, -h, -d }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } };
    vertices[13] = { { +w, -h, -d }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f } };
    vertices[14] = { { +w, -h, +d }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f } };
    vertices[15] = { { -w, -h, +d }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f } };

    // Fill in the left face vertex data.
    vertices[16] = { { -w, -h, +d }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } };
    vertices[17] = { { -w, +h, +d }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } };
    vertices[18] = { { -w, +h, -d }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } };
    vertices[19] = { { -w, -h, -d }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } };

    // Fill in the right face vertex data.
    vertices[20] = { { +w, -h, -d }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } };
    vertices[21] = { { +w, +h, -d }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } };
    vertices[22] = { { +w, +h, +d }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } };
    vertices[23] = { { +w, -h, +d }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } };

    std::vector<UINT> indices;
    indices.resize(36);

    // Fill in the front face index data
    indices[0] = 0; indices[1] = 1; indices[2] = 2;
    indices[3] = 0; indices[4] = 2; indices[5] = 3;

    // Fill in the back face index data
    indices[6] = 4; indices[7] = 5; indices[8] = 6;
    indices[9] = 4; indices[10] = 6; indices[11] = 7;

    // Fill in the top face index data
    indices[12] = 8; indices[13] = 9; indices[14] = 10;
    indices[15] = 8; indices[16] = 10; indices[17] = 11;

    // Fill in the bottom face index data
    indices[18] = 12; indices[19] = 13; indices[20] = 14;
    indices[21] = 12; indices[22] = 14; indices[23] = 15;

    // Fill in the left face index data
    indices[24] = 16; indices[25] = 17; indices[26] = 18;
    indices[27] = 16; indices[28] = 18; indices[29] = 19;

    // Fill in the right face index data
    indices[30] = 20; indices[31] = 21; indices[32] = 22;
    indices[33] = 20; indices[34] = 22; indices[35] = 23;

    auto mesh = std::make_shared<Mesh>();
    if (!mesh->Create(device, vertices, indices))
    {
        return nullptr;
    }

    return mesh;
}

std::shared_ptr<Mesh> MeshFactory::CreateSphere(ID3D11Device* device, float diameter, size_t tessellation)
{
    if (tessellation < 3) tessellation = 3;

    std::vector<Vertex3D> vertices;
    std::vector<UINT> indices;

    float radius = diameter / 2.0f;
    size_t verticalSegments = tessellation;
    size_t horizontalSegments = tessellation * 2;

    // Create rings of vertices at different latitudes.
    for (size_t i = 0; i <= verticalSegments; i++)
    {
        float v = 1.0f - (float)i / verticalSegments;

        float latitude = (i * DirectX::XM_PI / verticalSegments) - DirectX::XM_PIDIV2;
        float dy, dxz;
        DirectX::XMScalarSinCos(&dy, &dxz, latitude);

        for (size_t j = 0; j <= horizontalSegments; j++)
        {
            float u = (float)j / horizontalSegments;

            float longitude = j * DirectX::XM_2PI / horizontalSegments;
            float dx, dz;
            DirectX::XMScalarSinCos(&dx, &dz, longitude);

            dx *= dxz;
            dz *= dxz;

            DirectX::XMVECTOR normal = DirectX::XMVectorSet(dx, dy, dz, 0.f);
            DirectX::XMVECTOR position = DirectX::XMVectorScale(normal, radius);

            Vertex3D vert;
            XMStoreFloat3(&vert.pos, position);
            XMStoreFloat3(&vert.normal, normal);
            vert.uv = { u, v };
            vertices.push_back(vert);
        }
    }

    // Fill the index buffer
    size_t stride = horizontalSegments + 1;
    for (size_t i = 0; i < verticalSegments; i++)
    {
        for (size_t j = 0; j < horizontalSegments; j++)
        {
            size_t nextI = i + 1;
            size_t nextJ = j + 1;

            indices.push_back(i * stride + j);
            indices.push_back(nextI * stride + j);
            indices.push_back(i * stride + nextJ);

            indices.push_back(i * stride + nextJ);
            indices.push_back(nextI * stride + j);
            indices.push_back(nextI * stride + nextJ);
        }
    }

    auto mesh = std::make_shared<Mesh>();
    if (!mesh->Create(device, vertices, indices))
    {
        return nullptr;
    }

    return mesh;
}

std::shared_ptr<Mesh> MeshFactory::CreateQuad(ID3D11Device* device, const DirectX::XMFLOAT2& size)
{
    float halfWidth = size.x / 2.0f;
    float halfHeight = size.y / 2.0f;

    std::vector<Vertex3D> vertices = {
        { { -halfWidth, -halfHeight, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f } }, // Bottom-left
        { { -halfWidth,  halfHeight, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f } }, // Top-left
        { {  halfWidth,  halfHeight, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f } }, // Top-right
        { {  halfWidth, -halfHeight, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f } }  // Bottom-right
    };

    std::vector<unsigned int> indices = {
        0, 1, 2,
        0, 2, 3
    };

    //return std::make_shared<Mesh>(device, vertices, indices);

    auto mesh = std::make_shared<Mesh>();
    if (!mesh->Create(device, vertices, indices))
    {
        return nullptr;
    }

    return mesh;
}

std::shared_ptr<Mesh> MeshFactory::CreatePlane(ID3D11Device* device, const DirectX::XMFLOAT2& size, const DirectX::XMUINT2& tessellation)
{
    if (tessellation.x < 1 || tessellation.y < 1)
    {
        throw std::invalid_argument("Tessellation must be at least 1x1.");
    }

    std::vector<Vertex3D> vertices;
    std::vector<unsigned int> indices;

    unsigned int vertexCountX = tessellation.x + 1;
    unsigned int vertexCountY = tessellation.y + 1;

    float halfSizeX = size.x / 2.0f;
    float halfSizeY = size.y / 2.0f;

    float dx = size.x / tessellation.x;
    float dy = size.y / tessellation.y;

    float du = 1.0f / tessellation.x;
    float dv = 1.0f / tessellation.y;

    for (unsigned int j = 0; j < vertexCountY; ++j)
    {
        for (unsigned int i = 0; i < vertexCountX; ++i)
        {
            Vertex3D vertex;
            vertex.pos = { -halfSizeX + i * dx, 0.0f, halfSizeY - j * dy };
            vertex.normal = { 0.0f, 1.0f, 0.0f };
            vertex.uv = { i * du, j * dv };
            vertices.push_back(vertex);
        }
    }

    for (unsigned int j = 0; j < tessellation.y; ++j)
    {
        for (unsigned int i = 0; i < tessellation.x; ++i)
        {
            unsigned int topLeft = j * vertexCountX + i;
            unsigned int topRight = topLeft + 1;
            unsigned int bottomLeft = (j + 1) * vertexCountX + i;
            unsigned int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(topRight);
            indices.push_back(bottomLeft);

            indices.push_back(bottomLeft);
            indices.push_back(topRight);
            indices.push_back(bottomRight);
        }
    }

    //return std::make_shared<Mesh>(device, vertices, indices);
    auto mesh = std::make_shared<Mesh>();
    if (!mesh->Create(device, vertices, indices))
    {
        return nullptr;
    }

    return mesh;
}