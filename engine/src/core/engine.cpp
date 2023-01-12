#include "engine.hpp"
#include "../core/logger.cpp"

b8
PhoenixInit(u32 width, u32 height, wchar_t *applicationName, platform_state_ptr platformState)
{
    if (!PlatformInit(width, height, applicationName, platformState))
    {
        PXFATAL("Platform failed to create!");
        return 0;
    }

    return 1;
}
