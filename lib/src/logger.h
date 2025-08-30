#ifndef LOGGER_H
#define LOGGER_H

#include "defines.h"

#define LOG_WARN_ENABLED 1
#define LOG_INFO_ENABLED 1
#define LOG_DEBUG_ENABLED 1
#define LOG_TRACE_ENABLED 1

// disable debug and trace for release builds
#if SRELEASE == 1
#define LOG_DEBUG_ENABLED 0
#define LOG_TRACE_ENABLED 0
#endif

typedef enum log_level {
    LOG_LEVEL_FATAL=0,
    LOG_LEVEL_ERROR=1,
    LOG_LEVEL_WARN=2,
    LOG_LEVEL_INFO=3,
    LOG_LEVEL_DEBUG=4,
    LOG_LEVEL_TRACE=5
}
log_level;

b8 initialize_logging();
void shutdown_logging();

// variadic log
SAPI void log_output(log_level level, const char* message, ...);

// log fatal
#define SFATAL(message, ...) log_output(LOG_LEVEL_FATAL, message, ##__VA_ARGS__);

#ifndef SERROR
#define SERROR(message, ...) log_output(LOG_LEVEL_ERROR, message, ##__VA_ARGS__);
#endif

#if LOG_WARN_ENABLED == 1
#define SWARN(message, ...) log_output(LOG_LEVEL_WARN, message, ##__VA_ARGS__);
#else
// does nothing when LOG_x_ENABLED != 1
#define SWARN(message, ...);
#endif

#if LOG_INFO_ENABLED == 1
#define SINFO(message, ...) log_output(LOG_LEVEL_INFO, message, ##__VA_ARGS__);
#else
// does nothing when LOG_x_ENABLED != 1
#define SINFO(message, ...);
#endif

#if LOG_DEBUG_ENABLED == 1
#define SDEBUG(message, ...) log_output(LOG_LEVEL_DEBUG, message, ##__VA_ARGS__);
#else
// does nothing when LOG_x_ENABLED != 1
#define SDEBUG(message, ...);
#endif

#if LOG_TRACE_ENABLED == 1
#define STRACE(message, ...) log_output(LOG_LEVEL_TRACE, message, ##__VA_ARGS__);
#else
// does nothing when LOG_x_ENABLED != 1
#define SDTRACE(message, ...);
#endif

#endif