/**
 * @file err_controller.h
 * @author Wojciech Mytych
 * @brief Error helper library header.
 * @version 1.0.3
 * @date 2024-02-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once

#include <inttypes.h>
#include "CException.h"
#include "errors_list.h"

/**
 * @brief Standard error type typedef.
 * 
 */
typedef int32_t err_c_t;


#define CHECK_NULL_PTR(ptr, message)                \
    if(!(void*)ptr) {                               \
        message;                                    \
        return ERR_C_NULL_POINTER;                    \
    }

#define CHECK_MEM_ALLOC(ptr, message)                \
    if(!(void*)ptr) {                               \
        message;                                    \
        return ERR_C_NO_MEMORY;                    \
    }

#define CHECK_ERROR(statement, message)         \
    err_c_t val = statement;                      \
    if(!val) {                            \
        message;                            \
        return val;                       \
    }

/**
 * @brief Checks error and if it's different from ERR_C_OK throws error.
 * 
 * @param err Error code to check.
 * 
 * @warning This macro should only be used inside CException Try{} macro.
 * 
 */
#define ERR_C_CHECK_AND_THROW_ERR(err)   \
        do {                                            \
            err_c_t val_err = err;                       \
            if(val_err != 0) {                          \
                Throw(val_err);                          \
            }                                            \
                                                        \
        } while(0)                                      

/**
 * @brief Checks error and if it's different from ERR_C_OK throws error and logs error name. 
 * 
 * @param err Error code to check.
 * @param message Log function or macro to log error message (for example LOG_ERROR("error"))
 * 
 * @attention message passed to macro IS NOT STRING, but whole logging function.
 * 
 * @warning This macro should only be used inside CException Try{} macro.
 * 
 */
#define ERR_C_LOG_AND_THROW_ERR(err, message) if(err != 0)  \
        {                                            \
                message;                                 \
                Throw(err);                          \
        }  

/**
 * @brief Sets error holding variable to passed error code, and throws error.
 * 
 * @param err Error variable to be set with error code.
 * @param err_code Error code to be used as variable value, and thrown.
 * 
 * @warning This macro should only be used inside CException Try{} macro.
 * 
 * 
 */
#define ERR_C_SET_AND_THROW_ERR(err, err_code)       \
        err = err_code;                              \
        Throw(err)                                   \

/**
 * @brief Checks if passed pointer is NULL, if so logs message, and returns ERR_C_NULL_POINTER error.
 * 
 * @attention message passed to macro IS NOT STRING, but whole logging function.
 * 
 * @warning This macro is used to return from function returning int value.
 * 
 * @param ptr Pointer to check if it's NULL.
 * @param message Log function or macro to log error message (for example LOG_ERROR("error")).
 * 
 * @retval ERR_C_NULL_POINTER
 * 
 */
#define ERR_C_CHECK_NULL_PTR(ptr, message)                \
    if(!(void*)ptr) {                               \
        message;                                    \
        return ERR_C_NULL_POINTER;                    \
    }

/**
 * @brief Checks if allocated pointer is NULL, if so logs message, and returns ERR_C_NO_MEMORY error.
 * 
 * @attention message passed to macro IS NOT STRING, but whole logging function.
 * 
 * @warning This macro is used to return from function returning int value.
 * 
 * @param ptr Pointer to check if it's NULL.
 * @param message Log function or macro to log error message (for example LOG_ERROR("error"))
 * 
 * @retval ERR_C_NO_MEMORY
 * 
 */
#define ERR_C_CHECK_MEM_ALLOC(ptr, message)                \
    if(!(void*)ptr) {                               \
        message;                                    \
        return ERR_C_NO_MEMORY;                    \
    }    


/**
 * @brief Checks if error code is ERR_C_OK, if not then logs error message and returns passed error code.
 * 
 * @attention message passed to macro IS NOT STRING, but whole logging function.
 * 
 * @warning This macro is used to return from function returning int value.
 * 
 * @param statement Value or function returning error code to check.
 * @param message Log function or macro to log error message (for example LOG_ERROR("error"))
 * 
 */
#define ERR_C_CHECK_ERROR(statement, message)         \
do {                                           \
    err_c_t val = statement;                      \
    if(!val) {                            \
        message;                            \
        return val;                       \
    }                                       \
} while(0)


/**
 * @brief Translate error code to error name.
 * 
 * If ESP_PLATFORM is defined, and it doesn't find any predefined error codes, it uses esp_err_to_name function.
 * 
 * If ESP_PLATFORM is not defined, and it doesn't find any predefined error codes, it uses strerror function.
 * 
 * @param err Error code to translate.
 * 
 * @return const char* Error name.
 */
const char* error_to_name(err_c_t err);

/**
 * @brief Check if bitmask matches value.
 * 
 * @param mask Bitmask to be used in checking.
 * @param value Value to test with bitmask
 * 
 * @retval ERR_C_OK if value matches bitmask.
 * @retval 1 if value doesn't match bitmask.
 */
err_c_t err_check_bitmask(uint32_t mask, uint32_t value);

/**
 * @brief Check pointer it it's null.
 * 
 * @param ptr Pointer to check.
 *
 * @retval ERR_C_NULL_POINTER If pointer is null.
 * @retval ERR_C_OK If pointer is not null.
 */
err_c_t err_check_null_pointer(void* ptr);
