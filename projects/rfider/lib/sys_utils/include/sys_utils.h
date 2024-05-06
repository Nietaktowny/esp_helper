/**
 * @file sys_utils.h
 * @author Wojciech Mytych
 * @brief Source file of sys_utils library.
 * @version 1.0.3
 * @date 2024-02-17
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include "inttypes.h"
#include "stddef.h"
#include <stdbool.h>

/**
 * @brief Get base chip MAC address.
 *
 * @see
 * https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/api-reference/system/misc_system_api.html#mac-address
 *
 * @param mac Buffer to store MAC address.
 */
void sysutil_get_chip_base_mac(uint8_t mac[6]);

/**
 * @brief Get base chip MAC address as C string.
 *
 * @param buffer Buffer to store string.
 * @param buflen Size of buffer.
 *
 * @retval ERR_C_NULL_POINTER on NULL buffer.
 * @retval ERR_C_MEMORY_ERR on too small buffer.
 */
int sysutil_get_chip_base_mac_as_str(char *buffer, size_t buflen);

/* -------------------------------*/
/**
 * @brief Setup SNTP server for adjusting system time.

 * @note  Timezone is set to UTC-1
 *
 * @note If block is set to true, function will block till time is updated.
 *
 * @param ntp_server Name of SNTP server.
 * @param block Block if true.
 *
 * @retval   ERR_C_OK on success.
 * @retval   esp-idf specific errors.
 */
/* ---------------------------------*/
int sysutil_setup_ntp_server(const char *ntp_server, bool block);
