#include "wifi_controller.h"
#include "memory_utils.h"
#include "err_controller.h"
#include "logger.h"
#include "bmp280.h"
#include "http_client.h"
#include "wifi_manager.h"
#include "ota_controller.h"
#include "nvs_controller.h"

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#ifndef ESP32_C3_SUPERMINI
#include "cli_manager.h"

//#include "esp_netif_sntp.h"
#include <driver/gpio.h>
#endif

#define MY_SSID "TP-LINK_AD8313"
#define MY_PSK "20232887"
#define SOL_SSID "OstNet-952235"
#define SOL_PSK "Solonka106"

#ifdef ESP_WROVER_KIT
#define ESP_DEVICE_ID 111111
#define ESP_DEVICE_WIFI_LED GPIO_NUM_2
#define BUS_GPIO_SCL 22
#define BUS_GPIO_SDA 21
#elif ESP_DEV_MODULE
#define ESP_DEVICE_ID 222222
#define ESP_DEVICE_WIFI_LED GPIO_NUM_2
#define BUS_GPIO_SCL 22
#define BUS_GPIO_SDA 21
#elif ESP32_C3_SUPERMINI
#define ESP_DEVICE_ID 333333
#define BUS_GPIO_SCL 9
#define BUS_GPIO_SDA 8
#endif

void scan_task(void* args) {
    wifi_c_scan_result_t* scan = NULL;

    while (1)
    {
        wifi_c_scan_all_ap(&scan);
        wifi_c_print_scanned_ap();
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
    
}

void app_main(void)
{

    // Allow other core to finish initialization
    vTaskDelay(pdMS_TO_TICKS(100));

    // Create semaphores to synchronize logs
    logger_create_semphr();

    // Initialize NVS
    nvs_c_init_nvs();

    wifi_manager_init();

    //xTaskCreate(scan_task, "scan_task", 1024*9, NULL, 2, NULL);
}