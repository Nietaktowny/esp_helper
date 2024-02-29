/**
 * @file cli_manager_errors.h
 * @author Wojciech Mytych
 * @brief CLI Manager library errors list.
 * @version 1.0.9
 * @date 2024-02-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#define CLI_ERR_BASE                    0x03E8                      ///< CLI Manager error base.
#define CLI_ERR_CONNECT_FAIL            0x0001 + CLI_ERR_BASE       ///< Error when trying to connect to remote host.
#define CLI_ERR_NO_INTERNET             0x0002 + CLI_ERR_BASE       ///< Device has no internet connection.

