/**
 * @file spi_controller.h
 * @author Wojciech Mytych (wojciech.lukasz.mytych@gmail.com)
 * @brief SPI controller header file.
 * @version 1.0.0
 * @date 2024-05-07
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

#include "inttypes.h"

/**
 * @brief Types of available SPI transaction modes.
 *
 */
typedef enum {
  SPI_C_MODE_POLLING,
  SPI_C_MODE_INTERRUPT,
} spi_c_mode_t;

/**
 * @brief SPI controller handle object. Used to keep context of library.
 *
 */
typedef struct {
  void *esp_handle;  /*!< ESP-IDF specific handle. */
  spi_c_mode_t mode; /*!< SPI transaction mode. */
  uint8_t miso;      /*!< MISO GPIO. */
  uint8_t mosi;      /*!< MOSI GPIO. */
  uint8_t clk;       /*!< Clock GPIO. */
  uint8_t cs;        /*!< Chip select GPIO. */
} spi_c_handle_t;
