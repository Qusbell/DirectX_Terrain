#include "SceneManager.h"
#include "../Core/CoreGraphicsManager.h"
#include "Camera.h"
#include <algorithm> // for std::sort

SceneManager* SceneManager::s_instance = nullptr;

SceneManager* SceneManager::GetInstance()
{
    if (s_instance == nullptr)
    {
        s_instance = new SceneManager();
    }
    return s_instance;
}

SceneManager::SceneManager()
    : m_graphics(nullptr), m_windowWidth(0), m_windowHeight(0)
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::Initialize(CoreGraphicsManager* graphics, int windowWidth, int windowHeight)
{
    m_graphics = graphics;
    m_windowWidth = windowWidth;
    m_windowHeight = windowHeight;

    if (m_activeScene)
    {
        // m_activeScene->Initialize(graphics); // No longer needed
    }
}

void SceneManager::LoadScene(const std::string& name)
{
    m_activeScene = std::make_unique<Scene>(name);
    // Start is now called after initialization
}

void SceneManager::Update()
{
    if (m_activeScene)
    {
        m_activeScene->Update();
    }
}

void SceneManager::PrevRender( )
{
	if ( !m_activeScene || !m_graphics )
	{
		return;
	}

	// 1. Find all cameras in the scene
	auto cameras = m_activeScene->GetAllComponents<Camera>( );
	if ( cameras.empty( ) )
	{
		// Maybe render with a default camera or do nothing?
		// For now, we do nothing if no camera is present.
		return;
	}

	// 2. Sort cameras by depth
	std::sort(cameras.begin( ), cameras.end( ), [ ] (const Camera* a, const Camera* b) {
		return a->GetDepth( ) < b->GetDepth( );
		});

	// 3. Render the scene for each camera
	for ( Camera* camera : cameras )
	{
		// a. Set viewport
		D3D11_VIEWPORT viewport = {};
		const auto& rect = camera->GetViewportRect( );
		viewport.TopLeftX = rect.x * m_windowWidth;
		viewport.TopLeftY = rect.y * m_windowHeight;
		viewport.Width = rect.z * m_windowWidth;
		viewport.Height = rect.w * m_windowHeight;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		m_graphics->SetViewport(viewport);

		// b. Clear buffers based on camera flags
		m_graphics->Clear(camera->GetClearFlags( ), camera->GetBackgroundColor( ));

		// c. Render the scene with this camera
		m_activeScene->PreRender( );
	}
}



void SceneManager::Render()
{
    if (!m_activeScene || !m_graphics)
    {
        return;
    }

    // 1. Find all cameras in the scene
    auto cameras = m_activeScene->GetAllComponents<Camera>();
    if (cameras.empty())
    {
        // Maybe render with a default camera or do nothing?
        // For now, we do nothing if no camera is present.
        return;
    }

    // 2. Sort cameras by depth
    std::sort(cameras.begin(), cameras.end(), [](const Camera* a, const Camera* b) {
        return a->GetDepth() < b->GetDepth();
    });

    // 3. Render the scene for each camera
    for (Camera* camera : cameras)
    {
        // a. Set viewport
        D3D11_VIEWPORT viewport = {};
        const auto& rect = camera->GetViewportRect();
        viewport.TopLeftX = rect.x * m_windowWidth;
        viewport.TopLeftY = rect.y * m_windowHeight;
        viewport.Width = rect.z * m_windowWidth;
        viewport.Height = rect.w * m_windowHeight;
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        m_graphics->SetViewport(viewport);

        // b. Clear buffers based on camera flags
        m_graphics->Clear(camera->GetClearFlags(), camera->GetBackgroundColor());

        // c. Render the scene with this camera
        //m_activeScene->Render(camera); //  None Function
        m_activeScene->Render();
    }
}

