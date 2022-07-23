#include "Lunar-lib.h"
#include <iostream>

void __cdecl lunar::WaitMS(uint32_t milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds (milliseconds) );
}
void __cdecl lunar::WaitMCS(Uint32 microseconds)
{
    std::this_thread::sleep_for((std::chrono::microseconds)microseconds);
}
lunar::Lresult<std::chrono::steady_clock::time_point> __cdecl lunar::StartStopwatch(timer *timer)
{
    Lresult<std::chrono::steady_clock::time_point> result;
    timer->start_time = std::chrono::high_resolution_clock::now();
    return result;
}
lunar::Lresult<lunar::times> __cdecl lunar::CheckStopwatch(timer *timer)
{
    Lresult<lunar::times> result;

    result.result.milliseconds =  std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - timer->start_time).count()/1.f;
    result.result.seconds =  std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - timer->start_time).count()/1000.f;
    result.result.minutes =  std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - timer->start_time).count()/60000.f;
    result.result.hours = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - timer->start_time).count()/3600000.f;
    return result;
}
lunar::Lresult<void*> __cdecl lunar::ResetStopwatch(timer *timer)
{
    Lresult<void*> result;
    timer->start_time = std::chrono::high_resolution_clock::now();
    return result;
}

bool __cdecl lunar::CompareFlags(uint32_t lflag, uint32_t lcompare)
{
    return (lflag & lcompare) == lcompare;
}