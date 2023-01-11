#pragma once

#include <vulkan/vulkan.h>

#include "../defines.hpp"
#include "../core/memory_arenas.hpp"

struct platform_state
{
    b32 quitRequest;
    b32 windowResized;
    u32 width;
    u32 height;
    VkSurfaceKHR surface;
};

b8 PlatformPumpMessages(platform_state *platformState);

// TODO
void *PlatformAllocateMemory(u64 size, b8 aligned);
void PlatformFreeMemory(void *memory, b8 aligned);
