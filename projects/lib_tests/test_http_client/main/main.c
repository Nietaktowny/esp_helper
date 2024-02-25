#include <stdio.h>

#include "nvs_flash.h"
#include "esp_err.h"

#include "logger.h"
#include "wifi_controller.h"
#include "http_client.h"
#include "memory_utils.h"


#define MY_SSID "TP-LINK_AD8313"
#define MY_PSK "20232887"
#define SOL_SSID "OstNet-952235"
#define SOL_PSK "Solonka106"
#define MI_SSID "Mi"
#define MI_PSK  "Solonka106"

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
    
    //Init Wifi
    ESP_ERROR_CHECK(wifi_c_init_wifi(WIFI_C_MODE_STA));

    //Start STA and connect to AP:
    ESP_ERROR_CHECK(wifi_c_start_sta(MY_SSID, MY_PSK));

    //given
    int err = -1;
    const char* address = "wmytych.usermd.net";
    const char* path = "modules/getters/get_gpio_state.php?device_id=222222";
    char* resp = malloc(1025);
    const char* const expected_resp = "[{\"gpio\":22,\"state\":1,\"gpio_alias\":\"Relay\"},{\"gpio\":2,\"state\":1,\"gpio_alias\":\"Onboard LED\"}]";

    //when
    err = http_client_get(address, path, resp, 1024);
}