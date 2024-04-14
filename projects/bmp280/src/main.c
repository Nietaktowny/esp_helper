#include "bmp280.h"
#include "err_controller.h"
#include "esp_helper_utils.h"
#include "freertos/projdefs.h"
#include "http_client.h"
#include "logger.h"
#include "memory_utils.h"
#include "nvs_controller.h"
#include "sys_utils.h"
#include "wifi_controller.h"
#include "wifi_manager.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <math.h>
#include <stdint.h>
#include <string.h>

#include "cli_manager.h"
#include <driver/gpio.h>

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

#define BMP_NUMBER_OF_VALUES_TO_READ 10 ///< Numebr of values to read, before calculating average value.
#define BMP_DELAY_BETWEEN_READ 6000     ///< Delay to wait before single read iteration.

void read_temperature_task(void *args) {
    err_c_t err = 0;
    uint8_t iteration = 0;
    bmp_handle_t bmp = NULL;
    i2c_c_bus_handle_t bus = (i2c_c_bus_handle_t)args;
    http_client_t client = NULL;
    float temperature[BMP_NUMBER_OF_VALUES_TO_READ + 1] = {0};
    float pressure[BMP_NUMBER_OF_VALUES_TO_READ + 1] = {0};
    float altitude[BMP_NUMBER_OF_VALUES_TO_READ + 1] = {0};

    double temperature_avg = 0.0;
    double pressure_avg = 0.0;
    double altitude_avg = 0.0;

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
    if (err != ERR_C_OK) {
        LOG_ERROR("BMP280 device not found");
        // vTaskDelete(NULL);
    }
    err = bmp_configure(bmp, &config);
    if (err != ERR_C_OK) {
        LOG_ERROR("cannot configure BMP280 device");
        //  vTaskDelete(NULL);
    }
    LOG_INFO("Identified BMP280 device:\n address: %x\n chip id: %x\n mode: %x", bmp_get_i2c_addr(bmp), bmp_get_chip_id(bmp),
             bmp_get_mode(bmp));

    err = http_client_init_reuse(&client, "wmytych.usermd.net", "modules/setters/insert_data.php");
    if (err != ERR_C_OK) {
        LOG_ERROR("error %d when trying to prepare http_client handle for reuse: %s", err, error_to_name(err));
        //    vTaskDelete(NULL);
    }

    while (1) {
        LOG_VERBOSE("reading BMP280 value number: %d", iteration);
        bmp_readoutFloat(bmp, &temperature[iteration], &pressure[iteration]);
        altitude[iteration] = bmp_i2c_calculate_altitude(pressure[iteration] * 0.01, BMP_STANDARD_SEA_LEVEL_PRESSURE);
        LOG_DEBUG("temperature: %.2f C", temperature[iteration]);
        LOG_DEBUG("pressure: %.2fhPa", 0.01 * pressure[iteration]);
        LOG_DEBUG("altitude: %.2f m. n. p. m.", altitude[iteration]);

        if (iteration < BMP_NUMBER_OF_VALUES_TO_READ) {
            LOG_VERBOSE("number of already collected BMP280 values: %d / %d", iteration, BMP_NUMBER_OF_VALUES_TO_READ);
            iteration++;
            vTaskDelay(pdMS_TO_TICKS(BMP_DELAY_BETWEEN_READ));
            continue;
        }
        LOG_VERBOSE("calculating average BMP280 values...");
        // calculate average from number of readed values
        for (uint8_t i = 0; i < BMP_NUMBER_OF_VALUES_TO_READ; i++) {
            LOG_VERBOSE("read pressure value number %d is: %.2f hPa", i, pressure[i]);
            pressure_avg += pressure[i];
            LOG_VERBOSE("read temperature value number %d is: %.2f C", i, temperature[i]);
            temperature_avg += temperature[i];
            LOG_VERBOSE("read altitude value number %d is: %.2f m. n. p. m.", i, altitude[i]);
            altitude_avg += altitude[i];
        }

        // calculate average values, reset counter and stored values
        pressure_avg =
            pressure_avg / BMP_NUMBER_OF_VALUES_TO_READ; // we start from 0, so add 1 to make it the same as BMP_NUMBER_OF_VALUES_TO_READ
        LOG_INFO("calculated average pressure value: %.2f", pressure_avg * 0.01);
        temperature_avg = temperature_avg / BMP_NUMBER_OF_VALUES_TO_READ;
        LOG_INFO("calculated average temperature value: %.2f", temperature_avg);
        altitude_avg = altitude_avg / BMP_NUMBER_OF_VALUES_TO_READ;
        LOG_INFO("calculated average altitude value: %.2f", altitude_avg);

        // reset all values
        iteration = 0;
        memutil_zero_memory(pressure, sizeof(pressure));
        memutil_zero_memory(temperature, sizeof(temperature));
        memutil_zero_memory(altitude, sizeof(altitude));

        sprintf(post_data, "device_id=%s&temperature=%.2f&pressure=%.2f&altitude=%.2f", device_id, temperature_avg, pressure_avg * 0.01,
                altitude_avg);
        LOG_VERBOSE("data to send to database: %s", post_data);
        err = http_client_post_reuse(client, post_data, HTTP_CLIENT_POST_USE_STRLEN);
        if (err != ERR_C_OK) {
            LOG_ERROR("Client POST request returned error %d: %s", err, error_to_name(err));
            LOG_WARN("Reestablishing connection...");
            http_client_deinit_reuse(&client);
            err = http_client_init_reuse(&client, "wmytych.usermd.net", "modules/setters/insert_data.php");
            if (err != ERR_C_OK) {
                LOG_ERROR("error %d when trying to reestablish connection: %s", err, error_to_name(err));
                vTaskDelay(pdMS_TO_TICKS(BMP_DELAY_BETWEEN_READ));
                continue;
            }
            err = http_client_post_reuse(client, post_data, HTTP_CLIENT_POST_USE_STRLEN);
            if (err != ERR_C_OK) {
                LOG_ERROR("Client POST request returned error %d: %s", err, error_to_name(err));
                vTaskDelay(pdMS_TO_TICKS(BMP_DELAY_BETWEEN_READ));
                continue;
            }
            LOG_VERBOSE("Client POST request returned: %d", err);
        }
        // after sending, reset calculated values
        pressure_avg = 0;
        temperature_avg = 0;
        altitude_avg = 0;
        vTaskDelay(pdMS_TO_TICKS(BMP_DELAY_BETWEEN_READ));
    }

    http_client_deinit_reuse(&client);
    // if ever task got here, delete it
    vTaskDelete(NULL);
}

