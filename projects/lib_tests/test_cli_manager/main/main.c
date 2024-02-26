#include <stdio.h>

#include "nvs_flash.h"
#include "esp_err.h"

#include "logger.h"
#include "wifi_controller.h"
#include "cli_manager.h"
#include "esp_helper_utils.h"
#include "sys_utils.h"

#define MY_SSID "TP-LINK_AD8313"
#define MY_PSK "20232887"
#define SOL_SSID "OstNet-952235"
#define SOL_PSK "Solonka106"

void app_main(void)
{
    // Init logger library
    logger_create_semphr();
    
    // Initialize NVS    
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );
    
    //Init Wifi
    ESP_ERROR_CHECK(wifi_c_init_wifi(WIFI_C_MODE_STA));

    //Start STA and connect to AP:
    ESP_ERROR_CHECK(wifi_c_start_sta(MY_SSID, MY_PSK));

    helper_perform_ota();

    //start CLI manager
    cli_set_remote_logging(27015, wifi_c_get_sta_ipv4());
}