#pragma once
#include "ansi_colors.h"
#include <stdio.h>
#include <time.h>
#include <inttypes.h>

#define LOGGER_ERR_BASE         0x0950                                ///< Logger error base.
#define LOGGER_ERR_MUTEX_ERROR  0x0001 + LOGGER_ERR_BASE              ///< When some error with logger mutex occurred.
#define LOGGER_MUTEX_TIMEOUT    0x0002 + LOGGER_ERR_BASE              ///< When function getting semaphore timeout expired and cannot obtain log_mutex.
#define LOGGER_ERR_NULL_FILE    0x0003 + LOGGER_ERR_BASE              ///< When null pointer is passed as log file output.
#define LOGGER_NO_FREE_FILES    0x0004 + LOGGER_ERR_BASE              ///< When there are no new slots for log files, max number of files has been reached.


#define LOGGER_MAX_OUTPUT_FILES 4                                     ///< Max number of files used as output.

#define LOG_LEVEL_ZERO          0x00                                  ///< Log level zero, no logging.
#define LOG_LEVEL_FATAL         0x01                                  ///< Log level fatal, log only fatal errors.
#define LOG_LEVEL_ERROR         0x02                                  ///< Log level error, log only errors, and fatal errors.
#define LOG_LEVEL_WARN          0x03                                  ///< Log level warn, log warnings, errors and fatal errors.
#define LOG_LEVEL_INFO          0x04                                  ///< Log level info, log info logs, warnings, errors, and fatal errors.
#define LOG_LEVEL_DEBUG         0x05                                  ///< Log level debug, log debug information as well as all log levels apart from verbose logs.
#define LOG_LEVEL_VERBOSE       0x06                                  ///< Log level verbose, log all logs.

#ifndef FATAL_TAG
  #define FATAL_TAG   "FATAL"                                         ///< Fatal logs tag. 
#endif
#ifndef ERROR_TAG
  #define ERROR_TAG   "ERROR"                                         ///< Error logs tag.
#endif
#ifndef WARN_TAG
  #define WARN_TAG    "WARN"                                          ///< Warning logs tag.
#endif
#ifndef INFO_TAG
  #define INFO_TAG    "INFO"                                          ///< Informational logs tag.
#endif
#ifndef DEBUG_TAG
  #define DEBUG_TAG   "DEBUG"                                         ///< Debug logs tag.
#endif
#ifndef VERBOSE_TAG
  #define VERBOSE_TAG "VERBOSE"                                       ///< Verbose logs tag.
#endif

                            /*color,time| tag  | file:line  | fun:line | */
#define LOG_VERBOSE_FORMAT "%s%s | %-5s | %-15s:%d | %s:%d | "        ///< Verbose format of logs, default to all logs with levels higher than info.
                     /*color,time| tag  | fun:line | */
#define LOG_STRICT_FORMAT  "%s%s | %-5s | %s:%d | "                   ///< Strict format of logs, default to verbose, debug and info logs.

#ifndef LOG_TIME_FORMAT
#define LOG_TIME_FORMAT "%Y-%m-%d %H:%M:%S"                           ///< Time format used by all logs.
#endif

#ifndef SET_LOG_LEVEL
#define SET_LOG_LEVEL LOG_LEVEL_DEBUG                                 ///< Default log level is set to debug, change by defining SET_LOG_LEVEL level macro.
#endif

int logger_init(void);

int logger_add_log_file(FILE* file);

int logger_clear_all_log_files(void);

uint8_t logger_set_log_level(uint8_t level);

void* logger_get_logger_contex(void);

int logger_write(uint8_t level, const char* format, ...);

void logger_flush_logs(void);

int logger_get_lock(void);

int logger_create_semphr(void);

void logger_delete_semphr(void);

void* logger_get_log_mutex(void);

int logger_return_lock(void);

int set_logging_to_socket(char* address, uint16_t port);

int logger_esp_log(const char* format, ...);

int logger_redirect_esp_logs(void);

static inline char *timenow() {
    static char buffer[64];
    time_t rawtime;
    struct tm *timeinfo;
    
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    
    strftime(buffer, 64, LOG_TIME_FORMAT, timeinfo);
    
    return buffer;
}

