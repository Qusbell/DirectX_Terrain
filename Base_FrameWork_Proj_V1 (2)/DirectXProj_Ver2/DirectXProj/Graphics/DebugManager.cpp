#include "DebugManager.h"
#include "DebugRenderer.h"
#include "../Core/CoreGraphicsManager.h"
#include "Engine/Camera.h"
#include "DirectXHelpers.h"
#include "../Core/TimeManager.h"


DebugManager::DebugManager()
{
}

DebugManager::~DebugManager()
{
}

void DebugManager::CreateInitializeManager()
{
	
}

void DebugManager::Initialize(CoreGraphicsManager* graphics)
{
	m_Graphics = graphics;

	m_DebugRenderer = DebugRenderer::GetInstance();
	m_DebugRenderer->Initialize(m_Graphics->GetDevice(), m_Graphics->GetContext());
}

void DebugManager::DestroyManager()
{
	if(m_DebugRenderer)
	{
		m_DebugRenderer->Shutdown();
		m_DebugRenderer = nullptr;
	}
}

void DebugManager::Update()
{
}
void DebugManager::Render(Camera* camera)
{
	m_DebugRenderer->Render(camera);

}

void DebugManager::DrawLine2D_M(const DirectX::SimpleMath::Vector2& start, const DirectX::SimpleMath::Vector2& end, const DirectX::SimpleMath::Color& color)
{
	m_DebugRenderer->DrawLine2D(start, end, color);
}
void DebugManager::DrawRect_M(const RECT& rect, const DirectX::SimpleMath::Color& color)
{
	m_DebugRenderer->DrawRect(rect, color);
}
void DebugManager::DrawCircle2D_M(const DirectX::SimpleMath::Vector2& center, float radius, const DirectX::SimpleMath::Color& color, int segments)
{
	m_DebugRenderer->DrawCircle2D(center, radius, color, segments);
}
void DebugManager::DrawText_M(const std::wstring& text, const DirectX::SimpleMath::Vector2& position, const DirectX::SimpleMath::Color& color)
{
	m_DebugRenderer->DrawText_M(text, position, color);
}

// 3D Drawing
void DebugManager::DrawLine3D_M(const DirectX::SimpleMath::Vector3& start, const DirectX::SimpleMath::Vector3& end, const DirectX::SimpleMath::Color& color)
{
	m_DebugRenderer->DrawLine3D(start, end, color);
}
void DebugManager::DrawBox_M(const DirectX::SimpleMath::Vector3& center, const DirectX::SimpleMath::Vector3& extents, const DirectX::SimpleMath::Color& color)
{
	m_DebugRenderer->DrawBox(center, extents, color);
}
void DebugManager::DrawSphere_M(const DirectX::SimpleMath::Vector3& center, float radius, const DirectX::SimpleMath::Color& color, int segments)
{
	m_DebugRenderer->DrawSphere(center, radius, color, segments);
}
void DebugManager::DrawCylinder_M(const DirectX::SimpleMath::Vector3& base, const DirectX::SimpleMath::Vector3& top, float radius, const DirectX::SimpleMath::Color& color, int segments)
{
	m_DebugRenderer->DrawCylinder(base, top, radius, color, segments);
}
void DebugManager::DrawCapsule_M(const DirectX::SimpleMath::Vector3& start, const DirectX::SimpleMath::Vector3& end, float radius, const DirectX::SimpleMath::Color& color, int segments)
{
	m_DebugRenderer->DrawCapsule(start, end, radius, color, segments);
}
void DebugManager::DrawArrow_M(const DirectX::SimpleMath::Vector3& start, const DirectX::SimpleMath::Vector3& end, float tipSize, const DirectX::SimpleMath::Color& color)
{
	m_DebugRenderer->DrawArrow(start, end, tipSize, color);
}


void DebugManager::DrawLine2D(const DirectX::SimpleMath::Vector2& start, const DirectX::SimpleMath::Vector2& end, const DirectX::SimpleMath::Color& color)
{
	DebugManager::Instance().DrawLine2D_M(start, end, color);
}
void DebugManager::DrawRect(const RECT& rect, const DirectX::SimpleMath::Color& color)
{
	DebugManager::Instance().DrawRect_M(rect, color);
}
void DebugManager::DrawCircle2D(const DirectX::SimpleMath::Vector2& center, float radius, const DirectX::SimpleMath::Color& color, int segments)
{
	DebugManager::Instance().DrawCircle2D_M(center, radius, color, segments);
}
void DebugManager::DrawText(const std::wstring& text, const DirectX::SimpleMath::Vector2& position, const DirectX::SimpleMath::Color& color)
{
	DebugManager::Instance().DrawText_M(text, position, color);
}

