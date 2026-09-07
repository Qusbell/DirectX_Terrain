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

std::shared_ptr<Texture> TextureManager::LoadShared(ID3D11Device* device, const std::wstring& path)
{
    // Check if texture is already loaded
    auto it = m_textureCache.find(path);
    if (it != m_textureCache.end())
    {
        return it->second;
    }

    // If not, load it
    auto newTexture = std::make_shared<Texture>();
    if (newTexture->Load(device, path))
    {
        m_textureCache[path] = newTexture;
        return newTexture;
    }

    return nullptr;
}

Texture* TextureManager::Load(ID3D11Device* device, const std::wstring& path)
{
    std::shared_ptr<Texture> sharedTex = LoadShared(device, path);
    return sharedTex ? sharedTex.get() : nullptr;
}

void TextureManager::Clear()
{
    m_textureCache.clear();
}
