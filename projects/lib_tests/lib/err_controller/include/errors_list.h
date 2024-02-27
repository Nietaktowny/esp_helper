/**
 * @file errors_list.h
 * @author Wojciech Mytych
 * @brief Error helper library errors list.
 * @version 1.0.3
 * @date 2024-02-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once


#define ERR_C_OK                    0x0000                  ///< No error.
#define ERR_C_NULL_POINTER          0x0A01                  ///< Null pointer was passed or dereferenced.
#define ERR_C_NO_MEMORY             0x0A02                  ///< Memory allocation failed.
#define ERR_C_MEMORY_ERR            0x0A02                  ///< Memory error occurred.
#define ERR_C_WRONG_ARGS            0x0A03                  ///< Wrong arguments were used.