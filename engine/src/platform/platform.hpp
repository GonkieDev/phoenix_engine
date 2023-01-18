#pragma once

#include "../defines.hpp"

PXAPI b8 PlatformInit(wchar_t *applicationName, engine_state *engineState);

PXAPI b8 PlatformPumpMessages();
PXAPI f64 PlatformGetAbsoluteTime(platform_state *platform_state);

PXAPI void *PlatformAllocateMemory(u64 size, b8 aligned);
PXAPI void PlatformFreeMemory(void *memory, b8 aligned);

PXAPI void PlatformSleep(f64 secondsToSleep);

PXAPI void PlatformDebugOutput(char *message, log_level level);
