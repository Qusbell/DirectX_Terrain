#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include "Texture.h"

class TextureManager
{
public:
    static TextureManager* GetInstance();

    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

    Texture* Load(ID3D11Device* device, const std::wstring& path);
    std::shared_ptr<Texture> LoadShared(ID3D11Device* device, const std::wstring& path);
    void Clear();

private:
    TextureManager();
    ~TextureManager();

    static TextureManager* s_instance;

    std::unordered_map<std::wstring, std::shared_ptr<Texture>> m_textureCache;
};
