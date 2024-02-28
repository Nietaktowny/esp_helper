
#pragma once

#include <stddef.h>
#include "nvs_c_errors.h"

typedef struct nvs_c_handle_obj* nvs_c_handle_t;


int nvs_c_init_nvs (void);

int nvs_c_open(nvs_c_handle_t* handle, char* namespace);

int nvs_c_open_read_only(nvs_c_handle_t* handle, char* namespace);

int nvs_c_close(nvs_c_handle_t handle);

int nvs_c_write_string(nvs_c_handle_t handle, const char* key, const char* buffer);

int nvs_c_read_string(nvs_c_handle_t handle, const char* key, char* buffer, size_t buflen);

int nvs_c_erase_namespace(nvs_c_handle_t handle);