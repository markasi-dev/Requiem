#include "logger.h"
#include "asserts.h"

//TODO: Temporary
#include <stdio.h>
#include <string.h>
#include <stdarg.h>


b8 initialize_logging() {
    // TODO: Create a log file.
    return TRUE;
}

void shutdown_logging() {
    // TODO: Cleanup logging/write queued entries
}

void log_output(log_level level, const char* msg, ...) {
    const char* level_strings[6] = {"[FATAL]: ", "[ERROR]: ", "[WARN]: ", "[INFO]: ", "[DEBUG] ", "[TRACE]: "};
    //b8 is_error = level < 2;

    // Technically imposes a 32k character limit on a single log entry, but...
    // DON'T DO THAT!
    char out_message[32000];
    memset(out_message, 0, sizeof(out_message));

    // Format original message;
    __builtin_va_list arg_ptr;
    va_start(arg_ptr, msg);
    vsnprintf(out_message, 32000, msg, arg_ptr);
    va_end(arg_ptr);

    char out_message_final[32000];
    sprintf(out_message_final, "%s%s\n", level_strings[level], out_message);

    //TODO: Platform specific output
    printf("%s", out_message_final);
}


void report_assertion_failure(const char* expression, const char* msg, const char* file, i32 line) {
    log_output(LOG_LEVEL_FATAL, "Assertion Failue: %s, message: '%s', in file: %s, line: %d\n", expression, msg, file, line);
}