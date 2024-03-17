#include "wifi_controller.h"
#include "memory_utils.h"
#include "err_controller.h"
#include "logger.h"
#include "bmp280.h"
#include "http_client.h"
#include "wifi_manager.h"
#include "esp_helper_utils.h"
#include "sys_utils.h"
#include "nvs_controller.h"

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "cli_manager.h"
#include <driver/gpio.h>

#define MY_SSID "TP-LINK_AD8313"
#define MY_PSK "20232887"
#define SOL_SSID "OstNet-952235"
#define SOL_PSK "Solonka106"

#ifdef ESP_WROVER_KIT
#define ESP_DEVICE_WIFI_LED GPIO_NUM_2
#define BUS_GPIO_SCL 22
#define BUS_GPIO_SDA 21
#elif ESP_DEV_MODULE
#define ESP_DEVICE_WIFI_LED GPIO_NUM_2
#define BUS_GPIO_SCL 22
#define BUS_GPIO_SDA 21
#elif ESP32_C3_SUPERMINI
#define BUS_GPIO_SCL 9
#define BUS_GPIO_SDA 8
#endif

void read_temperature_task(void *args)
{
    err_c_t err = 0;
    bmp_handle_t bmp = NULL;
    i2c_c_bus_handle_t bus = (i2c_c_bus_handle_t)args;
    http_client_t client = NULL;
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
    char post_data[256] = {0};
    char device_id[20] = {0};
    sysutil_get_chip_base_mac_as_str(device_id, sizeof(device_id));

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
        //  vTaskDelete(NULL);
    }
    LOG_INFO("Identified BMP280 device:\n address: %x\n chip id: %x\n mode: %x", bmp_get_i2c_addr(bmp), bmp_get_chip_id(bmp), bmp_get_mode(bmp));

    err = http_client_init_reuse(&client, "wmytych.usermd.net", "modules/setters/insert_data.php");
    if (err != ERR_C_OK)
    {
        LOG_ERROR("error %d when trying to prepare http_client handle for reuse: %s", err, error_to_name(err));
        //    vTaskDelete(NULL);
    }

    while (1)
    {
        bmp_readoutFloat(bmp, &temperature, &pressure);
        altitude = bmp_i2c_calculate_altitude(pressure * 0.01, BMP_STANDARD_SEA_LEVEL_PRESSURE);
        LOG_INFO("temperature: %.2f C", temperature);
        LOG_INFO("pressure: %.2fhPa", pressure * 0.01);
        LOG_INFO("altitude: %.2f m. n. p. m.", altitude);
        sprintf(post_data, "device_id=%s&temperature=%.2f&pressure=%.2f&altitude=%.2f", device_id, temperature, pressure * 0.01, altitude);
        LOG_VERBOSE("data to send to database: %s", post_data);
        err = http_client_post_reuse(client, post_data, HTTP_CLIENT_POST_USE_STRLEN);
        if (err != ERR_C_OK)
        {
            LOG_ERROR("Client POST request returned error %d: %s", err, error_to_name(err));
            LOG_WARN("Reestablishing connection...");
            http_client_deinit_reuse(&client);
            err = http_client_init_reuse(&client, "wmytych.usermd.net", "modules/setters/insert_data.php");
            if (err != ERR_C_OK)
            {
                LOG_ERROR("error %d when trying to reestablish connection: %s", err, error_to_name(err));
                vTaskDelay(pdMS_TO_TICKS(30000));
                continue;
            }
        }
        LOG_VERBOSE("Client POST request returned: %d", err);
        vTaskDelay(pdMS_TO_TICKS(30000));
    }

    http_client_deinit_reuse(&client);
    // if ever task got here, delete it
    vTaskDelete(NULL);
}

void update_wifi_info_task(void *args)
{
    err_c_t err = ERR_C_OK;
    char wifi_c_info[300] = {0};
    char device_info[350] = {0};
    char device_id[20] = {0};

    sysutil_get_chip_base_mac_as_str(device_id, sizeof(device_id));

    while (1)
    {

        err = wifi_c_get_status_as_json(wifi_c_info, 300);

        snprintf(device_info, 350, "device_id=%s&wifi_info=%s", device_id, wifi_c_info);
        err = http_client_post("wmytych.usermd.net", "modules/setters/update_wifi_info.php", device_info, HTTP_CLIENT_POST_USE_STRLEN);
        if (err != ERR_C_OK)
        {
            LOG_ERROR("error %d when posting device info data: %s", error_to_name(err));
            memutil_zero_memory(wifi_c_info, sizeof(wifi_c_info));
            memutil_zero_memory(device_info, sizeof(device_info));
            vTaskDelay(pdMS_TO_TICKS(60000 * 3));
            continue;
        }

        LOG_VERBOSE("sent wifi_c_status to the server");
        vTaskDelay(pdMS_TO_TICKS(60000 * 3)); /** @todo Make it a constant or configuration variable? */
    }
}

void inspect_heap_task(void *args)
{
    while (1)
    {
        uint32_t free_heap = esp_get_free_heap_size();
        uint32_t ever_free_heap = esp_get_minimum_free_heap_size();
        LOG_DEBUG("Currently available heap: %lu", free_heap);
        LOG_DEBUG("The minimum heap size that was ever available: %lu", ever_free_heap);

        if (free_heap < 8000)
        {
            LOG_ERROR("Currently free heap very low, restarting...");
            esp_restart();
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void on_connect_handler(void)
{
#ifndef ESP32_C3_SUPERMINI
    gpio_set_direction(ESP_DEVICE_WIFI_LED, GPIO_MODE_OUTPUT);
    gpio_set_level(ESP_DEVICE_WIFI_LED, 1);
    LOG_INFO("Onboard LED turned on!");
#endif

    helper_perform_ota();

    sysutil_setup_ntp_server("pool.ntp.org", 1);

    i2c_c_bus_handle_t i2c_bus = NULL;
    i2c_c_init_bus(I2C_C_NUM_0, BUS_GPIO_SCL, BUS_GPIO_SDA, &i2c_bus);
    xTaskCreate(inspect_heap_task, "inspect_task", 1024 * 2, NULL, 4, NULL);
    xTaskCreate(read_temperature_task, "log_task", 8096, (void *)i2c_bus, 1, NULL);
    xTaskCreate(update_wifi_info_task, "inspect_heap_task", 1024 * 6, NULL, 3, NULL);
}

#ifdef ESP_WROVER_KIT
void switch_off_all_leds(void)
{
    // switch off all LEDs
    gpio_set_direction(GPIO_NUM_0, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_0, 0);
    gpio_set_direction(GPIO_NUM_4, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_4, 0);
    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_2, 0);
}
#endif

void app_main()
{

    // Allow other core to finish initialization
    vTaskDelay(pdMS_TO_TICKS(100));

    // Create semaphores to synchronize logs
    logger_init();
    logger_set_log_level(LOG_LEVEL_VERBOSE);

    // Initialize NVS
    nvs_c_init_default_partition();

    char device_id[20] = {0};
    sysutil_get_chip_base_mac_as_str(device_id, sizeof(device_id));
    LOG_DEBUG("this device ID is: %s", device_id);

// ESP-WROVE-KIT has 3 LEDs, turn all off
#ifdef ESP_WROVER_KIT
    switch_off_all_leds();
#endif

    wifi_c_sta_register_connect_handler(on_connect_handler);

    wifi_manager_init();

    cli_set_remote_logging(27015, wifi_c_get_sta_ipv4());
}
