#pragma once
#include "Component.h"
#include "Terrain.h"
#include <deque>
#include <string>
#include <vector>

class Camera;

struct TerrainVertexChange { UINT index; float before; float after; };

class TerrainEditCommand
{
public:
    std::vector<TerrainVertexChange> changes;
    void Undo(Terrain& terrain) const;
    void Redo(Terrain& terrain) const;
};

class TerrainToolController : public Component
{
public:
    TerrainToolController(GameObject* owner, Terrain* terrain, Camera* camera);
    void Start() override;
    void Update() override;
    const TerrainBrush& GetBrush() const { return m_brush; }

private:
    void HandleShortcuts();
    void FinishStroke();
    void Undo();
    void Redo();
    void DrawHudAndBrush();
    std::wstring ModeName() const;

    Terrain* m_terrain;
    Camera* m_camera;
    class InputManager* m_input = nullptr;
    TerrainBrush m_brush;
    bool m_stroking = false;
    bool m_hasHover = false;
    DirectX::SimpleMath::Vector3 m_hoverPoint;
    std::vector<float> m_strokeBefore;
    std::deque<TerrainEditCommand> m_undo;
    std::deque<TerrainEditCommand> m_redo;
    std::string m_path = "Assets/Terrain/default.terrain.json";
    std::wstring m_status = L"Ready";
};
