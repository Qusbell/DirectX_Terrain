#include "../Core/stdafx.h"
#include "Terrain.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "../Core/CoreGraphicsManager.h"
#include "../Graphics/TextureManager.h"
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <limits>

Terrain::Terrain(GameObject* owner) : Component(owner), m_graphics(CoreGraphicsManager::GetI()) {}

bool Terrain::Initialize(float width, float depth, UINT columns, UINT rows)
{
    if (width <= 0.0f || depth <= 0.0f || columns < 2 || rows < 2) return false;
    m_width = width; m_depth = depth; m_columns = columns; m_rows = rows;
    m_heights.assign(static_cast<size_t>(columns) * rows, 0.0f);
    BuildMesh();
    if (!m_mesh) return false;

    m_renderer = GetOwner()->GetComponent<MeshRenderer>();
    if (!m_renderer) m_renderer = GetOwner()->AddComponent<MeshRenderer>();
    m_renderer->SetMesh(m_mesh);
    auto material = std::make_shared<Material>(m_graphics->GetDevice());
    material->SetShader(m_graphics->GetMeshShader());
    material->SetAlbedoTexture(TextureManager::GetInstance()->LoadShared(m_graphics->GetDevice(), L"Texture/TerrainGrid.png"));
    material->SetTiling({ 20.0f, 20.0f });
    m_renderer->SetMaterial(material);
    return true;
}

void Terrain::BuildMesh()
{
    m_vertices.clear(); m_indices.clear();
    m_vertices.reserve(static_cast<size_t>(m_columns) * m_rows);
    const float dx = m_width / (m_columns - 1), dz = m_depth / (m_rows - 1);
    for (UINT z = 0; z < m_rows; ++z)
        for (UINT x = 0; x < m_columns; ++x)
            m_vertices.push_back({ { -m_width * 0.5f + x * dx, m_heights[Index(x,z)], m_depth * 0.5f - z * dz }, {0,1,0}, { float(x)/(m_columns-1), float(z)/(m_rows-1) } });
    for (UINT z = 0; z + 1 < m_rows; ++z) for (UINT x = 0; x + 1 < m_columns; ++x)
    {
        const UINT tl = z*m_columns+x, tr=tl+1, bl=(z+1)*m_columns+x, br=bl+1;
        m_indices.insert(m_indices.end(), { tl,tr,bl, bl,tr,br });
    }
    RecalculateNormals();
    m_mesh = std::make_shared<Mesh>();
    if (!m_mesh->Create(m_graphics->GetDevice(), m_vertices, m_indices, true)) m_mesh.reset();
}

void Terrain::RecalculateNormals()
{
    const float dx = m_width/(m_columns-1), dz=m_depth/(m_rows-1);
    for (UINT z=0; z<m_rows; ++z) for (UINT x=0; x<m_columns; ++x)
    {
        const UINT right = (x + 1 < m_columns) ? x + 1 : m_columns - 1;
        const UINT down = (z + 1 < m_rows) ? z + 1 : m_rows - 1;
        const float l=m_heights[Index(x?x-1:x,z)], r=m_heights[Index(right,z)];
        const float u=m_heights[Index(x,z?z-1:z)], d=m_heights[Index(x,down)];
        DirectX::SimpleMath::Vector3 n(l-r, 2.0f*dx, d-u);
        n.Normalize(); m_vertices[Index(x,z)].normal=n;
    }
}

void Terrain::UploadMesh()
{
    for (size_t i=0; i<m_heights.size(); ++i) m_vertices[i].pos.y=m_heights[i];
    RecalculateNormals();
    m_mesh->UpdateVertices(m_graphics->GetContext(), m_vertices);
}

