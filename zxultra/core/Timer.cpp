#include "Timer.h"

namespace zxultra
{

void Timer::Start()
{
    m_startedAt = std::chrono::high_resolution_clock::now();
}

void Timer::Pause()
{
    m_pausedAt = std::chrono::high_resolution_clock::now();
}

void Timer::Resume()
{
    auto now = std::chrono::high_resolution_clock::now();
    auto pausedTime = m_pausedAt - now;
    m_pausedTime += pausedTime;
}

std::chrono::nanoseconds Timer::Elapsed() const
{
    auto now = std::chrono::high_resolution_clock::now();
    return now - m_startedAt - m_pausedTime;
}

double Timer::ElapsedSeconds() const
{
    auto elapsed = Elapsed().count();
    auto dElapsed = static_cast<double>(elapsed);
    return dElapsed / 1000 / 1000;
}

} // namespace zxultra