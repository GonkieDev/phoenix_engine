#pragma once

#include <defines.hpp>
#include <core/logger.hpp>

struct read_file_result
{
    u32 contentSize;
    void *contents;
};

PXAPI b8 PlatformInit(wchar_t *applicationName, struct engine_state *engineState);

PXAPI b8 PlatformPumpMessages();
PXAPI f64 PlatformGetAbsoluteTime(struct platform_state *platform_state);

PXAPI void *PlatformAllocateMemory(u64 size, b8 aligned);
PXAPI void PlatformFreeMemory(void *memory, b8 aligned);

PXAPI void PlatformSleep(f64 secondsToSleep);

PXAPI void PlatformDebugOutput(char *message, log_level level);

PXAPI b8 PlatformVulkanCreateSurface(struct platform_state *platformState, struct vulkan_context *context);

void PlatformFreeFileMemory(void *memory);
b8 PlatformWriteEntireFile(char *filename, u32 memorySize, void *memory);

/*
 * Reads contents of file and allocates enough memory for the file.
 * File handle is closed by the function.
 * Failure: read_file_result will be filled with 0s so both contentSize and the contents ptr will be 0
 */
read_file_result PlatformReadEntireFile(char *filename);
