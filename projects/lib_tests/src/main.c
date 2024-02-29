#ifdef ESP_PLATFORM
#include "wifi_controller.h"
#include "memory_utils.h"
#include "err_controller.h"
#include "logger.h"
#include "http_client.h"
#include "wifi_manager.h"
#include "ota_controller.h"
#include "nvs_controller.h"

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
#define ESP_DEVICE_ID 111111
#define ESP_DEVICE_WIFI_LED GPIO_NUM_2
#elif ESP_DEV_MODULE
#define ESP_DEVICE_ID 222222
#define ESP_DEVICE_WIFI_LED GPIO_NUM_2
#elif ESP32_C3_SUPERMINI
#define ESP_DEVICE_ID 333333
#endif

#define SERVER_URL "wmytych.usermd.net"
#define PHP_GET_GPIO_STATES_URL "modules/getters/get_gpio_state.php"

void switch_gpio_task(void *args)
{
    err_c_t err = 0;
    char response[500] = {0};
    char url[100] = {0};
    snprintf(url, sizeof(url), "%s?device_id=%d", PHP_GET_GPIO_STATES_URL, ESP_DEVICE_ID);

    cJSON *array = NULL;
    const cJSON *element = NULL;
    while (1)
    {
        err = http_client_get(
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
    NEW_SIZE(wifi_c_info, 300); // TODO add constant to wifi_controller, something like WIFI_C_STATUS_JSON_MIN_BUFLEN

    char *device_info = NULL;
    NEW_SIZE(device_info, 350);
    while (1)
    {
        uint32_t free_heap = esp_get_free_heap_size();
        uint32_t ever_free_heap = esp_get_minimum_free_heap_size();
        wifi_c_get_status_as_json(wifi_c_info, 300);
        snprintf(device_info, 350, "device_id=%d&wifi_info=%s", ESP_DEVICE_ID, wifi_c_info);
        http_client_post("wmytych.usermd.net", "modules/setters/update_wifi_info.php", device_info);
        LOG_DEBUG("sent wifi_c_status to the server");
        LOG_DEBUG("Currently available heap: %lu", free_heap);
        LOG_DEBUG("The minimum heap size that was ever available: %lu", ever_free_heap);
        vTaskDelay(pdMS_TO_TICKS(60000 * 3)); // TODO Make it a constant or configuration variable?
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
#endif
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
    logger_create_semphr();

    // Initialize NVS
    nvs_c_init_nvs();

// ESP-WROVE-KIT has 3 LEDs, turn all off
#ifdef ESP_WROVER_KIT
    switch_off_all_leds();
#endif

    wifi_c_sta_register_connect_handler(on_connect_handler);

    wifi_manager_init();

    char url[100];
    memutil_zero_memory(&url, sizeof(url));
    ota_c_prepare_url_with_device_id("http://wmytych.usermd.net/modules/getters/ota.php", ESP_DEVICE_ID, &url[0], sizeof(url));
    ota_c_start(&url[0]);

#ifndef ESP32_C3_SUPERMINI
    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
    esp_netif_sntp_init(&config);
    cli_set_remote_logging(27015);
#endif
}

#else
#include "logger.h"
#include "err_controller.h"

int main() {
    logger_init();
    LOG_DEBUG("Hello world!");
    int err = 2134;
    ERR_C_CHECK_ERROR(err, LOG_ERROR("%s", error_to_name(err)));
    return 0;
}
#endif