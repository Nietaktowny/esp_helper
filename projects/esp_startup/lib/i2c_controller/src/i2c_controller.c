#include "i2c_controller.h"
#include "i2c_controller_internal.h"
#include "logger.h"
#include "errors_list.h"
#include "err_controller.h"
#include "arena.h"
#include "memory_utils.h"

#include <inttypes.h>
#include <stddef.h>

#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

/**
 * @brief I2C bus object.
*/
struct i2c_c_bus_handle_obj {
    i2c_port_t port;
    uint8_t scl_gpio;
    uint8_t sda_gpio;
};

/**
 * @brief I2C device object.
*/
struct i2c_c_device_handle_obj {
    i2c_c_bus_handle_t bus;
    uint8_t device_addr;
    SemaphoreHandle_t lock;
};


int i2c_c_init_bus(const i2c_c_port_t port, const uint8_t scl, const uint8_t sda, i2c_c_bus_handle_t* out_handle) {
    err_c_t err = 0;
    ERR_C_CHECK_NULL_PTR(out_handle, LOG_ERROR("location where to store i2c bus handle cannot be NULL"));

    i2c_config_t config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = sda,
        .scl_io_num = scl,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 400 * 1000,
    };

    err = i2c_param_config((i2c_port_t)port, &config);
    if(err != ERR_C_OK) {
        LOG_ERROR("error %d during configuration of I2C interface: %s", err, esp_err_to_name(err));
        return err;
    }
    
    err = i2c_driver_install((i2c_port_t)port, I2C_MODE_MASTER, 0, 0, 0);
    if(err != ERR_C_OK) {
        LOG_ERROR("error %d during installing I2C drivers: %s", err, esp_err_to_name(err));
        return err;
    }

    //allocate and fill in bus handle
    NEW((*out_handle), struct i2c_c_bus_handle_obj);
    (*out_handle)->port = (i2c_port_t)port;
    (*out_handle)->scl_gpio = scl;
    (*out_handle)->sda_gpio = sda;

    return err;
}

int i2c_c_delete_bus(i2c_c_bus_handle_t bus) {
    err_c_t err = 0;
    
    CHECK_NULL_PTR(bus, LOG_ERROR("bus handle to delete cannot be NULL"));

    LOG_DEBUG("deleting I2C bus on port: %d", bus->port);
    memutil_zero_memory(bus, sizeof(struct i2c_c_bus_handle_obj));
    DELETE(bus);

    return err;
}

int i2c_c_add_device(const uint8_t addr, const i2c_c_bus_handle_t bus, i2c_c_device_handle_t* out_handle) {
    err_c_t err = 0;
    CHECK_NULL_PTR(out_handle, LOG_ERROR("output handle cannot be NULL"));
    CHECK_NULL_PTR(bus, LOG_ERROR("I2C bus handle cannot be NULL"));

    NEW((*out_handle), struct i2c_c_device_handle_obj); //allocate memory
    (*out_handle)->device_addr = addr;
    (*out_handle)->bus = bus;
    (*out_handle)->lock = xSemaphoreCreateMutex();
    return err;
}

int i2c_c_delete_device(i2c_c_device_handle_t device) {
    err_c_t err = 0;

    CHECK_NULL_PTR(device, LOG_ERROR("device handle to delete cannot be NULL"));

    LOG_DEBUG("deleting I2C device with address: %#x", device->device_addr);
    vSemaphoreDelete(device->lock);
    memutil_zero_memory(device, sizeof(struct i2c_c_device_handle_obj));
    DELETE(device);

    return err;
}

int i2c_c_set_device_addr(i2c_c_device_handle_t device, const uint8_t new_addr) {
    int err = 0;
    int prev_addr = 0;

    ERR_C_CHECK_NULL_PTR(device, LOG_ERROR("I2C device handle cannot be NULL"));
    
    prev_addr = device->device_addr;
    device->device_addr = new_addr;
    LOG_DEBUG("changed device address from %#X to %#X", prev_addr, new_addr);
    
    return err;
}

