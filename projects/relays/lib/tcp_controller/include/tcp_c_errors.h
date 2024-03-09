/**
 * @file tcp_c_errors.h
 * @author Wojciech Mytych
 * @brief TCP Controller errors list.
 * @version "1.2.3"
 * @date 2024-02-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#define ERR_TCP_BASE            0x012C                             ///< Tcp controller error base.
#define INVALID_SOCKET          0x0000-1 + ERR_TCP_BASE            ///< Invalid socket identifier.
#define ERR_TCP_ADDR_ERROR      0x0023 + ERR_TCP_BASE              ///< Error when preparing address.
#define ERR_TCP_INVALID_PORT    0x0024 + ERR_TCP_BASE              ///< Invalid port number.
#define ERR_TCP_INVALID_SOCKET  0x0025 + ERR_TCP_BASE              ///< Socket is invalid.
#define ERR_TCP_INVALID_ARGS    0x0026 + ERR_TCP_BASE              ///< Wrong arguments passed to function.