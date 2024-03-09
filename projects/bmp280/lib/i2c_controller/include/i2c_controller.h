#pragma once

#include <inttypes.h>
#include <stddef.h>
#include "i2c_c_errors.h"

/**
 * @brief Handle representing I2C bus.
*/
typedef struct i2c_c_bus_handle_obj *i2c_c_bus_handle_t;

/**
 * @brief Handle representing I2C device.
*/
typedef struct i2c_c_device_handle_obj *i2c_c_device_handle_t;

/**
 * @brief I2C port number, can be I2C_C_NUM_0, I2C_C_NUM_1.
 */
typedef enum
{
    I2C_C_NUM_0 = 0, /*!< I2C port 0 */
    I2C_C_NUM_1 = 1,
} i2c_c_port_t;

/**
 * @brief Initialize I2C bus and allocate memory for it.
 *
 * @param port I2C port, can be I2C_C_NUM_0 or I2C_C_NUM_1.
 * @param scl I2C SCL GPIO number.
 * @param sda I2C SDA GPIO number.
 * @param out_handle[out] Location where bus handle will be stored.
 *
 * @warning This function will exit when memory allocation fails.
 *
 * @retval - 0 on success
 * @retval - ERR_C_NULL_POINTER - when pointer to out_handle is NULL.
 * @retval - esp specific errors
 */
int i2c_c_init_bus(const i2c_c_port_t port, const uint8_t scl, const uint8_t sda, i2c_c_bus_handle_t *out_handle);

/**
 * @brief Delete bus handle and free bus resources.
 * 
 * @param bus Bus handle.
 * 
 * @retval - 0 on success
 * @retval - ERR_C_NULL_POINTER - when bus handle is NULL.
*/
int i2c_c_delete_bus(i2c_c_bus_handle_t bus);

/**
 * @brief Add I2C device and allocate memory for it.
 *
 * @param addr device 8-bit address
 * @param bus I2C bus handle.
 * @param out_handle[out] Location where device handle will be stored.
 *
 * @warning This function will exit when memory allocation fails.
 *
 * @retval - 0 on success
 * @retval - ERR_C_NULL_POINTER - when pointer to out_handle or bus is NULL.
 * @retval - esp specific errors.
 */
int i2c_c_add_device(const uint8_t addr, const i2c_c_bus_handle_t bus, i2c_c_device_handle_t *out_handle);

/**
 * @brief Delete I2C device handle and release resources.
 * 
 * @param device Device handle to delete.
 * 
 * @retval - 0 on success
 * @retval - ERR_C_NULL_POINTER - when device handle is NULL.
*/
int i2c_c_delete_device(i2c_c_device_handle_t device);

/**
 * @brief Change I2C device address.
 *
 * @param device Device handle.
 * @param new_addr New address of said device.
 *
 * @retval - 0 on success
 * @retval - ERR_C_NULL_POINTER - when device handle is NULL.
 * @retval - esp specific errors.
 */
int i2c_c_set_device_addr(i2c_c_device_handle_t device, const uint8_t new_addr);

/**
 * @brief Read register from I2C device.
 *
 * @param dev Device handle.
 * @param addr Register address.
 * @param out_buffer[out] Location where to store register value.
 * @param buflen Buffer size.
 * @param timeout Time after which function will return regardless of result of reading.
 * 
 * @retval - 0 on success
 * @retval - ERR_C_NULL_POINTER - when device handle is NULL.
 * @retval - I2C_C_ERR_LOCK_ERR - when lock acquisition fails.
 * @retval - esp specific errors. 
 */
int i2c_c_read(const i2c_c_device_handle_t dev, const uint8_t addr, uint8_t *out_buffer, const size_t buflen, const uint32_t timeout);

/**
 * @brief Write to register in I2C device.
 *
 * @param dev Device handle.
 * @param addr Register address.
 * @param in_buffer[in] Location of value to write
 * @param buflen Buffer size.
 * @param timeout Time after which function will return regardless of result of reading.
 * 
 * @retval - 0 on success
 * @retval - ERR_C_NULL_POINTER - when device handle or pointer to in_buffer are NULL.
 * @retval - I2C_C_ERR_LOCK_ERR - when lock acquisition fails.
 * @retval - esp specific errors. 
 */
int i2c_c_write(const i2c_c_device_handle_t dev, const uint8_t addr, const uint8_t *in_buffer, const size_t buflen, const uint32_t timeout);