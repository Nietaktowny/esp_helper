#include "spiffs_controller.h"
#include "err_controller.h"
#include "logger.h"
#include "memory_utils.h"

#include "esp_spiffs.h"
#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <stdbool.h>


int spiffs_c_init(const spiffs_c_handle_t* handle) {
    volatile err_c_t err = 0;
    LOG_DEBUG("initializing SPIFFS...");

    esp_vfs_spiffs_conf_t config = {
        .base_path = handle->base_name,
        .partition_label = handle->partition_label,
        .max_files = handle->max_files,
        .format_if_mount_failed = handle->format_on_err,
    };

    Try {
        ERR_C_CHECK_AND_THROW_ERR(esp_vfs_spiffs_register(&config));
        LOG_DEBUG("initialized SPIFFS successfully");
    } Catch(err) {
        switch (err)
        {
        case ESP_FAIL:
            LOG_ERROR("Failed to mount or format filesystem");
            break;
        case ESP_ERR_NOT_FOUND:
            LOG_ERROR("Failed to find SPIFFS partition");
            break;    
        default:
            LOG_ERROR("failed to initialize SPIFFS: %s", error_to_name(err));
            break;
        }
    }
    return err;
}


int spiffs_c_check(const char* partition_label) {
    volatile err_c_t err = 0;

    err = esp_spiffs_check(partition_label);

    if(err != ERR_C_OK) {
        LOG_ERROR("SPIFFS check failed: %s", error_to_name(err));
    } else {
        LOG_INFO("SPIFFS check successful");
    }
    return err;
}

int spiffs_c_format(const char* partition) {
    int err = 0;
    LOG_DEBUG("formatting SPIFFS...");
    err = esp_spiffs_format(partition);
    if(err != ERR_C_OK) {
        LOG_ERROR("error when formatting SPIFFS: %s", error_to_name(err));
    } else {
        LOG_DEBUG("SPIFFS formatted successful");
    }
    return err;
}

int spiffs_c_get_info(const char* partition_label, size_t* out_total, size_t* out_used) {
    int err = 0;
    //Guard clause
    ERR_C_CHECK_NULL_PTR(out_total, LOG_ERROR("pointer where to store total memory cannot be NULL"));
    ERR_C_CHECK_NULL_PTR(out_used, LOG_ERROR("pointer where to store used memory cannot be NULL"));

    err = esp_spiffs_info(partition_label, out_total, out_used);
    
    if(err != ERR_C_OK) {
        LOG_ERROR("failed to get SPIFFS partition information, error: %s", error_to_name(err));
        spiffs_c_format(partition_label);
    } else {
        LOG_INFO("partition size:\n\ttotal: %d,\n\tused: %d", (*out_total), (*out_used));
    }

    if((*out_used) > (*out_total)) {
        LOG_WARN("Number of used bytes cannot be larger than total. Performing SPIFFS check...");
        err = spiffs_c_check(partition_label);
    }
    return err;
}

int spiffs_c_open_file(spiffs_c_handle_t* handle, char* filename, const char* mode, FILE** out_file) {
    int err = 0;
    char path[64];
    memutil_zero_memory(&path, sizeof(path));

    ERR_C_CHECK_NULL_PTR(filename, LOG_ERROR("filename cannot be NULL"));
    ERR_C_CHECK_NULL_PTR(mode, LOG_ERROR("file opening mode cannot be NULL"));
    ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("SPIFFS controller handle cannot be NULL"));
    
    //prepare file path
    sprintf(path, "%s/%s", handle->base_name, filename);
    LOG_DEBUG("opening file on path: %s", path);
    (*out_file) = fopen(path, mode);

    ERR_C_CHECK_NULL_PTR((*out_file), LOG_ERROR("failed to open file in mode %s", mode));

    return err;
}

void spiffs_c_close_file(FILE* file) {
    fclose(file);
}