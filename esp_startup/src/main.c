#include "cli_manager.h"
#include "nvs_controller.h"
#include "wifi_controller.h"
#include "memory_utils.h"
#include "linked_list.h"
#include "arena.h"
#include "err_controller.h"
#include "logger.h"
#include <string.h>
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/task.h"
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "linked_list.h"
#include <sys/time.h>
#include "esp_netif_sntp.h"

#define MY_SSID "TP-LINK_AD8313"
#define MY_PSK "20232887"
#define SOL_SSID "OstNet-952235"
#define SOL_PSK "Solonka106"


void app_main(void)
{
    // Initialize NVS
    nvs_c_init_nvs();
    wifi_c_init_wifi(WIFI_C_MODE_STA);
    err_c_t err = wifi_c_start_sta(SOL_SSID, SOL_PSK);
    if(err != 0) {
        LOG_FATAL("failed to connect to wifi, rebooting...");
        wifi_c_deinit();
        esp_restart();
    }
    LOG_INFO("Connected to wifi, current ip: %s", wifi_c_get_ipv4());
    cli_set_remote_logging(27015);
    /*delay for dhcp to configure IP adress*/
    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
    esp_netif_sntp_init(&config);
    while(1) {
        wifi_c_scan_result_t records;
        memutil_zero_memory(&records, sizeof(records));
        wifi_c_scan_all_ap(&records);
        wifi_c_print_scanned_ap();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}