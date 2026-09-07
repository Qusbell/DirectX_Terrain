#include "stdafx.h"
#include "Game.h"
#include "Engine/GameObject.h"
#include "Engine/SpriteRenderer.h"
#include "Engine/ComponentFactory.h"
#include "Engine/Transform.h"
#include "Engine/Camera.h"
#include "Engine/RandomRotator.h"
#include "Engine/MeshRenderer.h"
#include "Engine/InputComponent.h"
#include "Engine/Material.h"
#include "../Graphics/TextureManager.h"
#include "../Utils/MeshFactory.h"
#include <string>
#ifdef HOT_RELOAD_ENABLED
#include "Graphics/ShaderManager.h"
#endif

//#include "Graphics/DebugRenderer.h"
#include "Graphics/DebugManager.h"
#include "SimpleMath.h"
#include "Engine/BoxCollider.h"
#include "Engine/SphereCollider.h"
#include "Engine/CapsuleCollider.h"
#include "Engine/PickingManager.h"
#include "Engine/UIImage.h"


Game::Game(HINSTANCE hInstance)
    : m_hInstance(hInstance), m_inputManager(nullptr), m_sceneManager(nullptr), m_timeManager(nullptr), m_collisionManager(nullptr), m_pickingManager(nullptr)
{
}

Game::~Game()
{
    Shutdown();
}

bool Game::Initialize(const std::wstring& title, int width, int height)
{
    // Create Window
    m_window = std::make_unique<Window>(m_hInstance, title, width, height);
    if (!m_window->Create()) return false;

    // Create Graphics
    m_graphics = CoreGraphicsManager::GetI();
    if (!m_graphics->Initialize(m_window->GetHWND(), width, height)) return false;

    // Get Managers
    m_inputManager = InputManager::GetInstance();
    m_inputManager->Initialize(width, height);
    m_sceneManager = SceneManager::GetInstance();
    m_timeManager = TimeManager::GetInstance();
    m_timeManager->Initialize();
    m_collisionManager = CollisionManager::GetI();
    m_pickingManager = PickingManager::GetI();
    // Must be initialized after scene is loaded to create the debug GO
	

    // Register components with the factory
    auto* factory = ComponentFactory::GetInstance();
    factory->Register<Transform>(typeid(Transform).name());
    factory->Register<SpriteRenderer>(typeid( SpriteRenderer).name());
    factory->Register<Camera>(typeid(Camera).name());
    factory->Register<RandomRotator>(typeid(RandomRotator).name());
    factory->Register<MeshRenderer>(typeid(MeshRenderer).name());
    factory->Register<Light>(typeid(Light).name());
    factory->Register<InputComponent>(typeid(InputComponent).name());
    factory->Register<BoxCollider>(typeid(BoxCollider).name());
    factory->Register<SphereCollider>(typeid(SphereCollider).name());
    factory->Register<CapsuleCollider>(typeid(CapsuleCollider).name());
    factory->Register<UIImage>(typeid(UIImage).name());

    // Load initial scene
    m_sceneManager->LoadScene("Default Scene");

    // --- Create Test Scene ---
    Scene* scene = m_sceneManager->GetActiveScene();

    // Create Camera
    auto cameraGO = scene->AddGameObject("Main Camera");
    //cameraGO->GetTransform()->SetLocalPosition(0, 2.0f, -5.0f); // Move camera back and up
    cameraGO->GetTransform()->SetLocalPosition(0, 0.5f, -4.0f);
    auto cameraComp = cameraGO->AddComponent<Camera>();
    cameraGO->AddComponent<InputComponent>(); // Add our new input component
    cameraComp->SetProjectionType(Camera::ProjectionType::Perspective);
    cameraComp->SetFieldOfView(60.0f);
    cameraComp->SetAspectRatio((float)width / (float)height);
    cameraComp->SetNearClipPlane(0.1f);
    cameraComp->SetFarClipPlane(1000.0f);
    scene->SetMainCamera(cameraComp);

    // Create a directional light
    auto lightGO = scene->AddGameObject("Directional Light");
    lightGO->AddComponent<Light>();
    lightGO->GetTransform()->SetLocalRotation(30.0f, 30.0f, 0.0f); // Point it down and to the right


    // Create Meshes using the factory
    auto boxMesh = MeshFactory::CreateBox(m_graphics->GetDevice());
    auto sphereMesh = MeshFactory::CreateSphere(m_graphics->GetDevice());

    // Create Box GameObject
    auto boxGO = scene->AddGameObject("Box");
    auto boxRenderer = boxGO->AddComponent<MeshRenderer>();
    boxRenderer->SetMesh(boxMesh);
    boxGO->GetTransform()->SetLocalPosition(-1.5f, 0.0f, 0.0f);
    boxGO->AddComponent<BoxCollider>();

    // Create and assign a material to the box
    auto boxMaterial = std::make_shared<Material>(m_graphics->GetDevice());
    auto boxTexture = TextureManager::GetInstance()->LoadShared(m_graphics->GetDevice(), L"Texture/UVTexture.png");
    boxMaterial->SetShader(m_graphics->GetMeshShader());
    boxMaterial->SetAlbedoTexture(boxTexture);

    boxRenderer->SetMaterial(boxMaterial);

    // Create Sphere GameObject
    auto sphereGO = scene->AddGameObject("Sphere");
    auto sphereRenderer = sphereGO->AddComponent<MeshRenderer>();
    sphereRenderer->SetMesh(sphereMesh);
    sphereGO->GetTransform()->SetLocalPosition(1.5f, 0.0f, 0.0f);
    sphereGO->AddComponent<SphereCollider>();

    // Create Sprites
    const float spriteSpacing = 200.0f;
    for (int i = 0; i < 3; ++i)
    {
        std::string name = "Sprite " + std::to_string(i + 1);
        auto go = scene->AddGameObject(name);

        // Create material for the sprite
        auto spriteMaterial = std::make_shared<Material>(m_graphics->GetDevice());
        auto spriteTexture = TextureManager::GetInstance()->LoadShared(m_graphics->GetDevice(), L"Texture/testuv.png");
        spriteMaterial->SetShader(m_graphics->GetSpriteShader());
        spriteMaterial->SetAlbedoTexture(spriteTexture);

        go->AddComponent<SpriteRenderer>(spriteMaterial);
        go->AddComponent<RandomRotator>();
        float xPos = (i - 1) * spriteSpacing;
        go->GetTransform()->SetLocalPosition(xPos, 0, 0);
    }

    // Initialize components that need graphics resources
    m_sceneManager->Initialize(m_graphics, width, height);

    // Initialize Debug Renderer
    m_DebugManager = DebugManager::GetI();
    m_DebugManager->Initialize(m_graphics);

    m_pickingManager->Initialize(m_graphics, scene);

    return true;
}

