#ifndef _LOG_H
#define _LOG_H

#include <stddef.h>
#include <stdbool.h>

typedef enum {
    Debug = 0,
    Verbose = 1,
    Info = 2,
    Error = 3,
    Fatal = 4,
} log_level_t;

#define LOG_OUTPUT_DONT_CARE (size_t)-1
#define LOG_OUTPUT_SERIAL (1 << 0)
#define LOG_OUTPUT_DEBUGCON (1 << 1)
#define LOG_OUTPUT_FRAMEBUFFER (1 << 2)
#define LOG_OUTPUT_COUNT 3

void init_log(size_t default_outputs, log_level_t trim_below_level, bool use_vga_video);
void set_log_trim_level(size_t new_trim);
void enable_log_colours(bool yes);
void logline(log_level_t level, const char* msg);
void loglinef(log_level_t level, const char* msg, ...);

#endif
