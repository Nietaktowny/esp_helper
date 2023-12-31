#pragma once

#include "CException.h"

typedef int err_c_t;

#define ERR_C_CHECK_AND_THROW_ERR(err) if(err != 0)  \
        {                                            \
                Throw(err);                          \
        }                                            \


#define ERR_C_SET_AND_THROW_ERR(err, err_code)       \
        err = err_code;                              \
        Throw(err)                                   \

#define ERR_C_CHECK_NULL_PTR(ptr, message)                \
    if(!(void*)ptr) {                               \
        message;                                    \
        return ERR_NULL_POINTER;                    \
    }

#define ERR_C_CHECK_MEM_ALLOC(ptr, message)                \
    if(!(void*)ptr) {                               \
        message;                                    \
        return ERR_NO_MEMORY;                    \
    }

#define ERR_C_CHECK_ERROR(statement, message)         \
    err_mt val = statement;                      \
    if(!val) {                            \
        message;                            \
        return val;                       \
    }

typedef enum {
        ERR_C_OK = 0,
        ERR_C_INVALID_ARGS,
        ERR_C_NO_MEMORY,
        ERR_C_MEMORY_ERR,
} err_c_error_t;

char* err_c_read_esp_err_code(err_c_t err);