// 3D Drawing
void DebugManager::DrawLine3D(const DirectX::SimpleMath::Vector3& start, const DirectX::SimpleMath::Vector3& end, const DirectX::SimpleMath::Color& color)
{
	DebugManager::Instance().DrawLine3D_M(start, end, color);
}
void DebugManager::DrawBox(const DirectX::SimpleMath::Vector3& center, const DirectX::SimpleMath::Vector3& extents, const DirectX::SimpleMath::Color& color)
{
	DebugManager::Instance().DrawBox_M(center, extents, color);
}
void DebugManager::DrawSphere(const DirectX::SimpleMath::Vector3& center, float radius, const DirectX::SimpleMath::Color& color, int segments )
{
	DebugManager::Instance().DrawSphere_M(center, radius, color, segments);
}
void DebugManager::DrawCylinder(const DirectX::SimpleMath::Vector3& base, const DirectX::SimpleMath::Vector3& top, float radius, const DirectX::SimpleMath::Color& color, int segments)
{
	DebugManager::Instance().DrawCylinder_M(base, top, radius, color, segments);
}
void DebugManager::DrawCapsule(const DirectX::SimpleMath::Vector3& start, const DirectX::SimpleMath::Vector3& end, float radius, const DirectX::SimpleMath::Color& color, int segments)
{
	DebugManager::Instance().DrawCapsule_M(start, end, radius, color, segments);
}
void DebugManager::DrawArrow(const DirectX::SimpleMath::Vector3& start, const DirectX::SimpleMath::Vector3& end, float tipSize, const DirectX::SimpleMath::Color& color)
{
	DebugManager::Instance().DrawArrow_M(start, end, tipSize, color);
}

void DebugManager::Test_DebugDrawSample()
{
	auto* debug = m_DebugRenderer;// DebugRenderer::GetInstance();

	// Draw a green box at the origin
	debug->DrawBox({ 0, 0, 0 }, { 0.5f, 0.5f, 0.5f }, DirectX::SimpleMath::Color(DirectX::Colors::LawnGreen));

	// Draw a sphere next to the existing box
	debug->DrawSphere({ -3.0f, 0.5f, 0.0f }, 0.5f, DirectX::SimpleMath::Color(DirectX::Colors::Aqua));

	// Draw an arrow pointing up
	debug->DrawArrow({ 0, 1.5f, 0 }, { 0, 2.5f, 0 }, 0.2f, DirectX::SimpleMath::Color(DirectX::Colors::Red));

	// Draw a cylinder
	debug->DrawCylinder({ 2.0f, 0.0f, 2.0f }, { 2.0f, 1.5f, 2.0f }, 0.4f, DirectX::SimpleMath::Color(DirectX::Colors::Magenta));

	// Draw a capsule
	debug->DrawCapsule({ -2.0f, 0.2f, 2.0f }, { -1.0f, 1.0f, 2.0f }, 0.3f, DirectX::SimpleMath::Color(DirectX::Colors::Orange));

	// Draw lines for the world axes
	debug->DrawLine3D({ 0,0,0 }, { 5,0,0 }, DirectX::SimpleMath::Color(DirectX::Colors::Red)); // X-axis
	debug->DrawLine3D({ 0,0,0 }, { 0,5,0 }, DirectX::SimpleMath::Color(DirectX::Colors::Green)); // Y-axis
	debug->DrawLine3D({ 0,0,0 }, { 0,0,5 }, DirectX::SimpleMath::Color(DirectX::Colors::Blue)); // Z-axis


	TimeManager* m_timeManager = TimeManager::GetInstance();
	// FPS Counter
	float fps = m_timeManager->GetFPS();
	std::wstring fpsText = L"FPS: " + std::to_wstring(static_cast<int>(fps));
	debug->DrawText_M(fpsText, { 10.f, 10.f }, DirectX::SimpleMath::Color(DirectX::Colors::Yellow));

	// --- End Debug Rendering ---
}
