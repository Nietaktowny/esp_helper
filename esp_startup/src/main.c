#include "cli_manager.h"
#include "nvs_controller.h"
#include "wifi_controller.h"
#include "memory_utils.h"
#include "linked_list.h"
#include "arena.h"
#include "err_controller.h"
#include "logger.h"
#include "bmp280.h"
#include "http_client.h"
#include "spiffs_controller.h"

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

void read_temperature_task(void* args) {
    bmp_handle_t bmp = NULL;
    i2c_c_bus_handle_t bus = (i2c_c_bus_handle_t)args;
    float temperature = 0;
    float pressure = 0;
    float altitude = 0;
    bmp_config_t config = {
        .iir_filter = BMP_IIR_X16,
        .press_over = BMP_OVERSAMPLING_X4,
        .temp_over = BMP_OVERSAMPLING_X4,
        .standby = BMP_STANDBY_1000M,
        .mode = BMP_MODE_NORMAL,
    };
    char post_data[256];
    memutil_zero_memory(&post_data, sizeof(post_data));

    bmp_init(&bmp, bus);
    bmp_configure(bmp, &config);
    LOG_INFO("Identified BMP280 device:\n address: %x\n chip id: %x\n mode: %x", bmp_get_i2c_addr(bmp), bmp_get_chip_id(bmp), bmp_get_mode(bmp));
    while (1)
    {   
        bmp_readoutFloat(bmp, &temperature, &pressure);
        altitude = bmp_i2c_calculate_altitude(pressure * 0.01, BMP_STANDARD_SEA_LEVEL_PRESSURE);
        LOG_INFO("temperature: %.2f C", temperature);
        LOG_INFO("pressure: %.2fhPa", pressure * 0.01);
        LOG_INFO("altitude: %.2f m. n. p. m.", altitude);
        sprintf(post_data, "device_id=1&temperature=%.2f&pressure=%.2f&altitude=%.2f", temperature, pressure * 0.01, altitude);
        LOG_DEBUG("data to send to database: %s", post_data);
        http_client_post("wmytych.usermd.net/", "insert_data.php", post_data);
        vTaskDelay(pdMS_TO_TICKS(30000));
    }
}

void inspect_task(void* args) {
    while (1)
    {
        uint32_t free_heap = esp_get_free_heap_size();
        uint32_t ever_free_heap = esp_get_minimum_free_heap_size();

        LOG_DEBUG("Currently available heap: %lu", free_heap);
        LOG_DEBUG("The minimum heap size that was ever available: %lu", ever_free_heap);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void shutdown_handler(void) {
    LOG_FATAL("FATAL ERROR - RESTARTING ESP DEVICE!");
}

void app_main(void)
{

    //Allow other core to finish initialization
    vTaskDelay(pdMS_TO_TICKS(100));
    esp_register_shutdown_handler(shutdown_handler);
    //Create semaphores to synchronize
    logger_create_semphr();
    // Initialize NVS
    nvs_c_init_nvs();
    // Initialize WiFi
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
    /*
    i2c_c_bus_handle_t i2c_bus = NULL;
    i2c_c_init_bus(I2C_C_NUM_0, GPIO_NUM_22, GPIO_NUM_21, &i2c_bus);

    xTaskCreate(read_temperature_task, "log_task", 7000, (void*)i2c_bus, 1, NULL);
    */
    xTaskCreate(inspect_task, "inspect_heap_task", 2096, NULL, 2, NULL);
    spiffs_c_handle_t spiffs_config = {
        .base_name = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_on_err = true,
    };
    size_t total = 0;
    size_t used = 0;
    
    spiffs_c_init(&spiffs_config);
    spiffs_c_get_info(spiffs_config.partition_label, &total, &used);
    FILE* stylesheet = NULL;
    char line[256];
    spiffs_c_open_file(&spiffs_config, "index.html", "r+", &stylesheet);
    while(fgets(line, sizeof(line), stylesheet) != NULL)
    {
      printf(line);
    }
    spiffs_c_close_file(stylesheet);
    
    cli_set_remote_logging(27015);
}