void Game::Run()
{
    GameLoop();
}

void Game::Shutdown()
{
    if (m_DebugManager)
    {
        m_DebugManager->DestroyManager();
    }

    if (m_pickingManager)
    {
        m_pickingManager->OnDestroy();
    }
    if (m_sceneManager)
    {
        m_sceneManager->OnDestroy();
    }
    // Singleton CoreGraphicsManager is automatically destroyed
}

void Game::GameLoop()
{
    // Initial processing of objects added during Initialize
    m_sceneManager->GetActiveScene()->ProcessPendingChanges();
    m_sceneManager->GetActiveScene()->Start();

    while (m_window->ProcessMessages())
    {
        // 0. Update Time
        m_timeManager->Update();

#ifdef HOT_RELOAD_ENABLED
        ShaderManager::GetInstance()->Update();
#endif

        // 1. Update Game Logic
        m_sceneManager->Update();
        m_pickingManager->Update();

        // 2. Render
        Camera* mainCamera = m_sceneManager->GetActiveScene()->GetMainCamera();

        // Picking Pass
        m_pickingManager->RenderPickingPass(m_sceneManager->GetActiveScene(), mainCamera);

        m_graphics->BeginFrame(mainCamera);
        m_graphics->UpdateLights(mainCamera);
        m_sceneManager->Render(); // The SceneManager now handles clearing, viewports, and rendering

        // --- UI Rendering ---
        auto ui_images = m_sceneManager->GetActiveScene()->GetAllComponents<UIImage>();
        for (auto* image : ui_images)
        {
			image->OnGUIRender(mainCamera);
            //m_graphics->DrawUI(image);
        }

        // --- Debug Rendering ---
        m_DebugManager->Test_DebugDrawSample();
		m_DebugManager->Render(mainCamera);


        m_graphics->EndFrame();

        // 4. Process pending component/object changes
        m_sceneManager->ProcessPendingChanges();

        // 5. Late Update Input Manager (to transition states for next frame)
        m_inputManager->Update();
    }
}
