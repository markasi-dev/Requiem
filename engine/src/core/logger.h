#pragma once

#include "defines.h"

#define LOG_WARN_ENABLED 1
#define LOG_INFO_ENABLED 1
#define LOG_DEBUG_ENABLED 1
#define LOG_TRACE_ENABLED 1

// Disable debug and trace logging for release builds.
#if RQ_REALEASE == 1
    #define LOG_DEBUG_ENABLED 0
    #define LOG_TRACE_ENABLED 0
#endif

typedef enum log_level {
    LOG_LEVEL_FATAL = 0,
    LOG_LEVEL_ERROR = 1,
    LOG_LEVEL_WARN = 2,
    LOG_LEVEL_INFO = 3,
    LOG_LEVEL_DEBUG = 4,
    LOG_LEVEL_TRACE = 5
} log_level;

b8 initialize_logging();
void shutdown_logging();

RAPI void log_output(log_level level, const char* msg, ...);

// Logs a fatal-level message
#define RQ_FATAL(msg, ...) log_output(LOG_LEVEL_FATAL, msg, ##__VA_ARGS__);


#ifndef RQ_ERROR
    // Logs an error-level message
    #define RQ_ERROR(msg, ...) log_output(LOG_LEVEL_ERROR, msg, ##__VA_ARGS__);
#endif


#if LOG_WARN_ENABLED == 1
    // Logs a warning-level message
    #define RQ_WARN(msg, ...) log_output(LOG_LEVEL_WARN, msg, ##__VA_ARGS__);
#else
    // Do nothing.
    #define RQ_WARN(msg, ...)
#endif


#if LOG_INFO_ENABLED == 1
    // Logs a warning-level message
    #define RQ_INFO(msg, ...) log_output(LOG_LEVEL_INFO, msg, ##__VA_ARGS__);
    // Do nothing.
    #else
    #define RQ_INFO(msg, ...)
#endif


#if LOG_DEBUG_ENABLED == 1
    // Logs a warning-level message
    #define RQ_DEBUG(msg, ...) log_output(LOG_LEVEL_DEBUG, msg, ##__VA_ARGS__);
#else
    // Do nothing.
    #define RQ_DEBUG(msg, ...)
#endif


#if LOG_TRACE_ENABLED == 1
    // Logs a warning-level message
    #define RQ_TRACE(msg, ...) log_output(LOG_LEVEL_TRACE, msg, ##__VA_ARGS__);
#else
    // Do nothing.
    #define RQ_TRACE(msg, ...)
#endif