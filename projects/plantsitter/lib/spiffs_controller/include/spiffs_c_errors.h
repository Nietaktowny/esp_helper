/**
 * @file spiffs_controller_errors.h
 * @author Wojciech Mytych
 * @brief Spiffs controller library errors list.
 * @version 1.0.4
 * @date 2024-02-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#define ERR_SPIFFS_C_BASE                       0x0190                          ///< Spiffs controller error base.
#define SPIFFS_C_ERR_NOT_ENOUGH_SPACE           0x0001 + ERR_SPIFFS_C_BASE      ///< Not enough space to store filepath.