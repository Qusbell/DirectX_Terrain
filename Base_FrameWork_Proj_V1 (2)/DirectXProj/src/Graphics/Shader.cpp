#include "Shader.h"
#include <d3dcompiler.h>
#include <fstream>

#pragma comment(lib, "d3dcompiler.lib")

namespace {
    // Helper to compile a shader from file and handle errors
    HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
    {
        DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
        dwShaderFlags |= D3DCOMPILE_DEBUG;
        dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
        ID3DBlob* pErrorBlob = nullptr;
        HRESULT hr = D3DCompileFromFile(szFileName, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, szEntryPoint, szShaderModel,
            dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
        if (FAILED(hr))
        {
            if (pErrorBlob)
            {
                OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
                MessageBoxA(nullptr, reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()), "Shader Compilation Error", MB_OK);
                pErrorBlob->Release();
            }
            return hr;
        }
        if (pErrorBlob) pErrorBlob->Release();
        return S_OK;
    }
}

Shader::Shader()
{
}

Shader::~Shader()
{
}

bool Shader::Load(ID3D11Device* device, const std::wstring& vsPath, const std::wstring& psPath, bool cacheCompiled)
{
    m_device = device;
    m_vsPath = vsPath;
    m_psPath = psPath;

    // Compile the vertex shader
    Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;
    HRESULT hr = CompileShaderFromFile(m_vsPath.c_str(), "main", "vs_5_0", &vsBlob);
    if (FAILED(hr)) return false;

    // Compile the pixel shader
    Microsoft::WRL::ComPtr<ID3DBlob> psBlob;
    hr = CompileShaderFromFile(m_psPath.c_str(), "main", "ps_5_0", &psBlob);
    if (FAILED(hr)) return false;

    // Create the Direct3D shader objects
    hr = m_device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_vertexShader);
    if (FAILED(hr)) return false;

    hr = m_device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_pixelShader);
    if (FAILED(hr)) return false;

    // Create the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = ARRAYSIZE(layout);
    hr = m_device->CreateInputLayout(layout, numElements, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_inputLayout);
    if (FAILED(hr)) return false;

    // Optionally save the compiled shaders to .cso files
    if (cacheCompiled)
    {
        auto save_blob = [](const std::wstring& path, ID3DBlob* blob) {
            std::wstring cso_path = path.substr(0, path.find_last_of(L'.')) + L".cso";
            std::ofstream out(cso_path, std::ios::binary);
            out.write(static_cast<char*>(blob->GetBufferPointer()), blob->GetBufferSize());
            out.close();
        };
        save_blob(m_vsPath, vsBlob.Get());
        save_blob(m_psPath, psBlob.Get());
    }

    return true;
}

bool Shader::Reload()
{
    // Re-compile and re-create all resources.
    return Load(m_device.Get(), m_vsPath, m_psPath, false);
}

void Shader::Set(ID3D11DeviceContext* context)
{
    context->IASetInputLayout(m_inputLayout.Get());
    context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
    context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
}
