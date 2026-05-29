#include "core/logger.h"
#include "core/asserts.h"

#include "platform/platform.h"

//TODO: Temporary
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

typedef struct logger_system_state {
    b8 initialized;
} logger_system_state;

static logger_system_state* state_ptr;

b8 logging_initialize(u64* memory_requirement, void* state) {   
    *memory_requirement = sizeof(logger_system_state);
    if (state == 0) {
        return TRUE;
    }

    state_ptr = (logger_system_state*)state;
    state_ptr->initialized = TRUE;

    RQ_FATAL("TEST MESSAGE: %f", 48.12);
    RQ_ERROR("TEST MESSAGE: %f", 48.12);
    RQ_WARN("TEST MESSAGE: %f", 48.12);
    RQ_DEBUG("TEST MESSAGE: %f", 48.12);
    RQ_INFO("TEST MESSAGE: %f", 48.12);
    RQ_TRACE("TEST MESSAGE: %f", 48.12);

    // TODO: Create a log file.
    return TRUE;
}

void logging_shutdown(void* state) {
    state_ptr = 0;
    // TODO: Cleanup logging/write queued entries
}

void log_output(log_level level, const char* msg, ...) {
    const char* level_strings[6] = {"[FATAL]: ", "[ERROR]: ", "[WARN]: ", "[INFO]: ", "[DEBUG]: ", "[TRACE]: "};
    b8 is_error = level < 2;

    // Technically imposes a 32k character limit on a single log entry, but...
    // DON'T DO THAT!
    const i32 message_length = 32000;
    char out_message[message_length];
    memset(out_message, 0, sizeof(out_message));

    // Format original message;
    __builtin_va_list arg_ptr;
    va_start(arg_ptr, msg);
    vsnprintf(out_message, message_length, msg, arg_ptr);
    va_end(arg_ptr);

    char out_message_final[32000];
    sprintf(out_message_final, "%s%s\n", level_strings[level], out_message);

    // Platform-specific output.
    if (is_error) {
        platform_console_write_error(out_message_final, level);
    } else {
        platform_console_write(out_message_final, level);
    }
}

void log_line() {
    const char* line = "--------------------------------------------------\n";

    platform_console_write(line, LOG_LEVEL_INFO);
}


void report_assertion_failure(const char* expression, const char* msg, const char* file, i32 line) {
    log_output(LOG_LEVEL_FATAL, "Assertion Failure: %s, message: '%s', in file: %s, line: %d\n", expression, msg, file, line);
}