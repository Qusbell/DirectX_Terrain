#include "TimeManager.h"

TimeManager* TimeManager::s_instance = nullptr;

TimeManager* TimeManager::GetInstance()
{
    if (s_instance == nullptr)
    {
        s_instance = new TimeManager();
    }
    return s_instance;
}

TimeManager::TimeManager()
    : m_deltaTime(0.0f), m_totalTime(0.0f)
{
    m_frequency = {};
    m_prevCount = {};
}

TimeManager::~TimeManager()
{
}

void TimeManager::Initialize()
{
    QueryPerformanceFrequency(&m_frequency);
    QueryPerformanceCounter(&m_prevCount);
}

void TimeManager::Update()
{
    LARGE_INTEGER currentCount;
    QueryPerformanceCounter(&currentCount);

    m_deltaTime = static_cast<float>(currentCount.QuadPart - m_prevCount.QuadPart) / static_cast<float>(m_frequency.QuadPart);
    m_totalTime += m_deltaTime;

    m_prevCount = currentCount;

    // A simple guard against huge delta times if debugging or breakpointing
    if (m_deltaTime > 0.1f)
    {
        m_deltaTime = 0.1f;
    }
}
