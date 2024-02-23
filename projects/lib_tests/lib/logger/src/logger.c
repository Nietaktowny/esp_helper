#include <stdio.h>
#include "logger.h"
#include "ansi_colors.h"
#include <stdarg.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>


struct logger_contex
{
    FILE *logfiles[LOGGER_MAX_OUTPUT_FILES];
    pthread_mutex_t mutex;
    uint8_t level;
    uint8_t logfiles_num;
    uint8_t silent_mode;
};

static struct logger_contex logger = {
    .level = LOG_LEVEL_DEBUG,
    .logfiles_num = 0,
    .silent_mode = 0,
};
static uint8_t log_level = LOG_LEVEL_DEBUG;

#define LOG_INTERNAL_ERROR(error)                                                                  \
    printf(LOG_VERBOSE_FORMAT, RED, timenow(), ERROR_TAG, __FILE__, __LINE__, __func__, __LINE__); \
    printf(error);                                                                                 \
    printf("%s\n", RESET);                                                                         \
    fflush(stdout)

int logger_init(void)
{
    int err = 0;

    err = logger_create_semphr();
    if (err != 0)
    {
        LOG_INTERNAL_ERROR("cannot init logger, error when creating mutex");
        return err;
    }

    // test log
    LOG_DEBUG("logger initialized successfully");
    return err;
}

void *logger_get_logger_contex(void)
{
    return &logger;
}

int logger_add_log_file(FILE *file)
{
    if (file == NULL)
    {
        LOG_INTERNAL_ERROR("file to be used as log output cannot be NULL");
        return LOGGER_ERR_NULL_FILE;
    }

    if(logger.logfiles_num == LOGGER_MAX_OUTPUT_FILES) {
        LOG_INTERNAL_ERROR("maximum number of files to log reached, cannot add new logfile");
        return LOGGER_NO_FREE_FILES;
    }

    logger.logfiles[logger.logfiles_num] = file;
    logger.logfiles_num++;

    LOG_DEBUG("new file added as logger output!");

    return 0;
}

int logger_clear_all_log_files(void)
{
    memset(&logger.logfiles, 0, sizeof(logger.logfiles));
    logger.logfiles_num = 0;
    return 0;
}

void logger_stop_logging(void)
{
#ifdef SET_LOG_LEVEL
#undef SET_LOG_LEVEL
#define SET_LOG_LEVEL LOG_LEVEL_ZERO
#endif
}

int logger_create_semphr(void)
{
    int err = pthread_mutex_init(&logger.mutex, NULL);
    if (err != 0)
    {
        LOG_INTERNAL_ERROR("error when creating log mutex");
        printf("error %d: %s", err, strerror(err));
        return LOGGER_ERR_MUTEX_ERROR;
    }

    LOG_DEBUG("logger mutex was created");
    return 0;
}

void logger_delete_semphr(void)
{
    pthread_mutex_destroy(&logger.mutex);
}

void *logger_get_log_mutex(void)
{
    return &logger.mutex;
}

uint8_t logger_set_log_level(uint8_t level)
{
    uint8_t prev = log_level;
    log_level = level;
    return prev;
}

int logger_get_lock(void)
{
    return pthread_mutex_lock(&logger.mutex);
}

int logger_return_lock(void)
{
    return pthread_mutex_unlock(&logger.mutex);
}

int logger_esp_log(const char *format, ...)
{
    int output = 0;
    va_list args;
    va_start(args, format);

    for (uint8_t i = 0; i < logger.logfiles_num; i++)
    {
        if (logger.logfiles[i])
        {
            output = vfprintf(logger.logfiles[i], format, args);
        }
    }

    // Log to standard error output
    if (!logger.silent_mode)
    {
        output = vfprintf(stderr, format, args);
    }

    va_end(args);

    return output;
}

int logger_write(uint8_t level, const char *format, ...)
{
    if (level > log_level)
    {
        return 0;
    }

    int output = 0;
    va_list args;
    va_start(args, format);

    for (uint8_t i = 0; i < logger.logfiles_num; i++)
    {
        // log to file
        output = vfprintf(logger.logfiles[i], format, args);
    }

    if (!logger.silent_mode)
    {
        // Log to stderr
        output = vfprintf(stderr, format, args);
    }

    va_end(args);
    return output;
}

void logger_flush_logs(void)
{
    for (uint8_t i = 0; i < logger.logfiles_num; i++)
    {
        fflush(logger.logfiles[i]);
    }
    

    if(!logger.silent_mode)
    {
        fflush(stderr);
    }
}