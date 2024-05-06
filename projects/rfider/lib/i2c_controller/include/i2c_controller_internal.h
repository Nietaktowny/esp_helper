#pragma once

#include "i2c_controller.h"

/**
 * @brief Get I2C device lock. Used for thread safety.
 * 
 * @param device Device handle.
 * @param timeout Time in miliseconds to wait for lock.
 * 
 * @retval - 0 on success
 * @retval - ERR_C_NULL_POINTER - when device handle is NULL.
 * @retval - I2C_C_ERR_LOCK_ERR - when it fails to get lock before timeout expires.
*/
int i2c_c_device_lock(i2c_c_device_handle_t const device, const uint32_t timeout);

/**
 * @brief Return I2C device lock. Used for thread safety.
 * 
 * @param device Device handle.
 * 
 * @retval - 0 on success
 * @retval - ERR_C_NULL_POINTER - when device handle is NULL.
 * @retval - I2C_C_ERR_LOCK_ERR - when it fails to return lock.
*/
int i2c_c_device_unlock(i2c_c_device_handle_t const device);

