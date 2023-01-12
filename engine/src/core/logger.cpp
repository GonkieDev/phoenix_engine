#include "logger.hpp"
#include "asserts.h"
#include "../platform/platform.hpp"

// TODO: temporary
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#if _DEBUG
void
ReportAssertionFailure(char *expression, char *message, char *file, i32 line)
{
    LogOutput(
        LOG_LEVEL_FATAL,
        "Assertion Failure: %s, message: %s, in file '%s'[%d]",
        expression, message, file, line
    );
}
#endif

void
LogOutput(log_level level, char *message, ...)
{
#define __LOG_LEVEL_STRING_LEN 10

    char logLevelStrings[LOG_LEVEL_COUNT][__LOG_LEVEL_STRING_LEN] = {
        "[FATAL]: ",
        "[ERROR]: ",
        "[WARN ]: ",
        "[INFO ]: ",
        "[DEBUG]: ",
        "[TRACE]: "
    };

#ifndef _DEBUG
    if (level > LOG_LEVEL_ERROR)
    {
        return;
    }
#endif

    char formattedMessage[16000] = {};

    __builtin_va_list argPtr;
    va_start(argPtr, message);
    vsnprintf(formattedMessage, ArrayLen(formattedMessage), message, argPtr);
    va_end(argPtr);

    char outMessage[ArrayLen(formattedMessage) + __LOG_LEVEL_STRING_LEN] = {};
    snprintf(outMessage, ArrayLen(outMessage), "%s%s\n", logLevelStrings[level], formattedMessage);

    PlatformDebugOutput(outMessage);
}

internal b8
InitLogging()
{
    // TODO(gonk): create log file
    return 1;
}

internal void
ShutdownLogging()
{
}




