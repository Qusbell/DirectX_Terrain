#include "Texture.h"
#include <wincodec.h> // For WIC
#include <vector>

#pragma comment(lib, "windowscodecs.lib")

Texture::Texture() : m_width(0), m_height(0)
{
}

Texture::~Texture()
{
}

bool Texture::Load(ID3D11Device* device, const std::wstring& path)
{
    m_path = path;
    // WIC boilerplate to load image data into a raw buffer
    Microsoft::WRL::ComPtr<IWICImagingFactory> wicFactory;
    HRESULT hr = CoCreateInstance(
        CLSID_WICImagingFactory,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&wicFactory)
    );
    if (FAILED(hr)) return false;

    Microsoft::WRL::ComPtr<IWICBitmapDecoder> wicDecoder;
    hr = wicFactory->CreateDecoderFromFilename(
        path.c_str(),
        nullptr,
        GENERIC_READ,
        WICDecodeMetadataCacheOnLoad,
        &wicDecoder
    );
    if (FAILED(hr)) return false;

    Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> wicFrame;
    hr = wicDecoder->GetFrame(0, &wicFrame);
    if (FAILED(hr)) return false;

    UINT width, height;
    hr = wicFrame->GetSize(&width, &height);
    if (FAILED(hr)) return false;
    m_width = width;
    m_height = height;

    WICPixelFormatGUID pixelFormat;
    hr = wicFrame->GetPixelFormat(&pixelFormat);
    if (FAILED(hr)) return false;

    Microsoft::WRL::ComPtr<IWICFormatConverter> wicConverter;
    hr = wicFactory->CreateFormatConverter(&wicConverter);
    if (FAILED(hr)) return false;

    hr = wicConverter->Initialize(
        wicFrame.Get(),
        GUID_WICPixelFormat32bppPBGRA, // A format compatible with D3D11
        WICBitmapDitherTypeNone,
        nullptr,
        0.f,
        WICBitmapPaletteTypeMedianCut
    );
    if (FAILED(hr)) return false;

    UINT stride = width * 4;
    UINT bufferSize = stride * height;
    std::vector<BYTE> buffer(bufferSize);
    hr = wicConverter->CopyPixels(nullptr, stride, bufferSize, buffer.data());
    if (FAILED(hr)) return false;

    // Now create the D3D11 texture from the raw buffer
    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA subData = {};
    subData.pSysMem = buffer.data();
    subData.SysMemPitch = stride;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2D;
    hr = device->CreateTexture2D(&texDesc, &subData, &tex2D);
    if (FAILED(hr)) return false;

    // Create the shader resource view
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = texDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    hr = device->CreateShaderResourceView(tex2D.Get(), &srvDesc, &m_srv);
    if (FAILED(hr)) return false;

    return true;
}

void Texture::Set(ID3D11DeviceContext* context, UINT slot)
{
    context->PSSetShaderResources(slot, 1, m_srv.GetAddressOf());
}

void Texture::Initialize(ID3D11ShaderResourceView* srv)
{
	m_srv = srv;
}
