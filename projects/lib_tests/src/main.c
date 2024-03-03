#ifdef ESP_PLATFORM
#include <driver/gpio.h>
#include <string.h>

#include "cJSON.h"
#include "cli_manager.h"
#include "err_controller.h"
#include "esp_netif_sntp.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "http_client.h"
#include "logger.h"
#include "memory_utils.h"
#include "nvs_controller.h"
#include "ota_controller.h"
#include "wifi_controller.h"
#include "wifi_manager.h"

#define MY_SSID "TP-LINK_AD8313"
#define MY_PSK "20232887"
#define SOL_SSID "OstNet-952235"
#define SOL_PSK "Solonka106"

void app_main() {
    // Allow other core to finish initialization
    vTaskDelay(pdMS_TO_TICKS(100));

    // Create semaphores to synchronize logs
    logger_init();
    logger_set_log_level(LOG_LEVEL_VERBOSE);

    // given
    const char* key = "asdwadq";
    uint8_t data = 12;
    nvs_c_handle_t handle = NULL;

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "test", NVS_C_READWRITE);
    nvs_c_write_uint8(handle, key, data);
    nvs_c_erase_key(handle, key);
}

#else
#include "err_controller.h"
#include "logger.h"

int main() {
    logger_init();
    LOG_DEBUG("Hello world!");
    int err = 2134;
    ERR_C_CHECK_ERROR(err, LOG_ERROR("%s", error_to_name(err)));
    return 0;
}
#endif
