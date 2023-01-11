#pragma once

#include "../defines.hpp"

struct platform_state
{
    void *internalState;
};

struct platform_startup_info
{
    char *appName;
    u32 width;
    u32 height;
};

PXAPI b8 PlatformStartup(platform_startup_info info, platform_state *platformState);

void PlatformShutdown(platform_state *platformState);
