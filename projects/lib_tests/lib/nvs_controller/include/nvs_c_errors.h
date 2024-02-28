/**
 * @file nvs_c_errors.h
 * @author Wojciech Mytych
 * @brief NVS Controller library errors list.
 * @version 0.1
 * @date 2024-02-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#define NVS_C_ERR_BASE                  0x0258                      ///< NVS Controller error base.
#define NVS_C_ERR_WRONG_MODE            0x0001 + NVS_C_ERR_BASE     ///< NVS was init in read only mode.