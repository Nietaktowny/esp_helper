/**
 * @file logger_errors.h
 * @author Wojciech Mytych
 * @brief Logger library errors list.
 * @version 2.0.3
 * @date 2024-02-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#define LOGGER_ERR_BASE         0x0064                                ///< Logger error base.
#define LOGGER_ERR_MUTEX_ERROR  0x0001 + LOGGER_ERR_BASE              ///< When some error with logger mutex occurred.
#define LOGGER_MUTEX_TIMEOUT    0x0002 + LOGGER_ERR_BASE              ///< When function getting semaphore timeout expired and cannot obtain log_mutex.
#define LOGGER_ERR_NULL_FILE    0x0003 + LOGGER_ERR_BASE              ///< When null pointer is passed as log file output.
#define LOGGER_NO_FREE_FILES    0x0004 + LOGGER_ERR_BASE              ///< When there are no new slots for log files, max number of files has been reached.