/**
 * @file spi_controller.c
 * @author Wojciech Mytych (wojciech.lukasz.mytych@gmail.com)
 * @brief SPI controller source file.
 * @version 1.0.0
 * @date 2024-05-07
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "spi_controller.h"
#include "err_controller.h"
#include "memory_utils.h"

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

int spi_c_write_reg(spi_c_handle_t handle, const uint8_t data,
                    bool keep_cs_active) {
  err_c_t err = ERR_C_OK;

  return err;
}

int spi_c_read_reg(spi_c_handle_t handle, const uint8_t data,
                   bool keep_cs_active) {
  err_c_t err = ERR_C_OK;

  return err;
}
