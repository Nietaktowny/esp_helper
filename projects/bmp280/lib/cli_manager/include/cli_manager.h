/**
 * @file cli_manager.h
 * @author Wojciech Mytych
 * @brief Cli Manager library header file.
 * @version 1.0.9
 * @date 2024-02-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include "inttypes.h"
#include "cli_manager_errors.h"

#define CLI_DEFAULT_PORT                    27015                   ///< TCP Cli manager listen port.
#define CLI_KEEPALIVE_IDLE                  5                       ///< TCP keep-alive idle time(s).
#define CLI_KEEPALIVE_INTERVAL              5                       ///< TCP keep-alive interval time(s).
#define CLI_KEEPALIVE_COUNT                 3                       ///< TCP keep-alive packet retry send counts.
#define CLI_CMD_INTS_NUMBER                 2                       ///< Number of ints used to indicate command in received data.
#define CLI_CMD_ARGS_NUMBER                 64                      ///< Number of chars used to indicate arguments in received data.
#define CLI_CMD_MAX_COMMANDS                64                      ///< Max number of commands to register.
#define CLI_ACCEPT_TASK_DEFAULT_SIZE        4*1024                  ///< Default stack size used for cli_accept_task.

/**
 * @brief Log all logger library logs to remote host on TCP port.
 * 
 * @attention Ports under 1000 cannot be used, as they are restricted.
 * 
 * @param port Port on which set up TCP log server.
 * @param address Address on which set up TCP log server.
 * 
 * @retval ERR_C_NULL_POINTER If address is NULL.
 * @retval CLI_ERR_CONNECT_FAIL If error occurs during setup of TCP server.
 * @retval CLI_ERR_NO_INTERNET If device has no internet connection.
 * @retval 0 on success.
 */
int cli_set_remote_logging(uint16_t port, const char* address);

/**
 * @brief Task responsible for accepting new hosts in TCP log server.
 * 
 * It will block till new connection is accepted.
 * 
 * In case invalid listen socket is passed, task will delete itself.
 * 
 * Task name is the same as function.
 * 
 * @param socket Additional arguments passed to task, used to pass listen socket.
 */
void cli_accept_task(void* socket);
