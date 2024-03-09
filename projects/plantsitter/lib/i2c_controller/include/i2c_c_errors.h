/**
 * @file i2c_c_errors.h
 * @author Wojciech Mytych
 * @brief I2C Controller errors list.
 * @version 1.0.4
 * @date 2024-02-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#define I2C_C_ERR_BASE                  0x0320                          ///< I2C Controller error base.
#define I2C_C_ERR_LOCK_ERR              0x0001 + I2C_C_ERR_BASE         ///< Cannot obtain device lock.