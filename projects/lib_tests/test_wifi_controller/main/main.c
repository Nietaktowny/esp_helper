#include <stdio.h>

#include "nvs_flash.h"
#include "esp_err.h"

#include "logger.h"
#include "wifi_controller.h"
#include "wifi_manager.h"
#include "ota_controller.h"
#include "linked_list.h"


#define MY_SSID "TP-LINK_AD8313"
#define MY_PSK "20232887"
#define SOL_SSID "OstNet-952235"
#define SOL_PSK "Solonka106"


void connect_handler(void) {
    //Things to do when STA connects to some Access Point.
}

void disconnect_handler(void) {
    //Things to do when STA loses connection with Access Point.
}

void app_main(void)
{
    // Init logger library
    logger_init();
    logger_set_log_level(LOG_LEVEL_VERBOSE);
    
    // Initialize NVS    
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );
    
    /*STA example*/

    //Init Wifi
    ESP_ERROR_CHECK(wifi_c_init_wifi(WIFI_C_MODE_APSTA));

    //Register connect handler (optional)
    ESP_ERROR_CHECK(wifi_c_sta_register_connect_handler(connect_handler));

    //Register connect handler (optional)
    ESP_ERROR_CHECK(wifi_c_sta_register_disconnect_handler(disconnect_handler));

    //Start STA and connect to AP:
    wifi_c_start_sta(MY_SSID, MY_PSK);

    //Start AP
    wifi_c_start_ap("TEST_AP", "TEST_AP_PASSWORD");
}