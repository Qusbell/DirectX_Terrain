#pragma once
#include "../Core/SingletonT.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <memory>
#include <map>
#include "SimpleMath.h"

class CoreGraphicsManager;
class GameObject;
class Shader;
class Scene;
class Camera;
class UIImage;
class Material;
class Texture;

class PickingManager : public SingletonT<PickingManager>
{
public:
    PickingManager();
    ~PickingManager();

    bool Initialize(CoreGraphicsManager* graphicsManager, Scene* scene);
    void OnDestroy();
    void Update();

    void RenderPickingPass(Scene* scene, Camera* camera);
    GameObject* Pick(const DirectX::SimpleMath::Vector2& screenPos);

    void RegisterObject(GameObject* obj);
    void UnregisterObject(GameObject* obj);

    void ToggleDebugThumbnail() { m_showDebugThumbnail = !m_showDebugThumbnail; }
    void SavePickingTextureToFile(const std::wstring& filePath);

private:
    friend class SingletonT<PickingManager>;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pickingTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pickingSRV;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pickingRTV;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pickingDSV;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_cpuReadableTexture;

    std::unique_ptr<Shader> m_pickingShader;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_pickingConstantBuffer;
    CoreGraphicsManager* m_graphicsManager;

    std::map<unsigned int, GameObject*> m_idToObjectMap;
    unsigned int m_nextId;

    // Debugging members
    bool m_showDebugThumbnail;
    GameObject* m_debugThumbnailGO;
    UIImage* m_debugThumbnailImage;
    std::shared_ptr<Material> m_debugMaterial;
    std::shared_ptr<Texture> m_debugTexture;
};