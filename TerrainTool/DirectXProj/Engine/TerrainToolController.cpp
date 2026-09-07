#include "../Core/stdafx.h"
#include "TerrainToolController.h"
#include "Camera.h"
#include "../Input/InputManager.h"
#include "../Core/TimeManager.h"
#include "../Graphics/DebugManager.h"
#include <algorithm>
#include <sstream>

void TerrainEditCommand::Undo(Terrain& terrain) const { auto h=terrain.GetHeights(); for(const auto&c:changes)h[c.index]=c.before; terrain.SetHeights(h); }
void TerrainEditCommand::Redo(Terrain& terrain) const { auto h=terrain.GetHeights(); for(const auto&c:changes)h[c.index]=c.after; terrain.SetHeights(h); }

TerrainToolController::TerrainToolController(GameObject* owner, Terrain* terrain, Camera* camera)
    : Component(owner), m_terrain(terrain), m_camera(camera) {}

void TerrainToolController::Start(){m_input=InputManager::GetInstance();}

void TerrainToolController::Update()
{
    if(!m_input||!m_terrain||!m_camera)return;
    HandleShortcuts();
    DirectX::SimpleMath::Vector2 mouse(float(m_input->GetMouseX()),float(m_input->GetMouseY()));
    m_hasHover=m_terrain->Raycast(m_camera->ScreenPointToRay(mouse),m_hoverPoint);
    const bool orbit=m_input->IsKeyPressed(VK_MENU);
    if(m_input->IsKeyDown(VK_LBUTTON)&&m_hasHover&&!orbit){m_stroking=true;m_strokeBefore=m_terrain->GetHeights();if(m_brush.mode==TerrainBrushMode::Flatten)m_brush.flattenHeight=m_hoverPoint.y;}
    if(m_stroking&&m_input->IsKeyPressed(VK_LBUTTON)&&m_hasHover&&!orbit)
        m_terrain->ApplyBrush(m_hoverPoint,m_brush,TimeManager::GetInstance()->GetDeltaTime(),m_input->IsKeyPressed(VK_SHIFT));
    if(m_stroking&&m_input->IsKeyUp(VK_LBUTTON))FinishStroke();
    DrawHudAndBrush();
}

void TerrainToolController::HandleShortcuts()
{
    if(m_input->IsKeyDown('1'))m_brush.mode=TerrainBrushMode::RaiseLower;
    if(m_input->IsKeyDown('2'))m_brush.mode=TerrainBrushMode::Flatten;
    if(m_input->IsKeyDown('3'))m_brush.mode=TerrainBrushMode::Smooth;
    if(m_input->IsKeyDown(VK_OEM_4))m_brush.radius=(m_brush.radius-0.5f>0.5f)?m_brush.radius-0.5f:0.5f;
    if(m_input->IsKeyDown(VK_OEM_6))m_brush.radius=(m_brush.radius+0.5f<50.0f)?m_brush.radius+0.5f:50.0f;
    if(m_input->IsKeyDown(VK_OEM_MINUS))m_brush.strength=(m_brush.strength-0.5f>0.25f)?m_brush.strength-0.5f:0.25f;
    if(m_input->IsKeyDown(VK_OEM_PLUS))m_brush.strength=(m_brush.strength+0.5f<100.0f)?m_brush.strength+0.5f:100.0f;
    const bool ctrl=m_input->IsKeyPressed(VK_CONTROL);
    if(ctrl&&m_input->IsKeyDown('Z'))Undo();
    if(ctrl&&m_input->IsKeyDown('Y'))Redo();
    if(ctrl&&m_input->IsKeyDown('S'))m_status=m_terrain->Save(m_path,m_brush)?L"Saved default.terrain.json":L"Save failed";
    if(ctrl&&m_input->IsKeyDown('O')){if(m_terrain->Load(m_path,m_brush)){m_undo.clear();m_redo.clear();m_status=L"Loaded default.terrain.json";}else m_status=L"Load rejected; terrain preserved";}
    if(ctrl&&m_input->IsKeyDown('N')){m_terrain->ResetHeights();m_undo.clear();m_redo.clear();m_status=L"New flat terrain";}
}

void TerrainToolController::FinishStroke()
{
    TerrainEditCommand command; const auto&after=m_terrain->GetHeights();
    for(UINT i=0;i<after.size();++i)if(std::abs(after[i]-m_strokeBefore[i])>0.00001f)command.changes.push_back({i,m_strokeBefore[i],after[i]});
    if(!command.changes.empty()){m_undo.push_back(std::move(command));if(m_undo.size()>64)m_undo.pop_front();m_redo.clear();m_status=L"Terrain modified";}
    m_strokeBefore.clear();m_stroking=false;
}

void TerrainToolController::Undo(){if(m_undo.empty())return;auto c=std::move(m_undo.back());m_undo.pop_back();c.Undo(*m_terrain);m_redo.push_back(std::move(c));m_status=L"Undo";}
void TerrainToolController::Redo(){if(m_redo.empty())return;auto c=std::move(m_redo.back());m_redo.pop_back();c.Redo(*m_terrain);m_undo.push_back(std::move(c));m_status=L"Redo";}

std::wstring TerrainToolController::ModeName()const{return m_brush.mode==TerrainBrushMode::RaiseLower?L"Raise/Lower":m_brush.mode==TerrainBrushMode::Flatten?L"Flatten":L"Smooth";}

void TerrainToolController::DrawHudAndBrush()
{
    std::wostringstream line;line<<L"Terrain Tool | "<<ModeName()<<L" | Radius "<<m_brush.radius<<L" | Strength "<<m_brush.strength;
    const DirectX::SimpleMath::Color white(1,1,1,1), gray(.75f,.75f,.75f,1), yellow(1,1,0,1);
    DebugManager::DrawText(line.str(),{16,16},white);
    DebugManager::DrawText(L"1 Raise/Lower  2 Flatten  3 Smooth   [ ] Radius   - = Strength",{16,40},gray);
    DebugManager::DrawText(L"LMB Sculpt  Shift+LMB Invert  RMB Rotate  MMB Pan  Wheel Zoom",{16,62},gray);
    DebugManager::DrawText(L"Ctrl+S Save  Ctrl+O Load  Ctrl+N New  Ctrl+Z/Y Undo/Redo",{16,84},gray);
    DebugManager::DrawText(m_status,{16,108},yellow);
    if(m_hasHover)
    {
        constexpr int segments=48;const DirectX::SimpleMath::Color color(1.0f,0.45f,0.0f,1.0f);
        for(int i=0;i<segments;++i){float a=float(i)*DirectX::XM_2PI/segments,b=float(i+1)*DirectX::XM_2PI/segments;
            auto p0=m_hoverPoint+DirectX::SimpleMath::Vector3(std::cos(a)*m_brush.radius,0.08f,std::sin(a)*m_brush.radius);
            auto p1=m_hoverPoint+DirectX::SimpleMath::Vector3(std::cos(b)*m_brush.radius,0.08f,std::sin(b)*m_brush.radius);DebugManager::DrawLine3D(p0,p1,color);}
    }
}
