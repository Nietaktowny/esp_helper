#include "cli_manager.h"
#include "nvs_controller.h"
#include "wifi_controller.h"
#include "tcp_driver.h"
#include "tcp_driver_errors.h"
#include "memory_utils.h"
#include "linked_list.h"
#include "arena.h"
#include "errors.h"
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

const char* TAG = "";

#define MY_SSID "TP-LINK_AD8313"
#define MY_PSK "20232887"
#define SOL_SSID "OstNet-952235"
#define SOL_PSK "Solonka106"


void app_main(void)
{
    // Initialize NVS
    nvs_c_init_nvs();
    wifi_c_init_wifi(WIFI_C_MODE_STA);
    wifi_c_start_sta(SOL_SSID, SOL_PSK);
    set_logging_to_socket("192.168.100.115", 27015);
    logger_set_log_level(LOG_LEVEL_DEBUG);
    linkedl_t list = linkedl_create(sizeof(int), "int lists");
    int data = 12;
    linkedl_add_first(list, &data, "first");
    linkedl_add_last(list, &data, "last");
    linkedl_delete(list, "last");
    linkedl_destroy(list);
    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
    esp_netif_sntp_init(&config);
    while(1) {
        LOG_DEBUG("LOGGED DEBUG");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}