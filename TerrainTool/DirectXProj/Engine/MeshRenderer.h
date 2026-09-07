#pragma once
#include "IRenderer.h"
#include <memory>


// Forward declarations
class Mesh;
class Camera;
class CoreGraphicsManager;
class Material;

class MeshRenderer : public IRenderer
{
public:
    MeshRenderer(GameObject* owner);
    MeshRenderer(GameObject* owner, std::shared_ptr<Mesh> mesh);
    ~MeshRenderer() override = default;

    void Render(Camera* camera) override;

    void SetMesh(std::shared_ptr<Mesh> mesh) { m_mesh = mesh; }
    std::shared_ptr<Mesh> GetMesh() const { return m_mesh; }

    void SetMaterial(std::shared_ptr<Material> material) { m_material = material; }
    std::shared_ptr<Material> GetMaterial() const { return m_material; }

    // Raw pointer accessors
    Mesh* GetMeshPoint() const;
    Material* GetMaterialPoint() const;

private:
    std::shared_ptr<Mesh> m_mesh;
    std::shared_ptr<Material> m_material;
    CoreGraphicsManager* m_graphics = nullptr;
};
