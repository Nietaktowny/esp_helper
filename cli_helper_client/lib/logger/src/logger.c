#include <stdio.h>
#include "logger.h"
#include "ansi_colors.h"
#include <stdarg.h>
#include <stdlib.h>
#include "memory_utils.h"


static FILE* logfile = NULL;
static uint8_t log_level = LOG_LEVEL_DEBUG;

void logger_set_log_output(FILE* file) {
    if(file != NULL) {
        logfile = file;
    }
}

void logger_create_semphr(void) {
    //log_mutex = xSemaphoreCreateMutex();
}

uint8_t logger_set_log_level(uint8_t level) {
    uint8_t prev = log_level;
    log_level = level;
    return prev;
}

int logger_get_lock(void) {
    return 0;//xSemaphoreTake(log_mutex, pdMS_TO_TICKS(1000));
}

void logger_return_lock(void) {
    //xSemaphoreGive(log_mutex);
}

int logger_esp_log(const char* format, ...) {
    int output = 0;
    va_list args;
    va_start(args, format);
    //logger_get_lock();
    if(logfile != NULL) {
        //log to file
        output = vfprintf(logfile, format, args);
    }
    //Log to standard error output
    output = vfprintf(stderr, format, args);
    va_end(args);
    //logger_return_lock();
    return output;    
}

int logger_write_to_file(uint8_t level, const char* format, ...) {
    if(level > log_level) {
        return 0;
    }
    
    int output = 0;
    va_list args;
    va_start(args, format);
    
    if(logfile != NULL) {
        //log to file
        output = vfprintf(logfile, format, args);
    }
    
    va_end(args);
    return output;
}

int logger_write(uint8_t level, const char* format, ...) {
    if(level > log_level) {
        return 0;
    }
    
    int output = 0;
    va_list args;
    va_start(args, format);
    
    if(logfile != NULL) {
        //log to file
        output = vfprintf(logfile, format, args);
    } else {
        // log to stderr
        output = vfprintf(stderr, format, args);
    }
    
    va_end(args);
    return output;    
}

void logger_flush_logs(void) {
    if(logfile != NULL) {
        fflush(logfile);
    }
    fflush(stderr);
}