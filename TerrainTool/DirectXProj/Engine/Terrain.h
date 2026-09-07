#pragma once
#include "Component.h"
#include "Mesh.h"
#include "SimpleMath.h"
#include <memory>
#include <string>
#include <vector>

class MeshRenderer;
class CoreGraphicsManager;

enum class TerrainBrushMode { RaiseLower, Flatten, Smooth };

struct TerrainBrush
{
    TerrainBrushMode mode = TerrainBrushMode::RaiseLower;
    float radius = 5.0f;
    float strength = 8.0f;
    float flattenHeight = 0.0f;
};

class Terrain : public Component
{
public:
    explicit Terrain(GameObject* owner);

    bool Initialize(float width = 100.0f, float depth = 100.0f, UINT columns = 129, UINT rows = 129);
    bool ApplyBrush(const DirectX::SimpleMath::Vector3& worldPoint, const TerrainBrush& brush, float deltaTime, bool invert);
    bool Raycast(const DirectX::SimpleMath::Ray& ray, DirectX::SimpleMath::Vector3& hitPoint) const;
    bool Save(const std::string& path, const TerrainBrush& brush) const;
    bool Load(const std::string& path, TerrainBrush& brush);
    void ResetHeights();
    void SetHeights(const std::vector<float>& heights);

    const std::vector<float>& GetHeights() const { return m_heights; }
    const std::shared_ptr<Mesh>& GetMesh() const { return m_mesh; }
    float GetWidth() const { return m_width; }
    float GetDepth() const { return m_depth; }
    UINT GetColumns() const { return m_columns; }
    UINT GetRows() const { return m_rows; }

private:
    void BuildMesh();
    void RecalculateNormals();
    void UploadMesh();
    size_t Index(UINT x, UINT z) const { return static_cast<size_t>(z) * m_columns + x; }

    CoreGraphicsManager* m_graphics = nullptr;
    MeshRenderer* m_renderer = nullptr;
    std::shared_ptr<Mesh> m_mesh;
    std::vector<Vertex3D> m_vertices;
    std::vector<UINT> m_indices;
    std::vector<float> m_heights;
    float m_width = 100.0f;
    float m_depth = 100.0f;
    UINT m_columns = 129;
    UINT m_rows = 129;
};
