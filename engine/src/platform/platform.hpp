#pragma once

#include <defines.hpp>
#include <core/logger.hpp>

// ==========================================================================  
//                                Structs
// ==========================================================================  

struct read_file_result
{
    u32 contentSize;
    void *contents;
};


// ==========================================================================  
//                          Function Declarations
// ==========================================================================  


PXAPI b8 PlatformInit(wchar_t *applicationName, struct engine_state *engineState);


//
// ======================================== Memory stuff ========================================
//

PXAPI void *PlatformAllocateMemory(u64 size, b8 aligned);

PXAPI void PlatformFreeMemory(void *memory, b8 aligned);

PXAPI void PlatformCopyMemory(void *dest, void *src, u32 sizeInBytes);


//
// ======================================== Files stuff ========================================
//

void PlatformFreeFileMemory(void *memory);
b8 PlatformWriteEntireFile(char *filename, u32 sourceBufferSize, void *sourceBuffer);

/*
 * Reads contents of file and allocates enough memory for the file.
 * File handle is closed by the function.
 * Failure: read_file_result will be filled with 0s so both contentSize and the contents ptr will be 0
 */
read_file_result PlatformReadEntireFile(char *filename);


//
// ======================================== Vulkan stuff ========================================
//

PXAPI b8 PlatformVulkanCreateSurface(struct platform_state *platformState, struct vulkan_context *context);


//
// ======================================== Utils ========================================
//

PXAPI b8 PlatformPumpMessages();

PXAPI f64 PlatformGetAbsoluteTime(struct platform_state *platform_state);

PXAPI void PlatformSleep(f64 secondsToSleep);

PXAPI void PlatformDebugOutput(char *message, log_level level);
