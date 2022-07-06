#include <logging.h>
#include <qemu.h>
#include <framebuffer.h>
#include <video.h>
#include <io.h>
#include <stdio.h>
#include <stdarg.h>

const char* log_level_strings[] = {
    "  [DEBUG] ",
    "[VERBOSE] ",
    "   [INFO] ",
    "  [ERROR] ",
    "  [FATAL] ",
};

//These are standard ANSI escape sequences, and should be parsed
//by any proper terminal. The connection (serial/debugcon) is irrelevent.
//The table used as a reference is listed here:
//https://gist.github.com/Prakasaka/219fe5695beeb4d6311583e79933a009
const char* log_level_colours[] = {
    "\e[94m", 
    "\e[90m",
    "\e[97m",
    "\e[91m",
    "\e[31m"
};

//these are formatted with red as byte 0, and blue as byte 2
const uint32_t log_fb_colours[] = {
    0xFF0000,
    0x888888,
    0xFFFFFF,
    0x000088,
    0x0000FF
};

const char* log_reset_colour_str = "\e[39m";

const size_t log_level_str_len = 10; //all the above strings are 10 chars long (excluding null terminator)
const size_t log_colours_str_len = 6;
const size_t format_buffer_len = 256; //formatted log output limit, in characters.

size_t log_dest_bitmap;
size_t log_trim_level;
bool log_colours_enabled;

size_t fb_current_line;
size_t fb_max_line;
bool use_vga_output;

void init_log(size_t default_outputs, log_level_t trim_below_level, bool use_vga_video){
    if (default_outputs == LOG_OUTPUT_DONT_CARE)
        default_outputs = LOG_OUTPUT_SERIAL; //default to serial
    
    log_dest_bitmap = default_outputs;
    log_trim_level = trim_below_level;
    log_colours_enabled = true;

    use_vga_output = use_vga_video;

    if (use_vga_output) {
        fb_max_line = _SCR_H;
    }
    else {
        uint32_t chars_high = 0;
        get_framebuffer_mode(NULL, NULL, NULL, &chars_high);
        fb_max_line = chars_high;

        //it's *extremely* unlikely that the screen is this tiny. Instead it's 
        //probably that the framebuffer hasn't been initialized yet, and we've divided by zero.
        if (chars_high < 2)
            fb_max_line = -1;
    }
}

void set_log_trim_level(size_t new_trim){
    log_trim_level = new_trim;
}

void enable_log_colours(bool yes){
    log_colours_enabled = yes;
}

void logline(log_level_t level, const char* msg){
    if (level < log_trim_level)
        return; //dont log things that we dont want to see for now. (would be nice to store these somewhere in the future perhaps, just not display them?)
    
    for (size_t i = 0; i < LOG_OUTPUT_COUNT; i++){
        if ((log_dest_bitmap & (1 << i)) == 0)
            continue; //bit is cleared, we should not log there

        switch (1 << i){
            case LOG_OUTPUT_SERIAL:
                if (log_colours_enabled)
                    qemu_write_string(log_level_colours[level]);
                qemu_write_string(log_level_strings[level]);
                if (log_colours_enabled)
                    qemu_write_string(log_reset_colour_str);
                
                qemu_write_string(msg);
                qemu_write_string("\r\n");
                break;

            case LOG_OUTPUT_DEBUGCON:
                if (log_colours_enabled)
                    qemu_write_string(log_level_colours[level]);
                debugcon_write_string(log_level_strings[level]);
                if (log_colours_enabled)
                    qemu_write_string(log_reset_colour_str);

                debugcon_write_string(msg);
                debugcon_write_string("\r\n");
                break;

            case LOG_OUTPUT_FRAMEBUFFER:
                if (use_vga_output) {
                    _setVgaCursorPos(0, fb_current_line);
                    _printStr(log_level_strings[level]);
                    _printStr(msg);
                    fb_current_line++;
                }
                else {
                    if (fb_max_line == -1) {
                        //fb_max_lines hasn't been calculated yet, try now.
                        uint32_t chars_high = 0;
                        get_framebuffer_mode(NULL, NULL, NULL, &chars_high);
                        fb_max_line = chars_high;

                        if (fb_max_line < 2)
                        {
                            fb_max_line = -1;
                            break;
                        }
                    }
                    const uint32_t fb_colour = log_colours_enabled ? log_fb_colours[level] : 0xFFFFFFFF;
                    _fb_printStr(log_level_strings[level], 0, fb_current_line, fb_colour, 0);
                    _fb_printStr(msg, log_level_str_len, fb_current_line, fb_colour, 0);
                    fb_current_line++;
                }

                if (fb_current_line > fb_max_line)
                        fb_current_line = 0;
                break;
        
            default:
                continue;
        }
    }

    if (level == Fatal)
    {
        cli();
        while (1);
    }
}

void loglinef(log_level_t level, const char* msg, ...)
{
    char format_buffer[format_buffer_len];
    
    va_list format_args;
    va_start(format_args, msg);
    vsprintf(format_buffer, msg, format_args);
    va_end(format_args);

    logline(level, format_buffer);
}
