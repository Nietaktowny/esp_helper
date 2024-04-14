#include "cli_manager.h"
#include "err_controller.h"
#include "esp_helper_utils.h"
#include "esp_netif_sntp.h"
#include "http_client.h"
#include "logger.h"
#include "memory_utils.h"
#include "nvs_controller.h"
#include "ota_controller.h"
#include "sys_utils.h"
#include "wifi_controller.h"
#include "wifi_manager.h"

#include "cJSON.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <driver/gpio.h>
#include <string.h>

#ifdef ESP_WROVER_KIT
#define ESP_DEVICE_WIFI_LED GPIO_NUM_2
#elif ESP_DEV_MODULE
#define ESP_DEVICE_WIFI_LED GPIO_NUM_2
#elif ESP32_C3_SUPERMINI
#endif

#define SERVER_URL "wmytych.usermd.net"
#define PHP_GET_GPIO_STATES_URL "modules/getters/get_gpio_state.php"

void switch_gpio_task(void *args) {
    err_c_t err = ERR_C_OK;
    char response[500] = {0};
    char url[120] = {0};
    char device_id[20] = {0};
    sysutil_get_chip_base_mac_as_str(device_id, sizeof(device_id));
    LOG_DEBUG("getting gpio state for device with ID: %s", device_id);
    snprintf(url, sizeof(url), "%s?device_id=%s", PHP_GET_GPIO_STATES_URL, device_id);

    http_client_t client = NULL;
    err = http_client_init_reuse(&client, SERVER_URL, url);
    if (err != ERR_C_OK) {
        LOG_ERROR("error %d when trying to prepare http_client handle for reuse: %s", err, error_to_name(err));
    }

    cJSON *array = NULL;
    const cJSON *element = NULL;
    while (1) {
        err = http_client_get_reuse(client, response, sizeof(response));
        if (err != ERR_C_OK) {
            LOG_ERROR("Client GET request returned error %d: %s", err, error_to_name(err));
            LOG_WARN("Reestablishing connection...");
            http_client_deinit_reuse(&client);
            err = http_client_init_reuse(&client, SERVER_URL, url);
            if (err != ERR_C_OK) {
                LOG_ERROR("error %d when trying to reestablish connection: %s", err, error_to_name(err));
                vTaskDelay(pdMS_TO_TICKS(2000));
                continue;
            }
        }

        LOG_DEBUG("HTTP GET RESPONSE: %s", response);

        // check if response json is empty
        if (strncmp(response, "{[]}", 5) == 0) {
            LOG_DEBUG("No GPIOs are configured yet");
            vTaskDelay(pdMS_TO_TICKS(2000));
            continue;
        }

        array = cJSON_Parse(response);
        if (!array) {
            const char *error_ptr = cJSON_GetErrorPtr();
            if (error_ptr) {
                LOG_ERROR("cJSON parsing error: %s", error_ptr);
            }
            cJSON_Delete(array);
        }

        cJSON_ArrayForEach(element, array) {
            cJSON *gpio = cJSON_GetObjectItemCaseSensitive(element, "gpio");
            cJSON *state = cJSON_GetObjectItemCaseSensitive(element, "state");

            if (!cJSON_IsNumber(gpio) || !cJSON_IsNumber(state)) {
                LOG_ERROR("cannot access gpio and state JSON members");
                continue;
            }

            LOG_INFO("switching gpio: %d to state: %d", gpio->valueint, state->valueint);
            gpio_set_direction(gpio->valueint, GPIO_MODE_OUTPUT);
            gpio_set_level(gpio->valueint, state->valueint);
        }

        cJSON_Delete(array);
        vTaskDelay(pdMS_TO_TICKS(2000));
    };

    http_client_deinit_reuse(&client);
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
        LOG_DEBUG("Currently available heap: %lu", free_heap);
        LOG_DEBUG("The minimum heap size that was ever available: %lu", ever_free_heap);

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
    // Set Timezone
    sysutil_setup_ntp_server("pool.ntp.org", 1);

    helper_perform_ota();
    xTaskCreate(inspect_heap_task, "inspect_task", 1024 * 2, NULL, 4, NULL);
    xTaskCreate(switch_gpio_task, "gpio_task", 1024 * 6, NULL, 2, NULL);
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
