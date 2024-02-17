#include "err_controller.h"
#include "errors_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <esp_err.h>

err_c_t err_check_null_pointer(void* ptr) {
    return !ptr ? ERR_NULL_POINTER : ERR_C_OK;
}

err_c_t err_check_bitmask(uint32_t mask, uint32_t value) {
    return ((mask&value) == mask) ? ERR_C_OK : 1;
}

void* wrap_malloc(size_t size) {
    void* data = NULL;          // data to return
    if(!size) {
        LOG_WARN("zero size allocations are prohibited");
        return NULL;
    }
    data = malloc(size);
    if(!data) {
        LOG_ERROR("no memory for allocation");
        return NULL;
    }

    return data;
}

err_c_t wrap_free(void* allocated) {
    CHECK_NULL_PTR(allocated, LOG_ERROR("cannot free NULL ptr"));
    free(allocated);
    return 0;
}


char* error_to_name(err_c_t err) {
    switch (err)
    {
    case ERR_NO_MEMORY:
        return "error when allocating memory";
    case ERR_NULL_POINTER:
        return "passed NULL pointer";
    case ERR_WRONG_ARGS:
        return "wrong arguments";
    default:
        return esp_err_to_name(err);
    }
    return "i don't know?";
}