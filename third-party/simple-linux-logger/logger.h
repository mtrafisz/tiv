#ifndef _SIMPLE_LINUX_LOGGER
#define _SIMPLE_LINUX_LOGGER

#include <stdbool.h>
#include <stdio.h>

typedef enum {
    LOG_ALL = 0,
    LOG_TRACE,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_FATAL,
    LOG_NONE
} LogLevel;

/**
 * Initialize logger - must be run before anything gets printed
 */
void logger_init();

/**
 * Set minimal level of logs, that will be displayed
 * 
 * Ex. if You call `set_log_level(LOG_FATAL)` only log messages containing fatal errors will be written to FILE
 */
void set_log_level(LogLevel level);
/**
 * Turn on/off writing colors using ANSI escape codes
 */
void set_log_color(bool on);
/**
 * Set FILE, to which logger will write messages
 */
void set_log_output(FILE* fd);
/**
 * Turn on/off '\n' appending to the end of log message
 */
void set_log_append_nl(bool on);
/**
 * Turn on/off appending datetime to beggining of log message
 */
void set_log_append_dt(bool on);
/**
 * Turn on/off appending log level in format '[<LOG-LEVEL>]` to beggining of log message (after datetime, if active)
 */
void set_log_append_lvl(bool on);

/**
 * Write log message to specified FILE (by default stdout)
 */
void log_message(LogLevel level, const char* fmt, ...);
#define log_errno(fmt, ...) log_message(LOG_FATAL, fmt": %s", __VA_ARGS__, strerror(errno));

#endif
