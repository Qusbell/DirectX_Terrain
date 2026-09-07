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

private:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srv;
    int m_width;
    int m_height;
};
