#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include "spiffs_c_errors.h"


struct spiffs_c_handle_obj {
    char* base_name;
    char* partition_label;
    uint8_t max_files;
    bool format_on_err;
};
typedef struct spiffs_c_handle_obj spiffs_c_handle_t;

#define SPIFFS_C_DEFAULT_CONFIG()       \
        {                               \
            .base_name = "/spiffs",     \
            .partition_label = NULL,    \
            .max_files = 5,             \
            .format_on_err = true,      \
        };

int spiffs_c_init(const spiffs_c_handle_t* handle);

int spiffs_c_check(const char* partition_label);

int spiffs_c_format(const char* partition);

int spiffs_c_get_info(const char* partition_label, size_t* out_total, size_t* out_used);

int spiffs_c_open_file(spiffs_c_handle_t* handle, char* filename, const char* mode, FILE** out_file);

void spiffs_c_close_file(FILE* file, const char* filename);

int spiffs_c_read_file(spiffs_c_handle_t* handle, char* filename, char* out_data, size_t buflen);