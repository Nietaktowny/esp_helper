/**
 * @file err_c_errors.h
 * @author Wojciech Mytych
 * @brief Error helper library errors list.
 * @version 1.0.3
 * @date 2024-02-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#define ERR_C_ERR_BASE              0x0000                                  ///< Error controller error base.
#define ERR_C_OK                    0x0000  + ERR_C_ERR_BASE                ///< No error.
#define ERR_C_NULL_POINTER          0x0001  + ERR_C_ERR_BASE                ///< Null pointer was passed or dereferenced.
#define ERR_C_NO_MEMORY             0x0002  + ERR_C_ERR_BASE                ///< Memory allocation failed.
#define ERR_C_MEMORY_ERR            0x0003  + ERR_C_ERR_BASE                ///< Memory error occurred.
#define ERR_C_WRONG_ARGS            0x0004  + ERR_C_ERR_BASE                ///< Wrong arguments were used.