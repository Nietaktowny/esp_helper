/**
 * @file err_controller.h
 * @author Wojciech Mytych
 * @brief Error helper library header.
 * @version 1.0.4
 * @date 2024-02-28
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

#include <inttypes.h>
#include "CException.h"
#include "err_c_errors.h"

/**
 * @brief Standard error type typedef.
 *
 */
typedef int32_t err_c_t;


/**
 * @brief Checks error and if it's different from ERR_C_OK throws error.
 *
 * @param err Error code to check.
 *
 * @warning This macro should only be used inside CException Try{} macro.
 *
 */
#define ERR_C_CHECK_AND_THROW_ERR(err) \
    do                                 \
    {                                  \
        err_c_t val_err = err;         \
        if (val_err != 0)              \
        {                              \
            Throw(val_err);            \
        }                              \
                                       \
    } while (0)

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
#define ERR_C_LOG_AND_THROW_ERR(err, message) \
    do                                        \
    {                                         \
        if (err != 0)                         \
        {                                     \
            message;                          \
            Throw(err);                       \
        }                                     \
    } while (0)

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
#define ERR_C_SET_AND_THROW_ERR(err, err_code) \
    do                                         \
    {                                          \
        err = err_code;                        \
        Throw(err);                            \
    } while (0)

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
#define ERR_C_CHECK_NULL_PTR(ptr, message) \
    do                                     \
    {                                      \
        if (!(void *)ptr)                  \
        {                                  \
            message;                       \
            return ERR_C_NULL_POINTER;     \
        }                                  \
    } while (0)

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
#define ERR_C_CHECK_MEM_ALLOC(ptr, message) \
    do                                      \
    {                                       \
        if (!(void *)ptr)                   \
        {                                   \
            message;                        \
            return ERR_C_NO_MEMORY;         \
        }                                   \
    } while (0)

/**
 * @brief Checks if error code is ERR_C_OK, if not then logs error message and returns passed error code.
 *
 * @attention message passed to macro IS NOT STRING, but whole logging function.
 *
 * @warning This macro is used to return from function returning int value.
 *
 * @param err Value or function returning error code to check.
 * @param message Log function or macro to log error message (for example LOG_ERROR("error"))
 *
 */
#define ERR_C_CHECK_ERROR(err, message) \
    do                                  \
    {                                   \
        err_c_t val = err;              \
        if (val)                        \
        {                               \
            message;                    \
            return val;                 \
        }                               \
    } while (0)

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
const char *error_to_name(err_c_t err);

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
