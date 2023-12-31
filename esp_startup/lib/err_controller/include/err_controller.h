#include <stdio.h>
#include <inttypes.h>
#include "logger.h"
#include <stdlib.h>
#include "CException.h"
#include "errors_list.h"

typedef int32_t err_c_t;


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
    err_c_t val = statement;                      \
    if(!val) {                            \
        message;                            \
        return val;                       \
    }

#define ERR_C_CHECK_AND_THROW_ERR(err) if(err != 0)  \
        {                                            \
                Throw(err);                          \
        }                                            \

#define ERR_C_LOG_AND_THROW_ERR(err, log) if(err != 0)  \
        {                                            \
                log;                                 \
                Throw(err);                          \
        }  

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
do {                                           \
    err_c_t val = statement;                      \
    if(!val) {                            \
        message;                            \
        return val;                       \
    }                                       \
} while(0)



char* error_to_name(err_c_t err);

err_c_t err_check_bitmask(uint32_t mask, uint32_t value);

err_c_t err_check_null_pointer(void* ptr);

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
err_c_t wrap_free(void* allocated);
