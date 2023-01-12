#pragma once

#include "../defines.hpp"

struct internal_state;

typedef struct platform_state
{
    u32 width;
    u32 height;

    internal_state *internalState;
} platform_state, *platform_state_ptr;


internal b8
PlatformInit(u32 width, u32 height, wchar_t *applicationName, platform_state_ptr platformState);

// TODO
b8 PlatformPumpMessages(platform_state *platformState);
f64 PlatformGetDeltaTime();

void *PlatformAllocateMemory(u64 size, b8 aligned);
void PlatformFreeMemory(void *memory, b8 aligned);

internal void PlatformDebugOutput(char *message);
