#include <stdio.h>
#include "logger.h"
#include "ansi_colors.h"
#include <stdarg.h>
#include <stdlib.h>
#include "memory_utils.h"
#include "esp_log.h"



static FILE* logfile = NULL;
static uint8_t log_level = LOG_LEVEL_DEBUG;

void logger_set_log_output(FILE* file) {
    if(file != NULL) {
        logfile = file;
    }
}

uint8_t logger_set_log_level(uint8_t level) {
    uint8_t prev = log_level;
    log_level = level;
    return prev;
}

int logger_redirect_esp_logs(void) {
    esp_log_set_vprintf(logger_esp_log);
    return 0;
}

int logger_esp_log(const char* format, ...) {
    int output = 0;
    va_list args;
    va_start(args, format);

    if(logfile != NULL) {
        //log to file
        output = vfprintf(logfile, format, args);
    } else {
        //Log to stderr
        output = vfprintf(stderr, format, args);
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
        //Log to stderr
        output = vfprintf(stderr, format, args);
    }
    va_end(args);

    return output;
}

void logger_flush_logs(void) {
    if(logfile != NULL) {
        fflush(logfile);
    } else {
        fflush(stderr);
    }
}