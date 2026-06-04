#pragma once
#include <atomic>

struct RuntimeState
{
    std::atomic<bool> running{ true };
    std::atomic<bool> unloading{ false };
    std::atomic<bool> initialized{ false };
};

inline RuntimeState g_state;