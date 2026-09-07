#include "stdafx.h"
#include "Game.h"
#include "../Engine/GameObject.h"
#include "../Engine/ComponentFactory.h"
#include "../Engine/Transform.h"
#include "../Engine/Camera.h"
#include "../Engine/InputComponent.h"
#include "../Engine/MeshRenderer.h"
#include "../Engine/Light.h"
#include "../Engine/Terrain.h"
#include "../Engine/TerrainToolController.h"
#include "../Graphics/DebugManager.h"

Game::Game(HINSTANCE hInstance)
    : m_hInstance(hInstance), m_graphics(nullptr), m_inputManager(nullptr), m_sceneManager(nullptr),
      m_timeManager(nullptr), m_collisionManager(nullptr), m_pickingManager(nullptr), m_DebugManager(nullptr) {}
Game::~Game(){Shutdown();}

bool Game::Initialize(const std::wstring& title, int width, int height)
{
    m_window=std::make_unique<Window>(m_hInstance,title,width,height); if(!m_window->Create())return false;
    m_graphics=CoreGraphicsManager::GetI(); if(!m_graphics->Initialize(m_window->GetHWND(),width,height))return false;
    m_inputManager=InputManager::GetInstance();m_inputManager->Initialize(width,height);
    m_sceneManager=SceneManager::GetInstance();m_timeManager=TimeManager::GetInstance();m_timeManager->Initialize();
    m_collisionManager=CollisionManager::GetI();m_pickingManager=PickingManager::GetI();

    auto* factory=ComponentFactory::GetInstance();
    factory->Register<Transform>(typeid(Transform).name());factory->Register<Camera>(typeid(Camera).name());
    factory->Register<InputComponent>(typeid(InputComponent).name());factory->Register<MeshRenderer>(typeid(MeshRenderer).name());
    factory->Register<Light>(typeid(Light).name());factory->Register<Terrain>(typeid(Terrain).name());

    m_sceneManager->LoadScene("Terrain Tool");Scene* scene=m_sceneManager->GetActiveScene();
    auto cameraGO=scene->AddGameObject("Editor Camera");
    cameraGO->GetTransform()->SetLocalPosition(0.0f,32.0f,-42.0f);
    cameraGO->GetTransform()->SetLocalRotation(DirectX::XMConvertToRadians(32.0f),0.0f,0.0f);
    Camera* camera=cameraGO->AddComponent<Camera>();cameraGO->AddComponent<InputComponent>();
    camera->SetAspectRatio(float(width)/height);camera->SetNearClipPlane(0.1f);camera->SetFarClipPlane(1000.0f);scene->SetMainCamera(camera);
    auto lightGO=scene->AddGameObject("Directional Light");lightGO->AddComponent<Light>();
    lightGO->GetTransform()->SetLocalRotation(DirectX::XMConvertToRadians(45.0f),DirectX::XMConvertToRadians(-30.0f),0.0f);
    auto terrainGO=scene->AddGameObject("Terrain");Terrain* terrain=terrainGO->AddComponent<Terrain>();
    if(!terrain->Initialize(100.0f,100.0f,129,129))return false;
    terrainGO->AddComponent<TerrainToolController>(terrain,camera);

    m_sceneManager->Initialize(m_graphics,width,height);
    m_DebugManager=DebugManager::GetI();m_DebugManager->Initialize(m_graphics);
    m_pickingManager->Initialize(m_graphics,scene);
    return true;
}

void Game::Run(){GameLoop();}
void Game::Shutdown(){if(m_DebugManager){m_DebugManager->DestroyManager();m_DebugManager=nullptr;}if(m_pickingManager){m_pickingManager->OnDestroy();m_pickingManager=nullptr;}if(m_sceneManager){m_sceneManager->OnDestroy();m_sceneManager=nullptr;}}

void Game::GameLoop()
{
    m_sceneManager->GetActiveScene()->ProcessPendingChanges();m_sceneManager->GetActiveScene()->Start();
    while(m_window->ProcessMessages())
    {
        m_timeManager->Update();m_sceneManager->Update();
        Camera* camera=m_sceneManager->GetActiveScene()->GetMainCamera();
        m_graphics->BeginFrame(camera);m_graphics->UpdateLights(camera);m_sceneManager->Render();
        m_DebugManager->Render(camera);m_graphics->EndFrame();
        m_sceneManager->ProcessPendingChanges();m_inputManager->Update();
    }
}
