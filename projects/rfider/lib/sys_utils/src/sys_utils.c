/**
 * @file sys_utils.c
 * @author Wojciech Mytych
 * @brief Source file of sys_utils library.
 *
 * @todo Implement FreeRTOS wrapper functions (delay, create task, delete task
 * itd.) or use pthreads
 *
 * @version 1.0.3
 * @date 2024-02-17
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "sys_utils.h"

#include "err_controller.h"
#include "esp_mac.h"
#include "esp_netif_sntp.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "logger.h"
#include "memory_utils.h"

#define SYSUTIL_MINIMAL_MAC_STR_LEN 18
#define SYSUTIL_SNTP_SYNC_RTRY_COUNT 150
#define SYSUTIL_SNTP_TIMEOUT 2000

void sysutil_get_chip_base_mac(uint8_t mac[6]) { esp_base_mac_addr_get(mac); }

int sysutil_get_chip_base_mac_as_str(char *buffer, size_t buflen) {
  err_c_t err = 0;
  uint8_t mac[6] = {0};
  ERR_C_CHECK_NULL_PTR(
      buffer, LOG_ERROR("buffer to store mac address string cannot be NULL"));

  if (buflen < SYSUTIL_MINIMAL_MAC_STR_LEN) {
    LOG_ERROR(
        "buffer to store mac address string has to be at least %d bytes big",
        SYSUTIL_MINIMAL_MAC_STR_LEN);
    return ERR_C_MEMORY_ERR;
  }

  sysutil_get_chip_base_mac(mac);
  snprintf(buffer, buflen, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1],
           mac[2], mac[3], mac[4], mac[5]);
  return err;
}

int sysutil_setup_ntp_server(const char *ntp_server, bool block) {
  err_c_t err = 0;

  ERR_C_CHECK_NULL_PTR(ntp_server, LOG_ERROR("ntp server name cannot be NULL"));
  esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG(ntp_server);

  setenv("TZ", "UTC-1", 1);
  tzset();
  ERR_C_CHECK_ERROR(esp_netif_sntp_init(&config),
                    LOG_ERROR("error %d, cannot init sntp server: %s", err,
                              error_to_name(err)));
  if (block) {
    while (esp_netif_sntp_sync_wait(pdMS_TO_TICKS(SYSUTIL_SNTP_TIMEOUT) ==
                                    ESP_ERR_TIMEOUT)) {
      LOG_VERBOSE("Waiting for system time to be set...");
      vTaskDelay(pdMS_TO_TICKS(100));
    }
  }

  return err;
}
