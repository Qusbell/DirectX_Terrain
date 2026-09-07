#pragma once
#include "../Core/SingletonT.h"
#include <DirectXMath.h>
#include "SimpleMath.h"

#pragma comment(lib, "DirectXTK.lib")

class DebugRenderer;
class CoreGraphicsManager;
class Camera;

class DebugManager : public SingletonT<DebugManager>
{
public:
	DebugManager();
	virtual ~DebugManager();
public:
	void Initialize( CoreGraphicsManager* graphics );
	virtual void CreateInitializeManager( ) override;
	virtual void DestroyManager( ) override;
    DebugRenderer* GetDebugRenderer() { return m_DebugRenderer; }


public:
	void Update();
	void Render(Camera* camera);

public:
    // 2D Drawing
    void DrawLine2D_M(const DirectX::SimpleMath::Vector2& start, const DirectX::SimpleMath::Vector2& end, const DirectX::SimpleMath::Color& color);
    void DrawRect_M(const RECT& rect, const DirectX::SimpleMath::Color& color);
    void DrawCircle2D_M(const DirectX::SimpleMath::Vector2& center, float radius, const DirectX::SimpleMath::Color& color, int segments = 16);
    void DrawText_M(const std::wstring& text, const DirectX::SimpleMath::Vector2& position, const DirectX::SimpleMath::Color& color);

    // 3D Drawing
    void DrawLine3D_M(const DirectX::SimpleMath::Vector3& start, const DirectX::SimpleMath::Vector3& end, const DirectX::SimpleMath::Color& color);
    void DrawBox_M(const DirectX::SimpleMath::Vector3& center, const DirectX::SimpleMath::Vector3& extents, const DirectX::SimpleMath::Color& color);
    void DrawSphere_M(const DirectX::SimpleMath::Vector3& center, float radius, const DirectX::SimpleMath::Color& color, int segments = 16);
    void DrawCylinder_M(const DirectX::SimpleMath::Vector3& base, const DirectX::SimpleMath::Vector3& top, float radius, const DirectX::SimpleMath::Color& color, int segments = 16);
    void DrawCapsule_M(const DirectX::SimpleMath::Vector3& start, const DirectX::SimpleMath::Vector3& end, float radius, const DirectX::SimpleMath::Color& color, int segments = 16);
    void DrawArrow_M(const DirectX::SimpleMath::Vector3& start, const DirectX::SimpleMath::Vector3& end, float tipSize, const DirectX::SimpleMath::Color& color);


    //// 2D Drawing
    static void DrawLine2D(const DirectX::SimpleMath::Vector2& start, const DirectX::SimpleMath::Vector2& end, const DirectX::SimpleMath::Color& color);
    static void DrawRect(const RECT& rect, const DirectX::SimpleMath::Color& color);
    static void DrawCircle2D(const DirectX::SimpleMath::Vector2& center, float radius, const DirectX::SimpleMath::Color& color, int segments = 16);
    static void DrawText(const std::wstring& text, const DirectX::SimpleMath::Vector2& position, const DirectX::SimpleMath::Color& color);

    // 3D Drawing
    static void DrawLine3D(const DirectX::SimpleMath::Vector3& start, const DirectX::SimpleMath::Vector3& end, const DirectX::SimpleMath::Color& color);
    static void DrawBox(const DirectX::SimpleMath::Vector3& center, const DirectX::SimpleMath::Vector3& extents, const DirectX::SimpleMath::Color& color);
    static void DrawSphere(const DirectX::SimpleMath::Vector3& center, float radius, const DirectX::SimpleMath::Color& color, int segments = 16);
    static void DrawCylinder(const DirectX::SimpleMath::Vector3& base, const DirectX::SimpleMath::Vector3& top, float radius, const DirectX::SimpleMath::Color& color, int segments = 16);
    static void DrawCapsule(const DirectX::SimpleMath::Vector3& start, const DirectX::SimpleMath::Vector3& end, float radius, const DirectX::SimpleMath::Color& color, int segments = 16);
    static void DrawArrow(const DirectX::SimpleMath::Vector3& start, const DirectX::SimpleMath::Vector3& end, float tipSize, const DirectX::SimpleMath::Color& color);


protected:
    DebugRenderer* m_DebugRenderer = nullptr;
	CoreGraphicsManager* m_Graphics = nullptr;
	//Camera* m_Camera = nullptr;



public:
	void Test_DebugDrawSample();
};

