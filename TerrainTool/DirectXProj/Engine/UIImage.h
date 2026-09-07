#pragma once
#include "Component.h"
#include "IRenderer.h"
#include <memory>
#include <d3d11.h>
#include <wrl/client.h>


class Material;

//struct Vertex
//{
//	DirectX::XMFLOAT3 pos;
//	DirectX::XMFLOAT2 uv;
//};

class UIImage : public IRenderer
{
public:
    UIImage(GameObject* owner);
    UIImage(GameObject* owner, std::shared_ptr<Material> material);
    ~UIImage() override = default;

    void Render(Camera* camera) override;
	virtual void OnGUIRender(Camera* camera) override;

    void SetMaterial(std::shared_ptr<Material> material);
    std::shared_ptr<Material> GetMaterial() const { return m_material; }

    Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer() const { return m_vertexBuffer; }
    Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer() const { return m_indexBuffer; }

private:
    void CreateQuad();

    std::shared_ptr<Material> m_material;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
    CoreGraphicsManager* m_graphics;
};