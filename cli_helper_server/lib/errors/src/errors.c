#include "errors.h"
#include "errors_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

err_t err_check_null_pointer(void* ptr) {
    return !ptr ? ERR_NULL_POINTER : ERR_OK;
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

err_t wrap_free(void* allocated) {
    CHECK_NULL_PTR(allocated, LOG_ERROR("cannot free NULL ptr"));
    free(allocated);
    return 0;
}


char* error_to_name(err_t err) {
    switch (err)
    {
    case ERR_NO_MEMORY:
        return "error when allocating memory";
    case ERR_NULL_POINTER:
        return "passed NULL pointer";
    case ERR_WRONG_ARGS:
        return "wrong arguments";
    default:
        strerror(err);
    }
    return "i don't know?";
}