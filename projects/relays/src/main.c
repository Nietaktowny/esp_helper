#include "wifi_controller.h"
#include "memory_utils.h"
#include "err_controller.h"
#include "logger.h"
#include "http_client.h"
#include "wifi_manager.h"
#include "ota_controller.h"
#include "nvs_controller.h"
#include "esp_helper_utils.h"
#include "sys_utils.h"

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "cJSON.h"
#include <driver/gpio.h>

#ifndef ESP32_C3_SUPERMINI
#include "cli_manager.h"
#include "esp_netif_sntp.h"
#endif

#define MY_SSID "TP-LINK_AD8313"
#define MY_PSK "20232887"
#define SOL_SSID "OstNet-952235"
#define SOL_PSK "Solonka106"

#ifdef ESP_WROVER_KIT
#define ESP_DEVICE_WIFI_LED GPIO_NUM_2
#elif ESP_DEV_MODULE
#define ESP_DEVICE_WIFI_LED GPIO_NUM_2
#elif ESP32_C3_SUPERMINI
#endif

#define SERVER_URL "wmytych.usermd.net"
#define PHP_GET_GPIO_STATES_URL "modules/getters/get_gpio_state.php"

void switch_gpio_task(void *args)
{
    char response[500] = {0};
    char url[120] = {0};
    char device_id[20] = {0};
    sysutil_get_chip_base_mac_as_str(device_id, sizeof(device_id));
    LOG_DEBUG("getting gpio state for device with ID: %s", device_id);
    snprintf(url, sizeof(url), "%s?device_id=%s", PHP_GET_GPIO_STATES_URL, device_id);

    cJSON *array = NULL;
    const cJSON *element = NULL;
    while (1)
    {
        http_client_get(
            SERVER_URL,
            url,
            response,
            sizeof(response));

        LOG_DEBUG("HTTP GET RESPONSE: %s", response);

        array = cJSON_Parse(response);
        if (!array)
        {
            const char *error_ptr = cJSON_GetErrorPtr();
            if (error_ptr)
            {
                LOG_ERROR("cJSON parsing error: %s", error_ptr);
            }
            cJSON_Delete(array);
        }

        cJSON_ArrayForEach(element, array)
        {
            cJSON *gpio = cJSON_GetObjectItemCaseSensitive(element, "gpio");
            cJSON *state = cJSON_GetObjectItemCaseSensitive(element, "state");

            if (!cJSON_IsNumber(gpio) || !cJSON_IsNumber(state))
            {
                LOG_ERROR("cannot access gpio and state JSON members");
                continue;
            }

            LOG_INFO("switching gpio: %d to state: %d", gpio->valueint, state->valueint);
            gpio_set_direction(gpio->valueint, GPIO_MODE_OUTPUT);
            gpio_set_level(gpio->valueint, state->valueint);
        }

        vTaskDelay(pdMS_TO_TICKS(2000));
    };
}

void inspect_task(void *args)
{
    char *wifi_c_info = NULL;
    NEW_SIZE(wifi_c_info, 300); /** @todo add constant to wifi_controller, something like WIFI_C_STATUS_JSON_MIN_BUFLEN */

    char *device_info = NULL;
    NEW_SIZE(device_info, 350);

    char device_id[20] = {0};
    sysutil_get_chip_base_mac_as_str(device_id, sizeof(device_id));
    while (1)
    {
        uint32_t free_heap = esp_get_free_heap_size();
        uint32_t ever_free_heap = esp_get_minimum_free_heap_size();
        wifi_c_get_status_as_json(wifi_c_info, 300);
        snprintf(device_info, 350, "device_id=%s&wifi_info=%s", device_id, wifi_c_info);
        http_client_post("wmytych.usermd.net", "modules/setters/update_wifi_info.php", device_info, HTTP_CLIENT_POST_USE_STRLEN);
        LOG_DEBUG("sent wifi_c_status to the server");
        LOG_DEBUG("Currently available heap: %lu", free_heap);
        LOG_DEBUG("The minimum heap size that was ever available: %lu", ever_free_heap);
        vTaskDelay(pdMS_TO_TICKS(60000 * 3)); /** @todo Make it a constant or configuration variable? */ 
    }
    DELETE(wifi_c_info);
    DELETE(device_info);
}

void on_connect_handler(void)
{
#ifndef ESP32_C3_SUPERMINI
    gpio_set_direction(ESP_DEVICE_WIFI_LED, GPIO_MODE_OUTPUT);
    gpio_set_level(ESP_DEVICE_WIFI_LED, 1);
    LOG_INFO("Onboard LED turned on!");
    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
    esp_netif_sntp_init(&config);
#endif

    helper_perform_ota();
    xTaskCreate(switch_gpio_task, "gpio_task", 1024 * 6, NULL, 2, NULL);
    xTaskCreate(inspect_task, "inspect_heap_task", 1024 * 6, NULL, 3, NULL);
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
#ifndef ESP32_C3_SUPERMINI
    cli_set_remote_logging(27015, wifi_c_get_sta_ipv4());
#endif
}