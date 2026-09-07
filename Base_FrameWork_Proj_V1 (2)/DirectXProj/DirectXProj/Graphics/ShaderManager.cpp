#include "ShaderManager.h"
#include <iostream> // For debug output

ShaderManager* ShaderManager::s_instance = nullptr;

ShaderManager* ShaderManager::GetInstance()
{
    if (s_instance == nullptr)
    {
        s_instance = new ShaderManager();
    }
    return s_instance;
}

ShaderManager::ShaderManager() {}
ShaderManager::~ShaderManager() {}

void ShaderManager::RegisterShader(Shader* shader)
{
    if (!shader) return;

    try
    {
        MonitoredShader monitored;
        monitored.shader = shader;
        monitored.vs_last_write = std::filesystem::last_write_time(shader->GetVSPath());
        monitored.ps_last_write = std::filesystem::last_write_time(shader->GetPSPath());
        m_shaders.push_back(monitored);
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        // Handle case where file might not exist yet, etc.
        // For now, just ignore.
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }
}

void ShaderManager::Update()
{
    for (auto& monitored : m_shaders)
    {
        try
        {
            auto vs_current_write = std::filesystem::last_write_time(monitored.shader->GetVSPath());
            auto ps_current_write = std::filesystem::last_write_time(monitored.shader->GetPSPath());

            if (vs_current_write != monitored.vs_last_write || ps_current_write != monitored.ps_last_write)
            {
                std::cout << "Shader file changed, reloading..." << std::endl;
                if (monitored.shader->Reload())
                {
                    monitored.vs_last_write = vs_current_write;
                    monitored.ps_last_write = ps_current_write;
                    std::cout << "Reload successful." << std::endl;
                }
                else
                {
                    std::cerr << "Reload failed." << std::endl;
                }
            }
        }
        catch (const std::filesystem::filesystem_error& e)
        {
            // File might have been deleted/renamed, ignore for now
             std::cerr << "Filesystem error during update: " << e.what() << std::endl;
        }
    }
}
