#include "UIImage.h"
#include "../Core/CoreGraphicsManager.h"
#include "../Graphics/Vertex.h"
#include "Material.h"
#include "GameObject.h"
#include "Transform.h"

UIImage::UIImage(GameObject* owner)
    : IRenderer(owner), m_graphics(CoreGraphicsManager::GetI())
{
    CreateQuad();
}

UIImage::UIImage(GameObject* owner, std::shared_ptr<Material> material)
    : IRenderer(owner), m_material(material), m_graphics(CoreGraphicsManager::GetI())
{
    CreateQuad();
}

void UIImage::Render(Camera* camera)
{
    // This will be handled by CoreGraphicsManager::DrawUI

	//if ( !m_vertexBuffer || !m_indexBuffer || !m_graphics || !m_material )
	//{
	//	return;
	//}

	//// Pass this renderer and the camera to the graphics system to be drawn.
	////m_graphics->Draw(this, camera);
	//m_graphics->DrawUI(this);
}

void UIImage::OnGUIRender(Camera* camera)
{
	if ( !this->GetOwner( )->IsActive( ) )
		return;

	if (!m_vertexBuffer || !m_indexBuffer || !m_graphics || !m_material)
	{
		return;
	}

	// Pass this renderer and the camera to the graphics system to be drawn.
	m_graphics->DrawUI(this);
}

void UIImage::SetMaterial(std::shared_ptr<Material> material)
{
    m_material = material;
}

void UIImage::CreateQuad()
{
    if (!m_graphics) return;

    // Define vertices for a quad
    UIVertex vertices[] = {
        { DirectX::SimpleMath::Vector3(-0.5f, -0.5f, 0.0f), DirectX::SimpleMath::Vector2(0.0f, 1.0f) },
        { DirectX::SimpleMath::Vector3(-0.5f,  0.5f, 0.0f), DirectX::SimpleMath::Vector2(0.0f, 0.0f) },
        { DirectX::SimpleMath::Vector3( 0.5f,  0.5f, 0.0f), DirectX::SimpleMath::Vector2(1.0f, 0.0f) },
        { DirectX::SimpleMath::Vector3( 0.5f, -0.5f, 0.0f), DirectX::SimpleMath::Vector2(1.0f, 1.0f) },
    };

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(UIVertex) * 4;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA sd = {};
    sd.pSysMem = vertices;

    HRESULT hr = m_graphics->GetDevice()->CreateBuffer(&bd, &sd, &m_vertexBuffer);
    if (FAILED(hr)) {
        // Handle error
        return;
    }

    // Define indices for the quad
    unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(unsigned int) * 6;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;

    sd.pSysMem = indices;

    hr = m_graphics->GetDevice()->CreateBuffer(&bd, &sd, &m_indexBuffer);
    if (FAILED(hr)) {
        // Handle error
    }
}