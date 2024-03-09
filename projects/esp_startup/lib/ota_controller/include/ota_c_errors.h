/**
 * @file ota_c_errors.h
 * @author Wojciech Mytych
 * @brief OTA Controller errors list.
 * @version 1.0.6
 * @date 2024-02-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#define OTA_C_ERR_BASE                      0x01F4                      ///< OTA Controller error base.
#define OTA_C_ERR_IMAGES_THE_SAME           0x0001 + OTA_C_ERR_BASE     ///< New and old image are the same.