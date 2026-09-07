#pragma once
#include <vector>
#include <filesystem>
#include "Shader.h"
#include <d3d11.h> // For D3D11_INPUT_ELEMENT_DESC

class ShaderManager
{
public:
    static ShaderManager* GetInstance();

    ShaderManager(const ShaderManager&) = delete;
    ShaderManager& operator=(const ShaderManager&) = delete;

    void RegisterShader(Shader* shader, const std::vector<D3D11_INPUT_ELEMENT_DESC>& layoutDesc);
    void Update(); // Called every frame

private:
    ShaderManager();
    ~ShaderManager();

    struct MonitoredShader
    {
        Shader* shader;
        std::filesystem::file_time_type vs_last_write;
        std::filesystem::file_time_type ps_last_write;
        std::vector<D3D11_INPUT_ELEMENT_DESC> layoutDescription;
    };

    static ShaderManager* s_instance;
    std::vector<MonitoredShader> m_shaders;
};
