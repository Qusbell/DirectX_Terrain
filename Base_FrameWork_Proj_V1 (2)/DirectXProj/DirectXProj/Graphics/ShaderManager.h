#pragma once
#include <vector>
#include <filesystem>
#include "Shader.h"

class ShaderManager
{
public:
    static ShaderManager* GetInstance();

    ShaderManager(const ShaderManager&) = delete;
    ShaderManager& operator=(const ShaderManager&) = delete;

    void RegisterShader(Shader* shader);
    void Update(); // Called every frame

private:
    ShaderManager();
    ~ShaderManager();

    struct MonitoredShader
    {
        Shader* shader;
        std::filesystem::file_time_type vs_last_write;
        std::filesystem::file_time_type ps_last_write;
    };

    static ShaderManager* s_instance;
    std::vector<MonitoredShader> m_shaders;
};
