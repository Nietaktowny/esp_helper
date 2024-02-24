/**
 * @file logger.h
 * @author Wojciech Mytych
 * @brief Logger library header file.
 * @version 0.1
 * @date 2024-02-24
 * 
 * @copyright Copyright (c) 2024
 * 
 */
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

#ifndef VERBOSE_COLOR
  #define VERBOSE_COLOR WHITE
#endif
#ifndef DEBUG_COLOR
  #define DEBUG_COLOR BLUE
#endif
#ifndef INFO_COLOR
  #define INFO_COLOR GREEN
#endif
#ifndef WARN_COLOR
  #define WARN_COLOR YELLOW
#endif
#ifndef ERROR_COLOR
  #define ERROR_COLOR RED
#endif
#ifndef FATAL_COLOR
  #define FATAL_COLOR RED_BOLD_UNDERLINE
#endif



                            /*color,time| tag  | file:line  | fun:line | */
#define LOG_VERBOSE_FORMAT "%s%s | %-5s | %-15s:%d | %s:%d | "        ///< Verbose format of logs, default to all logs with levels higher than info.
                     /*color,time| tag  | fun:line | */
#define LOG_STRICT_FORMAT  "%s%s | %-5s | %s:%d | "                   ///< Strict format of logs, default to verbose, debug and info logs.

#ifndef LOG_TIME_FORMAT
#define LOG_TIME_FORMAT "%Y-%m-%d %H:%M:%S"                           ///< Time format used by all logs.
#endif

#ifndef SET_LOG_LEVEL
#define SET_LOG_LEVEL LOG_LEVEL_VERBOSE                               ///< Default log level is set to debug, change by defining SET_LOG_LEVEL level macro.
#endif

/**
 * @brief Init logger library.
 * 
 * @attention This function should be called before any logging is done!
 * 
 * @retval LOGGER_ERR_MUTEX_ERROR if cannot create logger library mutex.
 * @retval 0 on success.
 */
int logger_init(void);

/**
 * @brief Add new log file to be used as log output.
 * 
 * @param file File descriptor.
 * 
 * @retval LOGGER_ERR_NULL_FILE If file is NULL.
 * @retval LOGGER_NO_FREE_FILES If there is already max number of files registered.
 * @retval 0 on success.
 */
int logger_add_log_file(FILE* file);

/**
 * @brief Clear all files used as log output.
 * 
 * @retval 0 on success.
 */
int logger_clear_all_log_files(void);

/**
 * @brief Set logger log level.
 * 
 * Possible values:
 * -  LOG_LEVEL_VERBOSE
 * -  LOG_LEVEL_DEBUG
 * -  LOG_LEVEL_INFO
 * -  LOG_LEVEL_WARNING
 * -  LOG_LEVEL_ERROR
 * -  LOG_LEVEL_FATAL
 * 
 * @param level Log level to set.
 * @return uint8_t Previous log level.
 */
uint8_t logger_set_log_level(uint8_t level);

/**
 * @brief Enable silent mode - don't log to stderr.
 * 
 * @return uint8_t Previous value of silent mode.
 * @retval 0 if silent mode was not enabled.
 * @retval 1 if silent mode was already enabled.
 */
uint8_t logger_enable_silent_mode(void);

/**
 * @brief Disable silent mode - log all logs to stderr.
 * 
 * @return uint8_t Previous value of silent mode.
 * @retval 0 if silent mode was not enabled.
 * @retval 1 if silent mode was already enabled.
 */
uint8_t logger_disable_silent_mode(void);

/**
 * @brief Get pointer to logger context.
 * 
 * 
 * @return void* Pointer to logger context.
 */
void* logger_get_logger_contex(void);

/**
 * @brief Write to logger log files and stderr.
 * 
 * @note If silent mode is enabled logs won't be printed to stderr.
 * 
 * @param level Log level of message to print.
 * @param format VA_ARGS format.
 * @param ... VA_ARGS
 * @return int Number of characters printed.
 */
int logger_write(uint8_t level, const char* format, ...);

/**
 * @brief Flush messages in all log files.
 * 
 */
void logger_flush_logs(void);

/**
 * @brief Get logger mutex.
 * 
 * @retval 0 on success.
 */
int logger_get_lock(void);

/**
 * @brief Create logger mutex used for synchronization.
 * 
 * @retval LOGGER_ERR_MUTEX_ERROR If cannot create logger mutex.
 */
int logger_create_semphr(void);

/**
 * @brief Delete logger mutex used for synchronization.
 * 
 */
void logger_delete_semphr(void);

/**
 * @brief Get logger mutex.
 * 
 * @return void* Pointer to log mutex.
 */
void* logger_get_log_mutex(void);

/**
 * @brief Return logger mutex.
 * 
 * @retval 0 on success.
 */
int logger_return_lock(void);

int logger_esp_log(const char* format, ...);

/**
 * @brief Method used to get current time for log messages.
 * 
 * @return char* Current time in LOG_TIME_FORMAT. 
 */
static inline char *timenow() {
    static char buffer[64];
    time_t rawtime;
    struct tm *timeinfo;
    
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    
    strftime(buffer, 64, LOG_TIME_FORMAT, timeinfo);
    
    return buffer;
}

/**
 * @brief Log fatal level message macro.
 * 
 */
#if SET_LOG_LEVEL >= LOG_LEVEL_FATAL
#define LOG_FATAL(...)     logger_get_lock();        \
                           logger_write(LOG_LEVEL_FATAL, LOG_VERBOSE_FORMAT, FATAL_COLOR, timenow(), FATAL_TAG, __FILE__, __LINE__, __func__, __LINE__); \
                           logger_write(LOG_LEVEL_FATAL, __VA_ARGS__);   \
                           logger_write(LOG_LEVEL_FATAL, "%s\n", RESET); \
                           logger_flush_logs();      \
                           logger_return_lock()
