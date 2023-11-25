#include <stdio.h>
#include <inttypes.h>
#include "logger.h"
#include <stdlib.h>


typedef int32_t err_t;

#define CHECK_NULL_PTR(ptr, message)                \
    if(!(void*)ptr) {                               \
        message;                                    \
        return ERR_NULL_POINTER;                    \
    }

#define CHECK_MEM_ALLOC(ptr, message)                \
    if(!(void*)ptr) {                               \
        message;                                    \
        return ERR_NO_MEMORY;                    \
    }

#define CHECK_ERROR(statement, message)         \
    err_t val = statement;                      \
    if(!val) {                            \
        message;                            \
        return val;                       \
    }


char* error_to_name(err_t err);

err_t err_check_null_pointer(void* ptr);

/**
 * @brief Wrapper function for std malloc, checks if allocation was successfull.
 * 
 * @param size_t size size of memory to allocate.
 * 
 * @return
 *          -void* to data if allocated memory
 *          -NULL if allocation wasn't successfull
*/
void* wrap_malloc(size_t size);

/**
 * @brief Wrapper function for std malloc, checks if freeing was successfull.
 * 
 * @param void* allocated - pointer to allocated memory.
 * 
 * @return
 *          - 0 on success
 *          - ERR_NULL_POINTER when allocated is NULL
*/
err_t wrap_free(void* allocated);
