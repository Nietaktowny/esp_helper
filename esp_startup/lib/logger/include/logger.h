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

#ifndef SET_LOG_LEVEL
#define SET_LOG_LEVEL   LOG_LEVEL_DEBUG
#endif

#ifndef _FILE
#define _FILE stderr
#endif

                            /*color,time| tag  | file:line  | fun:line | */
#define LOG_VERBOSE_FORMAT "%s%s | %-5s | %-15s:%d | %s:%d | "
                     /*color,time| tag  | fun:line | */
#define LOG_STRICT_FORMAT  "%s%s | %-5s | %s:%d | "

#ifndef LOG_TIME_FORMAT
#define LOG_TIME_FORMAT "%Y-%m-%d %H:%M:%S"
#endif

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
#define LOG_FATAL(...)     fprintf(_FILE, LOG_VERBOSE_FORMAT, RED_BOLD_UNTERLINE, timenow(), FATAL_TAG, __FILE__, __LINE__, __func__, __LINE__); \
                           fprintf(_FILE, __VA_ARGS__);   \
                           fprintf(_FILE, "%s\n", RESET); \
                           fflush(_FILE);
#else
#define LOG_FATAL(...)
#endif

#if SET_LOG_LEVEL >= LOG_LEVEL_ERROR
#define LOG_ERROR(...)     fprintf(_FILE, LOG_VERBOSE_FORMAT, RED, timenow(), ERROR_TAG, __FILE__, __LINE__, __func__, __LINE__);   \
                           fprintf(_FILE, __VA_ARGS__);   \
                           fprintf(_FILE, "%s\n", RESET); \
                           fflush(_FILE)
#else
#define LOG_ERROR(...)
#endif

#if SET_LOG_LEVEL >= LOG_LEVEL_WARN
#define LOG_WARN(...)     fprintf(_FILE, LOG_VERBOSE_FORMAT, YELLOW, timenow(), WARN_TAG, __FILE__, __LINE__, __func__, __LINE__); \
                          fprintf(_FILE, __VA_ARGS__);   \
                          fprintf(_FILE, "%s\n", RESET); \
                          fflush(_FILE)
#else
#define LOG_WARN(...)
#endif

#if SET_LOG_LEVEL >= LOG_LEVEL_INFO
  #ifndef USE_VERBOSE_FORMAT
    #define LOG_INFO(...)     fprintf(_FILE, LOG_STRICT_FORMAT, GREEN, timenow(), INFO_TAG, __func__, __LINE__); \
                              fprintf(_FILE, __VA_ARGS__);   \
                              fprintf(_FILE, "%s\n", RESET); \
                              fflush(_FILE)
    #else
    #define LOG_INFO(...)     fprintf(_FILE, LOG_VERBOSE_FORMAT, GREEN, timenow(), INFO_TAG, __FILE__, __LINE__, __func__, __LINE__); \
                              fprintf(_FILE, __VA_ARGS__);   \
                              fprintf(_FILE, "%s\n", RESET); \
                              fflush(_FILE)
  #endif
#else
#define LOG_INFO(...)
#endif

#if SET_LOG_LEVEL >= LOG_LEVEL_DEBUG
  #ifndef USE_VERBOSE_FORMAT
    #define LOG_DEBUG(...)     fprintf(_FILE, LOG_STRICT_FORMAT, RESET, timenow(), DEBUG_TAG, __func__, __LINE__); \
                               fprintf(_FILE, __VA_ARGS__);   \
                               fprintf(_FILE, "%s\n", RESET); \
                               fflush(_FILE)
    #else
    #define LOG_DEBUG(...)     fprintf(_FILE, LOG_VERBOSE_FORMAT, RESET, timenow(), DEBUG_TAG, __FILE__, __LINE__, __func__, __LINE__); \
                               fprintf(_FILE, __VA_ARGS__);   \
                               fprintf(_FILE, "%s\n", RESET); \
                               fflush(_FILE)
  #endif
#else
#define LOG_DEBUG(...)
#endif
