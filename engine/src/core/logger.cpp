#include "logger.hpp"
#include "asserts.h"
#include "../platform/platform.hpp"

// TODO: temporary
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

struct logger_system_state
{
    read_file_result readResult;
};

void
LogOutput(log_level level, char *message, ...);

void
ReportAssertionFailure(char *expression, char *message, char *file, i32 line)
{
#if _DEBUG
    LogOutput(
        LOG_LEVEL_FATAL,
        "Assertion Failure: %s, message: %s, in file '%s'[%d]",
        expression, message, file, line
    );
#endif
}

void
LogOutput(log_level level, char *message, ...)
{
#if _DEBUG
#define __LOG_LEVEL_STRING_LEN 10

    char logLevelStrings[LOG_LEVEL_COUNT][__LOG_LEVEL_STRING_LEN] = {
        "[FATAL]: ",
        "[ERROR]: ",
        "[WARN ]: ",
        "[INFO ]: ",
        "[DEBUG]: ",
        "[TRACE]: "
    };

    char formattedMessage[16000] = {};

    __builtin_va_list argPtr;
    va_start(argPtr, message);
    vsnprintf(formattedMessage, ArrayLen(formattedMessage), message, argPtr);
    va_end(argPtr);

    char outMessage[ArrayLen(formattedMessage) + __LOG_LEVEL_STRING_LEN] = {};
    snprintf(outMessage, ArrayLen(outMessage), "%s%s\n", logLevelStrings[level], formattedMessage);

    PlatformDebugOutput(outMessage, level);
#endif // !_DEBUG
}

internal b8
InitLogging(mem_arena *permArena, logger_state *loggerState)
{
    /* PXDEBUG("Logger initialised with bufSize of %d KiB", loggerState->bufSize / 1024); */
    /* loggerState->buf = (u8*)PXMemoryArenaAlloc(permArena, sizeof(loggerState->bufSize)); */

    return 1;
}

internal void
ShutdownLogging(logger_state *loggerState)
{
    // Write logger buffer to file
}