#else
#define LOG_FATAL(...)
#endif

/**
 * @brief Log error level message macro.
 * 
 */
#if SET_LOG_LEVEL >= LOG_LEVEL_ERROR
#define LOG_ERROR(...)     logger_get_lock();        \
                           logger_write(LOG_LEVEL_ERROR, LOG_VERBOSE_FORMAT, ERROR_COLOR, timenow(), ERROR_TAG, __FILE__, __LINE__, __func__, __LINE__);   \
                           logger_write(LOG_LEVEL_ERROR, __VA_ARGS__);   \
                           logger_write(LOG_LEVEL_ERROR, "%s\n", RESET); \
                           logger_flush_logs();       \
                           logger_return_lock()
#else
#define LOG_ERROR(...)
#endif

/**
 * @brief Log warn level message macro.
 * 
 */
#if SET_LOG_LEVEL >= LOG_LEVEL_WARN
#define LOG_WARN(...)     logger_get_lock();        \
                          logger_write(LOG_LEVEL_WARN, LOG_VERBOSE_FORMAT, WARN_COLOR, timenow(), WARN_TAG, __FILE__, __LINE__, __func__, __LINE__); \
                          logger_write(LOG_LEVEL_WARN, __VA_ARGS__);   \
                          logger_write(LOG_LEVEL_WARN, "%s\n", RESET); \
                          logger_flush_logs();     \
                          logger_return_lock()
#else
#define LOG_WARN(...)
#endif

/**
 * @brief Log info level message macro strict format.
 * 
 */
#if SET_LOG_LEVEL >= LOG_LEVEL_INFO
  #ifndef USE_VERBOSE_FORMAT
    #define LOG_INFO(...)     logger_get_lock();      \
                              logger_write(LOG_LEVEL_INFO, LOG_STRICT_FORMAT, INFO_COLOR, timenow(), INFO_TAG, __func__, __LINE__); \
                              logger_write(LOG_LEVEL_INFO, __VA_ARGS__);   \
                              logger_write(LOG_LEVEL_INFO, "%s\n", RESET); \
                              logger_flush_logs();      \
                              logger_return_lock()
    #else
    #define LOG_INFO(...)     logger_get_lock();      \
                              logger_write(LOG_LEVEL_INFO, LOG_VERBOSE_FORMAT, INFO_COLOR, timenow(), INFO_TAG, __FILE__, __LINE__, __func__, __LINE__); \
                              logger_write(LOG_LEVEL_INFO, __VA_ARGS__);   \
                              logger_write(LOG_LEVEL_INFO, "%s\n", RESET); \
                              logger_flush_logs();
                              logger_return_lock()
  #endif
#else
#define LOG_INFO(...)
#endif

/**
 * @brief Log debug level message macro strict format.
 * 
 */
#if SET_LOG_LEVEL >= LOG_LEVEL_DEBUG
  #ifndef USE_VERBOSE_FORMAT
    #define LOG_DEBUG(...)     logger_get_lock();      \
                               logger_write(LOG_LEVEL_DEBUG, LOG_STRICT_FORMAT, DEBUG_COLOR, timenow(), DEBUG_TAG, __func__, __LINE__); \
                               logger_write(LOG_LEVEL_DEBUG, __VA_ARGS__);   \
                               logger_write(LOG_LEVEL_DEBUG, "%s\n", RESET); \
                               logger_flush_logs();    \
                               logger_return_lock();
    #else
    #define LOG_DEBUG(...)     logger_get_lock();      \
                               logger_write(LOG_LEVEL_DEBUG, LOG_VERBOSE_FORMAT, DEBUG_COLOR, timenow(), DEBUG_TAG, __FILE__, __LINE__, __func__, __LINE__); \
                               logger_write(LOG_LEVEL_DEBUG, __VA_ARGS__);   \
                               logger_write(LOG_LEVEL_DEBUG, "%s\n", RESET); \
                               logger_flush_logs();     \
                               logger_return_lock()
  #endif
#else
#define LOG_DEBUG(...)
#endif

/**
 * @brief Log debug level message macro strict format.
 * 
 */
#if SET_LOG_LEVEL >= LOG_LEVEL_VERBOSE
  #ifndef USE_VERBOSE_FORMAT
    #define LOG_VERBOSE(...)   logger_get_lock();      \
                               logger_write(LOG_LEVEL_VERBOSE, LOG_STRICT_FORMAT, VERBOSE_COLOR, timenow(), VERBOSE_TAG, __func__, __LINE__); \
                               logger_write(LOG_LEVEL_VERBOSE, __VA_ARGS__);   \
                               logger_write(LOG_LEVEL_VERBOSE, "%s\n", RESET); \
                               logger_flush_logs();    \
                               logger_return_lock();
    #else
    #define LOG_VERBOSE(...)   logger_get_lock();      \
                               logger_write(LOG_LEVEL_VERBOSE, LOG_VERBOSE_FORMAT, VERBOSE_COLOR, timenow(), VERBOSE_TAG, __FILE__, __LINE__, __func__, __LINE__); \
                               logger_write(LOG_LEVEL_VERBOSE, __VA_ARGS__);   \
                               logger_write(LOG_LEVEL_VERBOSE, "%s\n", RESET); \
                               logger_flush_logs();     \
                               logger_return_lock()
  #endif
#else
#define LOG_VERBOSE(...)
#endif