void update_wifi_info_task(void *args) {
    err_c_t err = ERR_C_OK;
    char wifi_c_info[300] = {0};
    char device_info[350] = {0};
    char device_id[20] = {0};

    sysutil_get_chip_base_mac_as_str(device_id, sizeof(device_id));

    while (1) {

        err = wifi_c_get_status_as_json(wifi_c_info, 300);

        snprintf(device_info, 350, "device_id=%s&wifi_info=%s", device_id, wifi_c_info);
        err = http_client_post("wmytych.usermd.net", "modules/setters/update_wifi_info.php", device_info, HTTP_CLIENT_POST_USE_STRLEN);
        if (err != ERR_C_OK) {
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

void inspect_heap_task(void *args) {
    while (1) {
        uint32_t free_heap = esp_get_free_heap_size();
        uint32_t ever_free_heap = esp_get_minimum_free_heap_size();
        LOG_VERBOSE("Currently available heap: %lu", free_heap);
        LOG_VERBOSE("The minimum heap size that was ever available: %lu", ever_free_heap);

        if (free_heap < 8000) {
            LOG_ERROR("Currently free heap very low, restarting...");
            esp_restart();
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void on_connect_handler(void) {
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
void switch_off_all_leds(void) {
    // switch off all LEDs
    gpio_set_direction(GPIO_NUM_0, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_0, 0);
    gpio_set_direction(GPIO_NUM_4, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_4, 0);
    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_2, 0);
}
#endif

void app_main() {

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
