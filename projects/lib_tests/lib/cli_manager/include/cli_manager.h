/**
 * @file cli_manager.h
 * @author Wojciech Mytych
 * @brief Cli Manager library header file.
 * @version 1.0.7
 * @date 2024-02-23
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include "inttypes.h"

#define CLI_PORT                        27015                   ///< TCP Cli manager listen port.
#define CLI_KEEPALIVE_IDLE              5                       ///< TCP keep-alive idle time(s).
#define CLI_KEEPALIVE_INTERVAL          5                       ///< TCP keep-alive interval time(s).
#define CLI_KEEPALIVE_COUNT             3                       ///< TCP keep-alive packet retry send counts.
#define CLI_CMD_INTS_NUMBER             2                       ///< Number of ints used to indicate command in received data.
#define CLI_CMD_ARGS_NUMBER             64                      ///< Number of chars used to indicate arguments in received data.
#define CLI_CMD_MAX_COMMANDS            64                      ///< Max number of commands to register.

#define CLI_ERR_BASE                    0x0500                  
#define CLI_ERR_CONNECT_FAIL            0x0001 + CLI_ERR_BASE   ///< Error when trying to connect to remote host.
#define CLI_ERR_NO_INTERNET             0x0002 + CLI_ERR_BASE   ///< Device has no internet connection.

/**
 * @brief Log all logger library logs to remote host on TCP port.
 * 
 * @attention Ports under 1000 cannot be used, as they are restricted.
 * 
 * @param port Port on which set up TCP server.
 * @param address Address on which set up TCP server.
 * 
 * @retval ERR_NULL_POINTER If address is NULL.
 * @retval CLI_ERR_CONNECT_FAIL If error occurs during setup of TCP server.
 * @retval CLI_ERR_NO_INTERNET If device has no internet connection.
 */
int cli_set_remote_logging(uint16_t port, const char* address);

/**
 * @brief Task responsible for accepting new hosts in TCP server.
 * 
 * @param args Additional arguments passed to task.
 */
void cli_accept_task(void* args);