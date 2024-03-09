/**
 * @file esp_helper_utils.c
 * @author Wojciech Mytych
 * @brief ESP Helper utils library source file.
 * @version 1.0.6
 * @date 2024-02-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "esp_helper_utils.h"
#include "sys_utils.h"
#include "logger.h"
#include "err_controller.h"
#include "memory_utils.h"
#include "ota_controller.h"
#include "wifi_controller.h"

#include <stdbool.h>

#define ESP_HELPER_OTA_URL "http://wmytych.usermd.net/modules/getters/ota.php"

#define ESP_HELPER_MIN_URL_SIZE     100         ///< Minimal buffer size to store URLs prepared by ESP Helper utils.

/**
 * @todo Do some bit-magic with mac address so it's not obvious.
 * @todo tests after bit-magic is implemented
 */
int helper_get_chip_id(uint8_t chip_id[6])
{
    err_c_t err = 0;

    sysutil_get_chip_base_mac(chip_id);
    LOG_VERBOSE("Mac address: %02x:%02x:%02x:%02x:%02x:%02x", chip_id[0], chip_id[1], chip_id[2], chip_id[3], chip_id[4], chip_id[5]);

    return err;
}

int helper_prepare_url_with_device_id(const char *url, const char *device_id, char *buffer, size_t buflen)
{
    err_c_t err = 0;
    ERR_C_CHECK_NULL_PTR(url, LOG_ERROR("url to perfom ota cannot be NULL"));
    ERR_C_CHECK_NULL_PTR(buffer, LOG_ERROR("buffer to store URL cannot be NULL"));
    ERR_C_CHECK_NULL_PTR(device_id, LOG_ERROR("device_id cannot be NULL"));
    if (buflen == 0)
    {
        LOG_ERROR("buffer length to store URL cannot be zero");
        return ERR_C_MEMORY_ERR;
    } else if (buflen < ESP_HELPER_MIN_URL_SIZE) {
        LOG_ERROR("buffer to store URL must be at least %d bytes big.", ESP_HELPER_MIN_URL_SIZE);
        return ERR_C_MEMORY_ERR;
    }

    snprintf(buffer, buflen, "%s?device_id=%s", url, device_id);
    LOG_DEBUG("prepared url with device id for ota update:\n%s", buffer);
    return err;
}

int helper_perform_ota_with_id(const char* device_id)
{
    volatile err_c_t err = 0;

    ERR_C_CHECK_NULL_PTR(device_id, LOG_ERROR("device_id cannot be NULL to perform OTA update"));

    Try
    {
        char url[128] = {0};
        wifi_c_status_t* wifi = wifi_c_get_status();

        if(wifi->sta_connected == false) {
            LOG_ERROR("STA needs to be connected to Wifi to perform OTA.");
            return ESP_HELPER_ERR_NOT_CONNECTED;
        }
        ERR_C_CHECK_AND_THROW_ERR(helper_prepare_url_with_device_id(ESP_HELPER_OTA_URL, device_id, url, sizeof(url)));
        ERR_C_CHECK_AND_THROW_ERR(ota_c_start(url));
    }
    Catch(err)
    {
        LOG_ERROR("error %d when esp helper tried to perform OTA: %s", err, error_to_name(err));
    }

    return err;
}

int helper_perform_ota(void)
{
    volatile err_c_t err = 0;

    Try
    {
        char url[128] = {0};
        char device_id[20] = {0};
        wifi_c_status_t* wifi = wifi_c_get_status();

        if(wifi->sta_connected == false) {
            LOG_ERROR("STA needs to be connected to Wifi to perform OTA.");
            return ESP_HELPER_ERR_NOT_CONNECTED;
        }
        ERR_C_CHECK_AND_THROW_ERR(sysutil_get_chip_base_mac_as_str(device_id, sizeof(device_id)));
        ERR_C_CHECK_AND_THROW_ERR(helper_prepare_url_with_device_id(ESP_HELPER_OTA_URL, device_id, url, sizeof(url)));
        ERR_C_CHECK_AND_THROW_ERR(ota_c_start(url));
    }
    Catch(err)
    {
        LOG_ERROR("error %d when esp helper tried to perform OTA: %s", err, error_to_name(err));
    }

    return err;
}