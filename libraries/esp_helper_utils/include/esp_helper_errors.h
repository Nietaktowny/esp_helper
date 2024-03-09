/**
 * @file esp_helper_errors.h
 * @author Wojciech Mytych
 * @brief ESP Helper library errors list.
 * @version 1.0.6
 * @date 2024-02-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#define ESP_HELPER_ERR_BASE             0x0384                          ///< ESP Helper error base.
#define ESP_HELPER_ERR_NOT_CONNECTED    0x0001 + ESP_HELPER_ERR_BASE    ///< When trying to perform OTA when STA is not connected.
