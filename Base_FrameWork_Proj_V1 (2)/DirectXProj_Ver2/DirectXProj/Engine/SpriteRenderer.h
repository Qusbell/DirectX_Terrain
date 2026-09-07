#pragma once
#include "IRenderer.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <string>
#include <DirectXMath.h>
#include <memory>

// Forward declare to avoid including Graphics.h
class CoreGraphicsManager;
class Material;




struct Vertex_2DSPR
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 uv;
};

class SpriteRenderer : public IRenderer
{
public:
	SpriteRenderer(GameObject* owner);
	SpriteRenderer(GameObject* owner, std::shared_ptr<Material> material);
	~SpriteRenderer( ) override = default;

	void SetMaterial(std::shared_ptr<Material> material);
	std::shared_ptr<Material> GetMaterial( ) const { return m_material; }

	virtual void Start( ) override;
	virtual void Render(Camera* camera) override;
	virtual void ToJson(json& j) const override;
	virtual void FromJson(const json& j) override;

	ID3D11Buffer* GetVertexBuffer( ) const { return m_vertexBuffer.Get( ); }
	ID3D11Buffer* GetIndexBuffer( ) const { return m_indexBuffer.Get( ); }

private:
	void CreateBuffers( );

	CoreGraphicsManager* m_graphics; // Need access to the device
	std::shared_ptr<Material> m_material;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
};



