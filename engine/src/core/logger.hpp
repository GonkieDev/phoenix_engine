#pragma once

#include "../defines.hpp"

#define LOG_WARN_ENABLED 1
#define LOG_INFO_ENABLED 1
#define LOG_DEBUG_ENABLED 1
#define LOG_TRACE_ENABLED 1

#ifdef PXRELEASE
#define LOG_DEBUG_ENABLED 0
#define LOG_TRACE_ENABLED 0
#endif

struct logger_state
{
    // NOTE: buf gets allocate from memory arena and will be circular (to keep cosntant size)? 
    //  circular meaning that once the end of the buffer is reached, the start will be overwritten
    //  since we probably only care about logging if there's crashes?
    u8 *buf;
    u64 bufSize = MEGABYTES(1);
};

typedef enum log_level {
    LOG_LEVEL_FATAL,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_TRACE,

    LOG_LEVEL_COUNT
} log_level;


#define PXFATAL(message, ...) LogOutput(LOG_LEVEL_FATAL, message, ##__VA_ARGS__)
#define PXERROR(message, ...) LogOutput(LOG_LEVEL_ERROR, message, ##__VA_ARGS__)

#ifdef LOG_WARN_ENABLED
#define PXWARN(message, ...) LogOutput(LOG_LEVEL_WARN, message, ##__VA_ARGS__)
#else
#define PXWARN(message, ...)
#endif // !LOG_WARN_ENABLED

#ifdef LOG_INFO_ENABLED
#define PXINFO(message, ...) LogOutput(LOG_LEVEL_INFO, message, ##__VA_ARGS__)
#else
#define PXINFO(message, ...)
#endif // !LOG_INFO_ENABLED

#ifdef LOG_DEBUG_ENABLED
#define PXDEBUG(message, ...) LogOutput(LOG_LEVEL_DEBUG, message, ##__VA_ARGS__)
#else
#define PXDEBUG(message, ...)
#endif // !LOG_DEBUG_ENABLED

#ifdef LOG_TRACE_ENABLED
#define PXTRACE(message, ...) LogOutput(LOG_LEVEL_TRACE, message, ##__VA_ARGS__)
#else
#define PXTRACE(message, ...)
#endif // !LOG_TRACE_ENABLED


void LogOutput(log_level level, char *message, ...);
PXAPI b8 InitLogging(struct mem_arena *permArena, logger_state *loggerState);
PXAPI void ShutdownLogging(logger_state *loggerState);
