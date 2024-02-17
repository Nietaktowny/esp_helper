/**
 * @file esp_helper_utils.h
 * @author Wojciech Mytych
 * @brief ESP Helper library header file.
 * @version 0.1
 * @date 2024-02-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "inttypes.h"
#include "stddef.h"

#define ESP_HELPER_CHIP_ID_SIZE 6

/**
 * @brief Get ESP Helper device ID.
 * 
 * @param chip_id Buffer to store device ID.
 * @return int @todo error checking
 */
int helper_get_chip_id(uint8_t chip_id[6]);

/**
 * @brief Prepare URL for OTA with device ID as parameters.
 * 
 * @param url Server URL, null terminated C string.
 * @param device_id Device ID, null terminated C string.
 * @param buffer Buffer to store prepared URL.
 * @param buflen Size of buffer.
 * 
 * @retval ERR_NULL_POINTER on NULL buffer, url or device_id
 * @retval ERR_C_MEMORY_ERR on too small buffer.
 */
int helper_prepare_url_with_device_id(const char* url, const char* device_id, char* buffer, size_t buflen);

/**
 * @brief Perform simple OTA firmware update with standard ESP Helper server.
 * 
 * @retval ERR_NULL_POINTER on internal NULL pointer
 * @retval ERR_C_MEMORY_ERR on internal memory error.
 * @retval OTA_C_ERR_IMAGES_THE_SAME If current firmware version is the same as new.
 * @retval esp_specific errors 
 */
int helper_perform_ota(void);