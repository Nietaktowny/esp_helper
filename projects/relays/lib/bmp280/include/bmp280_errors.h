/**
 * @file bmp280_errors.h
 * @author Wojciech Mytych
 * @brief BMP280 library errors list.
 * @version 1.0.4
 * @date 2024-02-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#define BMP_ERR_BASE                        0x044C                          ///< BMP280 error base.
#define BMP_ERR_NO_DEVICE_FOUND             0x0001 + BMP_ERR_BASE           ///< No BMP280 device was found on bus.
#define BMP_ERR_MODE_ERROR                  0x0002 + BMP_ERR_BASE          ///< Cannot change BMP device operating mode.