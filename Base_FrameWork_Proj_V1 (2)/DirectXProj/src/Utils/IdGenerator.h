#pragma once
#include <cstdint>
#include <atomic>

class IdGenerator
{
public:
    static uint64_t GenerateId()
    {
        return ++s_counter;
    }

    static void SetCounter(uint64_t value)
    {
        s_counter.store(value);
    }

private:
    static std::atomic<uint64_t> s_counter;
};
