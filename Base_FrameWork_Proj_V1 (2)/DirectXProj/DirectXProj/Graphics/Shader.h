#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <string>

class Shader
{
public:
    Shader();
    ~Shader();

    bool Load(ID3D11Device* device, const std::wstring& vsPath, const std::wstring& psPath, bool cacheCompiled = false);
    bool Reload();
    void Set(ID3D11DeviceContext* context);

    // Will be used for hot reloading
    const std::wstring& GetVSPath() const { return m_vsPath; }
    const std::wstring& GetPSPath() const { return m_psPath; }

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

    // For hot reloading
    Microsoft::WRL::ComPtr<ID3D11Device> m_device;
    std::wstring m_vsPath;
    std::wstring m_psPath;
};