void SceneManager::PostRender( )
{
	if ( !m_activeScene || !m_graphics )
	{
		return;
	}

	// 1. Find all cameras in the scene
	auto cameras = m_activeScene->GetAllComponents<Camera>( );
	if ( cameras.empty( ) )
	{
		// Maybe render with a default camera or do nothing?
		// For now, we do nothing if no camera is present.
		return;
	}

	// 2. Sort cameras by depth
	std::sort(cameras.begin( ), cameras.end( ), [ ] (const Camera* a, const Camera* b) {
		return a->GetDepth( ) < b->GetDepth( );
		});

	// 3. Render the scene for each camera
	for ( Camera* camera : cameras )
	{
		// a. Set viewport
		D3D11_VIEWPORT viewport = {};
		const auto& rect = camera->GetViewportRect( );
		viewport.TopLeftX = rect.x * m_windowWidth;
		viewport.TopLeftY = rect.y * m_windowHeight;
		viewport.Width = rect.z * m_windowWidth;
		viewport.Height = rect.w * m_windowHeight;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		m_graphics->SetViewport(viewport);

		// b. Clear buffers based on camera flags
		m_graphics->Clear(camera->GetClearFlags( ), camera->GetBackgroundColor( ));

		// c. Render the scene with this camera
		m_activeScene->PostRender( );
	}
}

void SceneManager::OnGUIRender( )
{
	if ( !m_activeScene || !m_graphics )
	{
		return;
	}

	// 1. Find all cameras in the scene
	auto cameras = m_activeScene->GetAllComponents<Camera>( );
	if ( cameras.empty( ) )
	{
		// Maybe render with a default camera or do nothing?
		// For now, we do nothing if no camera is present.
		return;
	}

	// 2. Sort cameras by depth
	std::sort(cameras.begin( ), cameras.end( ), [ ] (const Camera* a, const Camera* b) {
		return a->GetDepth( ) < b->GetDepth( );
		});

	// 3. Render the scene for each camera
	for ( Camera* camera : cameras )
	{
		// a. Set viewport
		D3D11_VIEWPORT viewport = {};
		const auto& rect = camera->GetViewportRect( );
		viewport.TopLeftX = rect.x * m_windowWidth;
		viewport.TopLeftY = rect.y * m_windowHeight;
		viewport.Width = rect.z * m_windowWidth;
		viewport.Height = rect.w * m_windowHeight;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		m_graphics->SetViewport(viewport);

		// b. Clear buffers based on camera flags
		m_graphics->Clear(camera->GetClearFlags( ), camera->GetBackgroundColor( ));

		// c. Render the scene with this camera
		m_activeScene->OnGUIRender( );
	}
}

void SceneManager::OnDestroy()
{
    if (m_activeScene)
    {
        m_activeScene->OnDestroy();
    }
}

void SceneManager::ProcessPendingChanges()
{
    if (m_activeScene)
    {
        m_activeScene->ProcessPendingChanges();
    }
}

void SceneManager::UpdateAllComponent_ASESort( )
{
	if ( m_ISASESorting )
		return;

	m_ISASESorting = true;


	m_ASESortedComponentList.clear( );



	//auto allobject = m_activeScene->GetAllComponents<Component>( );
	//if ( cameras.empty( ) )
	//{
	//	// Maybe render with a default camera or do nothing?
	//	// For now, we do nothing if no camera is present.
	//	return;
	//}

	//// 2. Sort cameras by depth
	//std::sort(cameras.begin( ), cameras.end( ), [ ] (const Camera* a, const Camera* b) {
	//	return a->GetDepth( ) < b->GetDepth( );
	//	});


	////m_ASESortedComponentList = m_activeScene->GetAllComponents<Camera>( );
	//if ( cameras.empty( ) )
	//{
	//	// Maybe render with a default camera or do nothing?
	//	// For now, we do nothing if no camera is present.
	//	return;
	//}
}

void SceneManager::UpdateAllComponent_DESCSort( )
{
	if ( m_ISDESCSorting )
		return;

	m_ISDESCSorting = true;


}
