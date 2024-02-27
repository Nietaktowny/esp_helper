/**
 * @file err_controller.c
 * @author Wojciech Mytych
 * @brief Error helper library source file.
 * @version 1.0.3
 * @date 2024-02-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "err_controller.h"
#include "errors_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef ESP_PLATFORM
#include <esp_err.h>
#else
#include <errno.h>
#endif

err_c_t err_check_null_pointer(void* ptr) {
    return !ptr ? ERR_C_NULL_POINTER : ERR_C_OK;
}

err_c_t err_check_bitmask(uint32_t mask, uint32_t value) {
    return ((mask&value) == mask) ? ERR_C_OK : 1;
}


const char* error_to_name(err_c_t err) {
    switch (err)
    {
    case ERR_C_NO_MEMORY:
        return "error when allocating memory";
    case ERR_C_NULL_POINTER:
        return "passed NULL pointer";
    case ERR_C_WRONG_ARGS:
        return "wrong arguments";
    default:
        #ifdef ESP_PLATFORM
        return esp_err_to_name(err);
        #else
        err = errno;
        return strerror(err);
        #endif
    }
    return "i don't know?";
}