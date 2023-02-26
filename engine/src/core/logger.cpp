#include "logger.hpp"
#include "asserts.h"
#include "../platform/platform.hpp"

// TODO: temporary
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

logger_state *PX_gLoggerState;

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

    char formattedMessage[3000] = {};

    __builtin_va_list argPtr;
    va_start(argPtr, message);
    vsnprintf(formattedMessage, ArrayLen(formattedMessage), message, argPtr);
    va_end(argPtr);

    char outMessage[ArrayLen(formattedMessage) + __LOG_LEVEL_STRING_LEN] = {};
    snprintf(outMessage, ArrayLen(outMessage), "%s%s\n", logLevelStrings[level], formattedMessage);

    PlatformDebugOutput(outMessage, level);

    // Simple write to PX_gLoggerState buffer
    // NOTE: this is probably temporary and can be improved
    if (PX_gLoggerState && PX_gLoggerState->buf)
    {
        u64 outMessageLen = StringLength(outMessage);
        u64 charactersLeft = PX_gLoggerState->bufCharCount - PX_gLoggerState->currChar - 1;
        if (outMessageLen > charactersLeft)
        {
            PX_gLoggerState->currChar = 0;
            charactersLeft = PX_gLoggerState->bufCharCount - 1;
        }

        // Subtract one to ignore null characters
        PX_gLoggerState->currChar += StringCopy(
            (char*)PX_gLoggerState->buf + PX_gLoggerState->currChar,
            charactersLeft,
            outMessage,
            outMessageLen);
    }

#endif // !_DEBUG
}

internal b8
InitLogging(mem_arena *permArena, logger_state *loggerState)
{
    PX_gLoggerState = loggerState;

    PXDEBUG("Logger initialised with bufCharCount of %llu", loggerState->bufCharCount);
    loggerState->buf = (u8*)PXMemoryArenaAlloc(permArena, loggerState->bufCharCount);

    return 1;
}

internal void
ShutdownLogging(mem_arena *arena, logger_state *loggerState)
{
    if (!loggerState->buf)
    {
        return;
    }

    // NOTE: this is safe as the buffer all chars in the buffer are initialised to 0
    b8 hasLooped =
        loggerState->currChar == loggerState->bufCharCount-1 ||
        loggerState->buf[loggerState->currChar + 1] != 0;

    char *tempBuffer = (char *)PXMemoryArenaAlloc(arena, loggerState->bufCharCount);
    PXMemoryClean(tempBuffer, loggerState->bufCharCount);

    u64 loggerBufLen = loggerState->bufCharCount - 1;

    // Copy logger buffer to temp buffer, looping it so that it's in order when written to file

    if (hasLooped)
    {
        u64 charsWritten = StringCopy(
            tempBuffer,
            loggerBufLen,
            (char*)(loggerState->buf + loggerState->currChar),
            loggerBufLen - loggerState->currChar);

        StringCopy(
            tempBuffer + charsWritten,
            loggerBufLen - charsWritten,
            (char*)loggerState->buf,
            loggerBufLen);
    }
    else
    {
        StringCopy(
            tempBuffer,
            loggerBufLen,
            (char*)loggerState->buf,
            loggerBufLen);
    }

    PlatformWriteEntireFile(LOG_OUTPUT_FILENAME, loggerState->bufCharCount, tempBuffer);
}




