/**
 * @file err_controller.c
 * @author Wojciech Mytych
 * @brief Error helper library source file.
 * @version 1.0.4
 * @date 2024-02-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "err_controller.h"
#include "err_c_errors.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef ESP_PLATFORM
#include <esp_err.h>
#else
#include <errno.h>
#endif

err_c_t err_check_bitmask(uint32_t mask, uint32_t value) {
    return ((mask&value) == mask) ? ERR_C_OK : 1;
}