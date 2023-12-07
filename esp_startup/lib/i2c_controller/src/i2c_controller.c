#include "i2c_controller.h"
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


struct i2c_c_bus_handle_obj {
    i2c_port_t port;
    uint8_t scl_gpio;
    uint8_t sda_gpio;
};

struct i2c_c_device_handle_obj {
    i2c_c_bus_handle_t bus;
    uint8_t device_addr;
};


int i2c_c_init_bus(i2c_c_port_t port, uint8_t scl, uint8_t sda, i2c_c_bus_handle_t* out_handle) {
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

int i2c_c_add_device(uint8_t addr, i2c_c_bus_handle_t bus, i2c_c_device_handle_t* out_handle) {
    err_c_t err = 0;
    CHECK_NULL_PTR(out_handle, LOG_ERROR("output handle cannot be NULL"));
    CHECK_NULL_PTR(bus, LOG_ERROR("I2C bus handle cannot be NULL"));

    NEW((*out_handle), struct i2c_c_device_handle_obj); //allocate memory
    (*out_handle)->device_addr = addr;
    (*out_handle)->bus = bus;
    return err;
}

int i2c_c_set_device_addr(i2c_c_device_handle_t device, uint8_t new_addr) {
    int err = 0;
    int prev_addr = 0;

    ERR_C_CHECK_NULL_PTR(device, LOG_ERROR("I2C device handle cannot be NULL"));
    
    prev_addr = device->device_addr;
    device->device_addr = new_addr;
    LOG_DEBUG("changed device address from %#X to %#X", prev_addr, new_addr);
    
    return err;
}

int i2c_c_read(i2c_c_device_handle_t dev, uint8_t addr, uint8_t* out_buffer, size_t buflen, uint32_t timeout) {
    volatile err_c_t err = 0;
    volatile i2c_cmd_handle_t cmd = NULL;

    ERR_C_CHECK_NULL_PTR(out_buffer, LOG_ERROR("buffer provided to store output of i2c read cannot be NULL"));
    Try {
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
    } Catch(err) {
        LOG_ERROR("error %d during reading registers on I2C: %s", err, esp_err_to_name(err));
        //free resources
        if(!cmd) {
            i2c_cmd_link_delete(cmd);
        }
    }
    return err;
}

int i2c_c_write(i2c_c_device_handle_t dev, uint8_t addr, uint8_t* in_buffer, size_t buflen, uint32_t timeout) {
    volatile err_c_t err = 0;
    volatile i2c_cmd_handle_t cmd = NULL;

    ERR_C_CHECK_NULL_PTR(in_buffer, LOG_ERROR("input buffer for i2c write cannot be NULL"));
    Try {
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
    } Catch(err) {
        LOG_ERROR("error %d during reading registers on I2C: %s", err, esp_err_to_name(err));
        //free resources
        if(!cmd) {
            i2c_cmd_link_delete(cmd);
        }
    }
    return err;
}