#include "clock.hpp"

void ClockUpdate(clock *clock, platform_state *platformState)
{
    clock->elapsed = PlatformGetAbsoluteTime(platformState) - clock->start;
}

void ClockStart(clock *clock, platform_state *platformState)
{
    clock->start = PlatformGetAbsoluteTime(platformState);
}
