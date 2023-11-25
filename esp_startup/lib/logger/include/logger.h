#pragma once
#include "ansi_colors.h"
#include <stdio.h>
#include <time.h>

#define LOG_LEVEL_ZERO  0x00
#define LOG_LEVEL_FATAL 0x01
#define LOG_LEVEL_ERROR 0x02
#define LOG_LEVEL_WARN  0x03
#define LOG_LEVEL_INFO  0x04
#define LOG_LEVEL_DEBUG 0x05

#ifndef FATAL_TAG
  #define FATAL_TAG   "FATAL"
#endif
#ifndef ERROR_TAG
  #define ERROR_TAG   "ERROR"
#endif
#ifndef WARN_TAG
  #define WARN_TAG    "WARN"
#endif
#ifndef INFO_TAG
  #define INFO_TAG    "INFO"
#endif
#ifndef DEBUG_TAG
  #define DEBUG_TAG   "DEBUG"
#endif
                            /*color,time| tag  | file:line  | fun:line | */
#define LOG_VERBOSE_FORMAT "%s%s | %-5s | %-15s:%d | %s:%d | "
                     /*color,time| tag  | fun:line | */
#define LOG_STRICT_FORMAT  "%s%s | %-5s | %s:%d | "

#ifndef LOG_TIME_FORMAT
#define LOG_TIME_FORMAT "%Y-%m-%d %H:%M:%S"
#endif

void logger_set_log_output(FILE* file);

uint8_t logger_set_log_level(uint8_t level);

int logger_write(uint8_t level, const char* format, ...);

void logger_flush_logs(void);

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
#define LOG_FATAL(...)     logger_write(LOG_LEVEL_FATAL, LOG_VERBOSE_FORMAT, RED_BOLD_UNTERLINE, timenow(), FATAL_TAG, __FILE__, __LINE__, __func__, __LINE__); \
                           logger_write(LOG_LEVEL_FATAL, __VA_ARGS__);   \
                           logger_write(LOG_LEVEL_FATAL, "%s\n", RESET); \
                           logger_flush_logs()
#else
#define LOG_FATAL(...)
#endif

#if SET_LOG_LEVEL >= LOG_LEVEL_ERROR
#define LOG_ERROR(...)     logger_write(LOG_LEVEL_ERROR, LOG_VERBOSE_FORMAT, RED, timenow(), ERROR_TAG, __FILE__, __LINE__, __func__, __LINE__);   \
                           logger_write(LOG_LEVEL_ERROR, __VA_ARGS__);   \
                           logger_write(LOG_LEVEL_ERROR, "%s\n", RESET); \
                           logger_flush_logs()
#else
#define LOG_ERROR(...)
#endif

#if SET_LOG_LEVEL >= LOG_LEVEL_WARN
#define LOG_WARN(...)     logger_write(LOG_LEVEL_WARN, LOG_VERBOSE_FORMAT, YELLOW, timenow(), WARN_TAG, __FILE__, __LINE__, __func__, __LINE__); \
                          logger_write(LOG_LEVEL_WARN, __VA_ARGS__);   \
                          logger_write(LOG_LEVEL_WARN, "%s\n", RESET); \
                          logger_flush_logs()
#else
#define LOG_WARN(...)
#endif

#if SET_LOG_LEVEL >= LOG_LEVEL_INFO
  #ifndef USE_VERBOSE_FORMAT
    #define LOG_INFO(...)     logger_write(LOG_LEVEL_INFO, LOG_STRICT_FORMAT, GREEN, timenow(), INFO_TAG, __func__, __LINE__); \
                              logger_write(LOG_LEVEL_INFO, __VA_ARGS__);   \
                              logger_write(LOG_LEVEL_INFO, "%s\n", RESET); \
                              logger_flush_logs()
    #else
    #define LOG_INFO(...)     logger_write(LOG_LEVEL_INFO, LOG_VERBOSE_FORMAT, GREEN, timenow(), INFO_TAG, __FILE__, __LINE__, __func__, __LINE__); \
                              logger_write(LOG_LEVEL_INFO, __VA_ARGS__);   \
                              logger_write(LOG_LEVEL_INFO, "%s\n", RESET); \
                              logger_flush_logs()
  #endif
#else
#define LOG_INFO(...)
#endif

#if SET_LOG_LEVEL >= LOG_LEVEL_DEBUG
  #ifndef USE_VERBOSE_FORMAT
    #define LOG_DEBUG(...)     logger_write(LOG_LEVEL_DEBUG, LOG_STRICT_FORMAT, RESET, timenow(), DEBUG_TAG, __func__, __LINE__); \
                               logger_write(LOG_LEVEL_DEBUG, __VA_ARGS__);   \
                               logger_write(LOG_LEVEL_DEBUG, "%s\n", RESET); \
                               logger_flush_logs()
    #else
    #define LOG_DEBUG(...)     logger_write(LOG_LEVEL_DEBUG, LOG_VERBOSE_FORMAT, RESET, timenow(), DEBUG_TAG, __FILE__, __LINE__, __func__, __LINE__); \
                               logger_write(LOG_LEVEL_DEBUG, __VA_ARGS__);   \
                               logger_write(LOG_LEVEL_DEBUG, "%s\n", RESET); \
                               logger_flush_logs()
  #endif
#else
#define LOG_DEBUG(...)
#endif
