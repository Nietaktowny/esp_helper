#pragma once

#include <inttypes.h>
#include <stddef.h>


typedef struct i2c_c_bus_handle_obj* i2c_c_bus_handle_t;
typedef struct i2c_c_device_handle_obj* i2c_c_device_handle_t;


/**
 * @brief I2C port number, can be I2C_C_NUM_0, I2C_C_NUM_1.
 */
typedef enum {
    I2C_C_NUM_0 = 0,              /*!< I2C port 0 */
    I2C_C_NUM_1 = 1,
} i2c_c_port_t;


int i2c_c_init_bus(i2c_c_port_t port, uint8_t scl, uint8_t sda, i2c_c_bus_handle_t* out_handle);

int i2c_c_add_device(uint8_t addr, i2c_c_bus_handle_t bus, i2c_c_device_handle_t* out_handle);

int i2c_c_set_device_addr(i2c_c_device_handle_t device, uint8_t new_addr);

int i2c_c_read(i2c_c_device_handle_t dev, uint8_t addr, uint8_t* out_buffer, size_t buflen, uint32_t timeout);

int i2c_c_write(i2c_c_device_handle_t dev, uint8_t addr, uint8_t* in_buffer, size_t buflen, uint32_t timeout);