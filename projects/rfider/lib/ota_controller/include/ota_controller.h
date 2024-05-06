
#pragma once

#include <stddef.h>
#include <inttypes.h>
#include "ota_c_errors.h"

int ota_c_do_simple_ota(const char* url);

int ota_c_update_device_data(const char* url, const char* path, const char* device_id, const char* board);

int ota_c_start(const char* url);