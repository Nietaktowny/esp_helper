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
#define NVS_C_ERR_WRONG_MODE            0x0001 + NVS_C_ERR_BASE     ///< NVS namespace was opened in read only mode.
#define NVS_C_ERR_NOT_INIT              0x0002 + NVS_C_ERR_BASE     ///< NVS was not init.
#define NVS_C_ERR_PARTITION_NOT_FOUND   0x0003 + NVS_C_ERR_BASE     ///< NVS partition was not found.
#define NVS_C_ERR_NAMESPACE_NOT_FOUND   0x0004 + NVS_C_ERR_BASE     ///< NVS namespace was not found in partition.
#define NVS_C_ERR_KEY_NOT_FOUND         0x0005 + NVS_C_ERR_BASE     ///< NVS key was not found in namespace.