int i2c_c_read(const i2c_c_device_handle_t dev, const uint8_t addr, uint8_t* out_buffer, const size_t buflen, const uint32_t timeout) {
    volatile err_c_t err = 0;
    volatile i2c_cmd_handle_t cmd = NULL;

    ERR_C_CHECK_NULL_PTR(out_buffer, LOG_ERROR("buffer provided to store output of i2c read cannot be NULL"));
    ERR_C_CHECK_NULL_PTR(dev, LOG_ERROR("device handle cannot be NULL"));
    Try {
        ERR_C_CHECK_AND_THROW_ERR(i2c_c_device_lock(dev, timeout));
        cmd = i2c_cmd_link_create();
        // Write register address
        ERR_C_CHECK_AND_THROW_ERR(i2c_master_start(cmd));
        ERR_C_CHECK_AND_THROW_ERR(i2c_master_write_byte(cmd, dev->device_addr | I2C_MASTER_WRITE, true));
        ERR_C_CHECK_AND_THROW_ERR(i2c_master_write_byte(cmd, addr, true));

        // Read Registers
        ERR_C_CHECK_AND_THROW_ERR(i2c_master_start(cmd));
        ERR_C_CHECK_AND_THROW_ERR(i2c_master_write_byte(cmd, dev->device_addr | I2C_MASTER_READ, true));
        ERR_C_CHECK_AND_THROW_ERR(i2c_master_read(cmd, out_buffer, buflen, I2C_MASTER_LAST_NACK));
        ERR_C_CHECK_AND_THROW_ERR(i2c_master_stop(cmd));

        ERR_C_CHECK_AND_THROW_ERR(i2c_master_cmd_begin(dev->bus->port, cmd, pdMS_TO_TICKS(timeout)));
        i2c_cmd_link_delete(cmd);
        ERR_C_CHECK_AND_THROW_ERR(i2c_c_device_unlock(dev));
    } Catch(err) {
        LOG_ERROR("error %d during reading registers on I2C: %s", err, esp_err_to_name(err));
        //free resources
        if(!cmd) {
            i2c_cmd_link_delete(cmd);
        }
        i2c_c_device_unlock(dev);
    }
    return err;
}

int i2c_c_write(const i2c_c_device_handle_t dev, const uint8_t addr, const uint8_t* in_buffer, const size_t buflen, const uint32_t timeout) {
    volatile err_c_t err = 0;
    volatile i2c_cmd_handle_t cmd = NULL;

    ERR_C_CHECK_NULL_PTR(in_buffer, LOG_ERROR("input buffer for i2c write cannot be NULL"));
    Try {
        ERR_C_CHECK_AND_THROW_ERR(i2c_c_device_lock(dev, timeout));
        cmd = i2c_cmd_link_create();
        for(int i = 0; i < buflen; i++) {
            ERR_C_CHECK_AND_THROW_ERR(i2c_master_start(cmd));
            ERR_C_CHECK_AND_THROW_ERR(i2c_master_write_byte(cmd, dev->device_addr | I2C_MASTER_WRITE, true));
            //register
            ERR_C_CHECK_AND_THROW_ERR(i2c_master_write_byte(cmd, addr + i, true));
            //data
            ERR_C_CHECK_AND_THROW_ERR(i2c_master_write_byte(cmd, in_buffer[i], true));
        }
        ERR_C_CHECK_AND_THROW_ERR(i2c_master_stop(cmd));

        ERR_C_CHECK_AND_THROW_ERR(i2c_master_cmd_begin(dev->bus->port, cmd, pdMS_TO_TICKS(timeout)));
        i2c_cmd_link_delete(cmd);
        ERR_C_CHECK_AND_THROW_ERR(i2c_c_device_unlock(dev));
    } Catch(err) {
        LOG_ERROR("error %d during reading registers on I2C: %s", err, esp_err_to_name(err));
        //free resources
        if(!cmd) {
            i2c_cmd_link_delete(cmd);
        }
        i2c_c_device_unlock(dev);
    }
    return err;
}

//I2C INTERNAL

int i2c_c_device_lock(i2c_c_device_handle_t const device, const uint32_t timeout) {
    err_c_t err = 0;
    ERR_C_CHECK_NULL_PTR(device, LOG_DEBUG("device handle cannot be NULL to acquire lock!"));

    err = xSemaphoreTake(device->lock, pdMS_TO_TICKS(timeout));
    
    if(err == pdFALSE) {
        LOG_ERROR("could not obtain %#x device lock", device->lock);
        return I2C_C_ERR_LOCK_ERR;
    }
    //pdTRUE is 1, to keep things the same across whole lib, simply return 0 on success
    return 0;
}

int i2c_c_device_unlock(i2c_c_device_handle_t const device) {
    err_c_t err = 0;
    ERR_C_CHECK_NULL_PTR(device, LOG_DEBUG("device handle cannot be NULL to return lock!"));

    err = xSemaphoreGive(device->lock);
    
    if(err == pdFALSE) {
        LOG_ERROR("could not return lock of device: %#x", device->device_addr);
        return I2C_C_ERR_LOCK_ERR;
    }
    //pdTRUE is 1, to keep things the same across whole lib, simply return 0 on success
    return 0;
}
