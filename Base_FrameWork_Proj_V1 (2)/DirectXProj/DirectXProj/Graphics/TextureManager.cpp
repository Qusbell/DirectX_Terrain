#include "TextureManager.h"

TextureManager* TextureManager::s_instance = nullptr;

TextureManager* TextureManager::GetInstance()
{
    if (s_instance == nullptr)
    {
        s_instance = new TextureManager();
    }
    return s_instance;
}

TextureManager::TextureManager()
{
}

TextureManager::~TextureManager()
{
    Clear();
}

Texture* TextureManager::Load(ID3D11Device* device, const std::wstring& path)
{
    // Check if texture is already loaded
    auto it = m_textureCache.find(path);
    if (it != m_textureCache.end())
    {
        return it->second.get();
    }

    // If not, load it
    auto newTexture = std::make_unique<Texture>();
    if (newTexture->Load(device, path))
    {
        Texture* rawPtr = newTexture.get();
        m_textureCache[path] = std::move(newTexture);
        return rawPtr;
    }

    return nullptr;
}

void TextureManager::Clear()
{
    m_textureCache.clear();
}
