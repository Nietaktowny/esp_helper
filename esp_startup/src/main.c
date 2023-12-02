#include "cli_manager.h"
#include "nvs_controller.h"
#include "wifi_controller.h"
#include "tcp_driver.h"
#include "tcp_driver_errors.h"
#include "memory_utils.h"
#include "linked_list.h"
#include "arena.h"
#include "err_controller.h"
#include "logger.h"
#include "bmp280.h"
#include <string.h>
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
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

    //Allow other core to finish initialization
    vTaskDelay(pdMS_TO_TICKS(100));
    //Create semaphores to synchronize
    logger_create_semphr();
    // Initialize NVS
    nvs_c_init_nvs();
    wifi_c_init_wifi(WIFI_C_MODE_STA);
    err_c_t err = wifi_c_start_sta(MY_SSID, MY_PSK);
    if(err != 0) {
        LOG_FATAL("failed to connect to wifi, rebooting...");
        wifi_c_deinit();
        esp_restart();
    }
    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
    esp_netif_sntp_init(&config);
    LOG_INFO("Connected to wifi, current ip: %s", wifi_c_get_ipv4());
    cli_set_remote_logging();

    bmp_config_t bmp_config = {
        .gpio_cs = GPIO_NUM_14,
        .gpio_sclk = GPIO_NUM_32,
        .gpio_miso = GPIO_NUM_26,
        .gpio_mosi = GPIO_NUM_12,
        .host_id = 1,
    };
    bmp_handle_t bmp280 = NULL;
    bmp_add_device(&bmp_config, bmp280);
}