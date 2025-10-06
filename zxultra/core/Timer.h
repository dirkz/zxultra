#pragma once

#include <chrono>

namespace zxultra
{

struct Timer
{
    void Start();

    void Pause();
    void Resume();

    std::chrono::nanoseconds Elapsed() const;
    double ElapsedSeconds() const;

  private:
    std::chrono::high_resolution_clock::time_point m_startedAt;
    std::chrono::high_resolution_clock::time_point m_pausedAt;
    std::chrono::nanoseconds m_pausedTime;
};

} // namespace zxultra