bool Terrain::ApplyBrush(const DirectX::SimpleMath::Vector3& p, const TerrainBrush& brush, float dt, bool invert)
{
    bool changed=false; std::vector<float> source=m_heights;
    const float dx=m_width/(m_columns-1), dz=m_depth/(m_rows-1);
    for (UINT z=0; z<m_rows; ++z) for (UINT x=0; x<m_columns; ++x)
    {
        const size_t i=Index(x,z); float vx=-m_width*.5f+x*dx, vz=m_depth*.5f-z*dz;
        float dist=std::sqrt((vx-p.x)*(vx-p.x)+(vz-p.z)*(vz-p.z));
        if (dist>brush.radius) continue;
        float falloff=1.0f-dist/brush.radius, amount=brush.strength*dt*falloff;
        float next=source[i];
        if (brush.mode==TerrainBrushMode::RaiseLower) next += (invert?-amount:amount);
        else if (brush.mode==TerrainBrushMode::Flatten) next += (brush.flattenHeight-next)*((amount < 1.0f) ? amount : 1.0f);
        else
        {
            float sum=0; int count=0;
            for (int oz=-1;oz<=1;++oz) for(int ox=-1;ox<=1;++ox) { int sx=int(x)+ox, sz=int(z)+oz; if(sx>=0&&sz>=0&&sx<int(m_columns)&&sz<int(m_rows)){sum+=source[Index(sx,sz)];++count;} }
            next += ((sum/count)-next)*((amount < 1.0f) ? amount : 1.0f);
        }
        if (std::abs(next-m_heights[i])>0.00001f) { m_heights[i]=next; changed=true; }
    }
    if(changed) UploadMesh(); return changed;
}

bool Terrain::Raycast(const DirectX::SimpleMath::Ray& ray, DirectX::SimpleMath::Vector3& hit) const
{
    float nearest=(std::numeric_limits<float>::max)(); bool found=false;
    for(size_t i=0;i+2<m_indices.size();i+=3)
    {
        float distance=0; const auto&a=m_vertices[m_indices[i]].pos; const auto&b=m_vertices[m_indices[i+1]].pos; const auto&c=m_vertices[m_indices[i+2]].pos;
        if(DirectX::TriangleTests::Intersects(ray.position,ray.direction,a,b,c,distance)&&distance<nearest){nearest=distance;found=true;}
    }
    if(found) hit=ray.position+ray.direction*nearest; return found;
}

bool Terrain::Save(const std::string& path, const TerrainBrush& brush) const
{
    try { std::filesystem::path p(path); if(p.has_parent_path()) std::filesystem::create_directories(p.parent_path());
        json j={{"version",1},{"width",m_width},{"depth",m_depth},{"columns",m_columns},{"rows",m_rows},{"heights",m_heights},
          {"brush",{{"mode",int(brush.mode)},{"radius",brush.radius},{"strength",brush.strength},{"flattenHeight",brush.flattenHeight}}}};
        std::ofstream f(path); if(!f) return false; f<<j.dump(2); return bool(f); } catch(...) { return false; }
}

bool Terrain::Load(const std::string& path, TerrainBrush& brush)
{
    try { std::ifstream f(path); if(!f)return false; json j; f>>j;
        if(j.at("version").get<int>()!=1)return false; float w=j.at("width"),d=j.at("depth"); UINT c=j.at("columns"),r=j.at("rows"); auto h=j.at("heights").get<std::vector<float>>();
        if(w<=0||d<=0||c<2||r<2||h.size()!=size_t(c)*r||!std::all_of(h.begin(),h.end(),[](float v){return std::isfinite(v);}))return false;
        TerrainBrush next=brush; auto b=j.at("brush"); int mode=b.at("mode"); if(mode<0||mode>2)return false; next.mode=TerrainBrushMode(mode); next.radius=b.at("radius"); next.strength=b.at("strength"); next.flattenHeight=b.at("flattenHeight");
        if(!std::isfinite(next.radius)||!std::isfinite(next.strength)||next.radius<=0||next.strength<=0)return false;
        m_width=w;m_depth=d;m_columns=c;m_rows=r;m_heights=std::move(h);brush=next;BuildMesh(); if(!m_mesh)return false; if(m_renderer)m_renderer->SetMesh(m_mesh); return true; } catch(...) { return false; }
}

void Terrain::ResetHeights(){std::fill(m_heights.begin(),m_heights.end(),0.0f);UploadMesh();}
void Terrain::SetHeights(const std::vector<float>& h){if(h.size()==m_heights.size()){m_heights=h;UploadMesh();}}
