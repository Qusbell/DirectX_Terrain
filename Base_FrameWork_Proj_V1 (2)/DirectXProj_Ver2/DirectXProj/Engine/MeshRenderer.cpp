#include "../Core/stdafx.h"
#include "MeshRenderer.h"
#include "../Core/CoreGraphicsManager.h" // For DrawMesh call
#include "Mesh.h"             // For GetMesh
#include "IRenderer.h"
//#include "Component.h"

#include "Material.h"


MeshRenderer::MeshRenderer(GameObject* owner)
    : IRenderer(owner), m_mesh(nullptr), m_material(nullptr)
{
    m_graphics = CoreGraphicsManager::GetI();
}

MeshRenderer::MeshRenderer(GameObject* owner, std::shared_ptr<Mesh> mesh)
    : IRenderer(owner), m_mesh(mesh), m_material(nullptr)
{
    m_graphics = CoreGraphicsManager::GetI();
}

void MeshRenderer::Render(Camera* camera)
{
    if (!m_mesh || !m_graphics)
    {
        return;
    }

    // The actual drawing logic is in the Graphics class.
    // This component just tells the Graphics class what to draw.
    m_graphics->DrawMesh(this, camera);
}

Mesh* MeshRenderer::GetMeshPoint() const
{
    return m_mesh.get();
}

Material* MeshRenderer::GetMaterialPoint() const
{
    return m_material.get();
}
