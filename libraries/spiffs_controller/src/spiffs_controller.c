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

//PROTOTYPES
static int spiffs_c_prepare_filepath(char* prefix_path, char* filename, char* out_path, size_t buflen);

static int spiffs_c_read_file_stats(const spiffs_c_handle_t* handle, char* filename, struct stat* out_stats);

//END OF PROTOTYPES

static int spiffs_c_read_file_stats(const spiffs_c_handle_t* handle, char* filename, struct stat* out_stats) {
    err_c_t err = 0;
    char path[64];
    memutil_zero_memory(&path, sizeof(path));
    memutil_zero_memory(out_stats, sizeof(struct stat));

    ERR_C_CHECK_NULL_PTR(filename, LOG_ERROR("filename cannot be NULL"));

    spiffs_c_prepare_filepath(handle->base_name, filename, &path[0], sizeof(path));
    err = stat(path, out_stats);
    if(err != 0) {
        LOG_ERROR("error %d when reading file %s stats: %s", err, filename, strerror(err));
        return err;
    }

    return err;
} 

static int spiffs_c_prepare_filepath(char* prefix_path, char* filename, char* out_path, size_t buflen) {
    char ret_path[64];
    size_t path_len = 0;
    memutil_zero_memory(out_path, sizeof(out_path));

    ERR_C_CHECK_NULL_PTR(filename, LOG_ERROR("filename cannot be NULL"));
    ERR_C_CHECK_NULL_PTR(prefix_path, LOG_ERROR("prefix_path cannot be NULL"));

    sprintf(out_path, "%s/%s", prefix_path, filename);
    path_len = strlen(&ret_path[0]);
    if(path_len > buflen) {
        LOG_ERROR("insufficient size to store filepath");
        out_path = NULL;
        return SPIFFS_C_ERR_NOT_ENOUGH_SPACE;
    }

    LOG_VERBOSE("prepared filepath: %s", ret_path);
    //provide path to caller
    strlcpy(&ret_path[0], out_path, path_len);
    return 0;

}

int spiffs_c_init(const spiffs_c_handle_t* handle) {
    volatile err_c_t err = 0;
    LOG_VERBOSE("initializing SPIFFS...");

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
    char path[128];
    memutil_zero_memory(&path, sizeof(path));

    ERR_C_CHECK_NULL_PTR(filename, LOG_ERROR("filename cannot be NULL"));
    ERR_C_CHECK_NULL_PTR(mode, LOG_ERROR("file opening mode cannot be NULL"));
    ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("SPIFFS controller handle cannot be NULL"));
    
    //prepare file path
    spiffs_c_prepare_filepath(handle->base_name, filename, &path[0], sizeof(path));
    LOG_VERBOSE("opening file on path: %s", path);
    (*out_file) = fopen(path, mode);

    ERR_C_CHECK_NULL_PTR((*out_file), LOG_ERROR("failed to open file %s in mode %s", filename, mode));

    return err;
}

void spiffs_c_close_file(FILE* file, const char* filename) {
    if(filename != NULL) {
        LOG_VERBOSE("closing file: %s", filename);
    }
    fclose(file);
}

int spiffs_c_read_file(spiffs_c_handle_t* handle, char* filename, char* out_data, size_t buflen) {
    err_c_t err = 0;
    FILE* file = NULL;
    struct stat stats = {0};

    ERR_C_CHECK_NULL_PTR(filename, LOG_ERROR("filename cannot be NULL"));
    err = spiffs_c_read_file_stats(handle, filename, &stats);
    if(err != 0) {
        LOG_ERROR("error %d when trying to get file %s stats: %s", err, filename, strerror(err));
        return err;
    }

    if(stats.st_size > buflen) {
        LOG_ERROR("not enough space to store filesize, filesize: %d, buffer space: %d", stats.st_size, buflen);
        return SPIFFS_C_ERR_NOT_ENOUGH_SPACE;
    }

    err = spiffs_c_open_file(handle, filename, "r+", &file);
    if(err != ERR_C_OK) {
        LOG_ERROR("error %d when trying to open file %s: %s", err, filename, error_to_name(err));
        spiffs_c_close_file(file, filename);
        return err;
    }

    err = fread(out_data, stats.st_size, 1, file);
    if(err == 0) {
        LOG_ERROR("error %d when trying to read data from file %s: %s", err, filename, strerror(err));
    } else if(err != stats.st_size) {
        LOG_WARN("WARNING! Number of bytes read from file different from file size - not whole file was read");
    } else {
        LOG_DEBUG("%d bytes was read from file %s", err, filename);
        return 0;       // fread returned number of bytes, so simply return 0 to signal success
    }

    spiffs_c_close_file(file, filename);
    return err;
}