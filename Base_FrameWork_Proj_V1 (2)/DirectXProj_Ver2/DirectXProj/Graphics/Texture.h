#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <string>

class Texture
{
public:
    Texture();
    ~Texture();

    bool Load(ID3D11Device* device, const std::wstring& path);
    void Set(ID3D11DeviceContext* context, UINT slot);

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    const std::wstring& GetPath() const { return m_path; }

	void Initialize(ID3D11ShaderResourceView* srv);

private:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srv;
    std::wstring m_path;
    int m_width;
    int m_height;
};
