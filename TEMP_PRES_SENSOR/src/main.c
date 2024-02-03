// #include "cli_manager.h"
#include "wifi_controller.h"
#include "memory_utils.h"
#include "err_controller.h"
#include "logger.h"
#include "bmp280.h"
#include "http_client.h"
#include "wifi_manager.h"
#include "ota_controller.h"
#include "gpio_controller.h"
#include "cli_manager.h"
#include "nvs_controller.h"

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_netif_sntp.h"
#include "esp_task_wdt.h"

#define MY_SSID "TP-LINK_AD8313"
#define MY_PSK "20232887"
#define SOL_SSID "OstNet-952235"
#define SOL_PSK "Solonka106"

#define ESP_WROVER_KIT_DEVICE_ID 333333

void read_temperature_task(void *args)
{
    err_c_t err = 0;
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

    err = bmp_init(&bmp, bus);
    if (err != ERR_C_OK)
    {
        LOG_ERROR("BMP280 device not found");
        // vTaskDelete(NULL);
    }
    err = bmp_configure(bmp, &config);
    if (err != ERR_C_OK)
    {
        LOG_ERROR("cannot configure BMP280 device");
        // vTaskDelete(NULL);
    }
    LOG_INFO("Identified BMP280 device:\n address: %x\n chip id: %x\n mode: %x", bmp_get_i2c_addr(bmp), bmp_get_chip_id(bmp), bmp_get_mode(bmp));
    while (1)
    {
        bmp_readoutFloat(bmp, &temperature, &pressure);
        altitude = bmp_i2c_calculate_altitude(pressure * 0.01, BMP_STANDARD_SEA_LEVEL_PRESSURE);
        LOG_INFO("temperature: %.2f C", temperature);
        LOG_INFO("pressure: %.2fhPa", pressure * 0.01);
        LOG_INFO("altitude: %.2f m. n. p. m.", altitude);
        sprintf(post_data, "device_id=%d&temperature=%.2f&pressure=%.2f&altitude=%.2f", ESP_WROVER_KIT_DEVICE_ID, temperature, pressure * 0.01, altitude);
        LOG_DEBUG("data to send to database: %s", post_data);
        err = http_client_post("wmytych.usermd.net", "modules/setters/insert_data.php", post_data);
        LOG_INFO("Client POST request returned: %d", err);
        vTaskDelay(pdMS_TO_TICKS(30000));
    }
}

void switch_led_task(void *args)
{
    gpio_c_set_direction(GPIO_C_NUM_2, GPIO_C_MODE_OUTPUT);
    while (1)
    {
        gpio_c_set_level(GPIO_C_NUM_2, 1);
        LOG_INFO("CHANGED GPIO_2 LEVEL");
        vTaskDelay(pdMS_TO_TICKS(4000));
        gpio_c_set_level(GPIO_C_NUM_2, 0);
    }
}

void inspect_task(void *args)
{
    while (1)
    {
        uint32_t free_heap = esp_get_free_heap_size();
        uint32_t ever_free_heap = esp_get_minimum_free_heap_size();

        LOG_DEBUG("Currently available heap: %lu", free_heap);
        LOG_DEBUG("The minimum heap size that was ever available: %lu", ever_free_heap);
        vTaskDelay(pdMS_TO_TICKS(5000));
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

    i2c_c_bus_handle_t i2c_bus = NULL;
    i2c_c_init_bus(I2C_C_NUM_0, 22, 21, &i2c_bus);

    wifi_manager_init();

    ota_c_do_simple_ota("http://wmytych.usermd.net/ota.php");
    ota_c_start("http://wmytych.usermd.net/modules/getters/ota.php?device_id=333333");

    xTaskCreate(read_temperature_task, "log_task", 8096, (void *)i2c_bus, 1, NULL);

    xTaskCreate(inspect_task, "inspect_heap_task", 8096, NULL, 2, NULL);

    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
    esp_netif_sntp_init(&config);

    cli_set_remote_logging(27015);
}