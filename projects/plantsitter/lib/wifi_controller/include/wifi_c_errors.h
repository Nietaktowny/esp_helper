/**
 * @file wifi_c_errors.h
 * @author Wojciech Mytych
 * @brief Wifi controller library errors list.
 * @version 2.0.3
 * @date 2024-02-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */

/**
 * @brief Definitions of error codes for wifi_controller.
 * 
 */
#pragma once

#define WIFI_C_ERR_BASE                 0x00C8                      ///< Wifi controller error base.
#define WIFI_C_ERR_NULL_SSID            0x01 + WIFI_C_ERR_BASE      ///< SSID for WiFi was null or zero length.
#define WIFI_C_ERR_WRONG_MODE           0x02 + WIFI_C_ERR_BASE      ///< Mode type of WiFI was wrong.
#define WIFI_C_ERR_NETIF_INIT_FAILED    0x03 + WIFI_C_ERR_BASE      ///< Failed to initialize netif - see wifi_c_init_netif() (CRITICAL).
#define WIFI_C_ERR_WIFI_ALREADY_INIT    0x04 + WIFI_C_ERR_BASE      ///< WiFi was already initialized once.
#define WIFI_C_ERR_NETIF_ALREADY_INIT   0x05 + WIFI_C_ERR_BASE      ///< Netif is already initialized.
#define WIFI_C_ERR_WRONG_PASSWORD       0x06 + WIFI_C_ERR_BASE      ///< Password lenght is too short for WIFI_AUTH_WPA2_PSK (need at least 8 characters).
#define WIFI_C_ERR_WIFI_NOT_STARTED     0x07 + WIFI_C_ERR_BASE      ///< Wifi was not started.
#define WIFI_C_ERR_WIFI_NOT_INIT        0x08 + WIFI_C_ERR_BASE      ///< Wifi was not initialized.
#define WIFI_C_ERR_SCAN_NOT_DONE        0x09 + WIFI_C_ERR_BASE      ///< Trying to read scan results without prior scanning.
#define WIFI_C_ERR_STA_NOT_STARTED      0x0A + WIFI_C_ERR_BASE      ///< Trying to scan without configuring nad starting STA.
#define WIFI_C_ERR_AP_NOT_FOUND         0x0B + WIFI_C_ERR_BASE      ///< Not found desired AP when scanning.
#define WIFI_C_ERR_NEITF_NOT_INIT       0x0C + WIFI_C_ERR_BASE      ///< Netif was not initialized.
#define WIFI_C_ERR_EVENT_LOOP_NOT_INIT  0x0D + WIFI_C_ERR_BASE      ///< Event loop was not started.
#define WIFI_C_ERR_STA_NOT_CONNECTED    0x0E + WIFI_C_ERR_BASE      ///< STA is not connected to any AP.
#define WIFI_C_ERR_STA_CONNECT_FAIL     0x0F + WIFI_C_ERR_BASE      ///< STA failed to connect to AP.
#define WIFI_C_ERR_STA_TIMEOUT_EXPIRE   0x10 + WIFI_C_ERR_BASE      ///< wifi_c_start_sta function timeout expired.