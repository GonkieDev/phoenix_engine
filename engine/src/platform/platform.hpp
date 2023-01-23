#pragma once

#include <defines.hpp>
#include <core/logger.hpp>

PXAPI b8 PlatformInit(wchar_t *applicationName, struct engine_state *engineState);

PXAPI b8 PlatformPumpMessages();
PXAPI f64 PlatformGetAbsoluteTime(struct platform_state *platform_state);

PXAPI void *PlatformAllocateMemory(u64 size, b8 aligned);
PXAPI void PlatformFreeMemory(void *memory, b8 aligned);

PXAPI void PlatformSleep(f64 secondsToSleep);

PXAPI void PlatformDebugOutput(char *message, log_level level);
