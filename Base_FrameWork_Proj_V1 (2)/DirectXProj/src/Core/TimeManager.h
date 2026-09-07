#pragma once
#include <Windows.h>

class TimeManager
{
public:
    static TimeManager* GetInstance();

    TimeManager(const TimeManager&) = delete;
    TimeManager& operator=(const TimeManager&) = delete;

    void Initialize();
    void Update();

    float GetDeltaTime() const { return m_deltaTime; }
    float GetTotalTime() const { return m_totalTime; }

private:
    TimeManager();
    ~TimeManager();

    static TimeManager* s_instance;

    LARGE_INTEGER m_frequency;
    LARGE_INTEGER m_prevCount;
    float m_deltaTime;
    float m_totalTime;
};
