#include <stdio.h>
#include "logger.h"
#include "esp_helper_utils.h"
#include "wifi_controller.h"


#include "nvs_flash.h"
#include "nvs.h"
#include "esp_err.h"

#define MY_SSID "TP-LINK_AD8313"
#define MY_PSK "20232887"
#define SOL_SSID "OstNet-952235"
#define SOL_PSK "Solonka106"


void perform_ota_on_sta_connected(void) {
    helper_perform_ota();
}

void app_main(void)
{
    // Init logger library
    logger_create_semphr();
    LOG_INFO("TESTING ESP HELPER UTILS");

    // Initialize NVS    
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    // We need wifi to try to perform OTA
    wifi_c_init_wifi(WIFI_C_MODE_STA);

    wifi_c_sta_register_connect_handler(perform_ota_on_sta_connected);

    // uint8_t chip_id[ESP_HELPER_CHIP_ID_SIZE] = {0};
    // helper_get_chip_id(chip_id);
    wifi_c_start_sta(MY_SSID, MY_PSK);
}