#if SET_LOG_LEVEL >= LOG_LEVEL_FATAL
#define LOG_FATAL(...)     logger_get_lock();        \
                           logger_write(LOG_LEVEL_FATAL, LOG_VERBOSE_FORMAT, RED_BOLD_UNTERLINE, timenow(), FATAL_TAG, __FILE__, __LINE__, __func__, __LINE__); \
                           logger_write(LOG_LEVEL_FATAL, __VA_ARGS__);   \
                           logger_write(LOG_LEVEL_FATAL, "%s\n", RESET); \
                           logger_flush_logs();      \
                           logger_return_lock()
#else
#define LOG_FATAL(...)
#endif

#if SET_LOG_LEVEL >= LOG_LEVEL_ERROR
#define LOG_ERROR(...)     logger_get_lock();        \
                           logger_write(LOG_LEVEL_ERROR, LOG_VERBOSE_FORMAT, RED, timenow(), ERROR_TAG, __FILE__, __LINE__, __func__, __LINE__);   \
                           logger_write(LOG_LEVEL_ERROR, __VA_ARGS__);   \
                           logger_write(LOG_LEVEL_ERROR, "%s\n", RESET); \
                           logger_flush_logs();       \
                           logger_return_lock()
#else
#define LOG_ERROR(...)
#endif

#if SET_LOG_LEVEL >= LOG_LEVEL_WARN
#define LOG_WARN(...)     logger_get_lock();        \
                          logger_write(LOG_LEVEL_WARN, LOG_VERBOSE_FORMAT, YELLOW, timenow(), WARN_TAG, __FILE__, __LINE__, __func__, __LINE__); \
                          logger_write(LOG_LEVEL_WARN, __VA_ARGS__);   \
                          logger_write(LOG_LEVEL_WARN, "%s\n", RESET); \
                          logger_flush_logs();     \
                          logger_return_lock()
#else
#define LOG_WARN(...)
#endif

#if SET_LOG_LEVEL >= LOG_LEVEL_INFO
  #ifndef USE_VERBOSE_FORMAT
    #define LOG_INFO(...)     logger_get_lock();      \
                              logger_write(LOG_LEVEL_INFO, LOG_STRICT_FORMAT, GREEN, timenow(), INFO_TAG, __func__, __LINE__); \
                              logger_write(LOG_LEVEL_INFO, __VA_ARGS__);   \
                              logger_write(LOG_LEVEL_INFO, "%s\n", RESET); \
                              logger_flush_logs();      \
                              logger_return_lock()
    #else
    #define LOG_INFO(...)     logger_get_lock();      \
                              logger_write(LOG_LEVEL_INFO, LOG_VERBOSE_FORMAT, GREEN, timenow(), INFO_TAG, __FILE__, __LINE__, __func__, __LINE__); \
                              logger_write(LOG_LEVEL_INFO, __VA_ARGS__);   \
                              logger_write(LOG_LEVEL_INFO, "%s\n", RESET); \
                              logger_flush_logs();
                              logger_return_lock()
  #endif
#else
#define LOG_INFO(...)
#endif

#if SET_LOG_LEVEL >= LOG_LEVEL_DEBUG
  #ifndef USE_VERBOSE_FORMAT
    #define LOG_DEBUG(...)     logger_get_lock();      \
                               logger_write(LOG_LEVEL_DEBUG, LOG_STRICT_FORMAT, RESET, timenow(), DEBUG_TAG, __func__, __LINE__); \
                               logger_write(LOG_LEVEL_DEBUG, __VA_ARGS__);   \
                               logger_write(LOG_LEVEL_DEBUG, "%s\n", RESET); \
                               logger_flush_logs();    \
                               logger_return_lock();
    #else
    #define LOG_DEBUG(...)     logger_get_lock();      \
                               logger_write(LOG_LEVEL_DEBUG, LOG_VERBOSE_FORMAT, RESET, timenow(), DEBUG_TAG, __FILE__, __LINE__, __func__, __LINE__); \
                               logger_write(LOG_LEVEL_DEBUG, __VA_ARGS__);   \
                               logger_write(LOG_LEVEL_DEBUG, "%s\n", RESET); \
                               logger_flush_logs();     \
                               logger_return_lock()
  #endif
#else
#define LOG_DEBUG(...)
#endif
