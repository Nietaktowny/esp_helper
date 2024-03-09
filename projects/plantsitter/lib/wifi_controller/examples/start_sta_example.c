#include "nvs.h"
#include "esp_err.h"
#include "wifi_controller.h"

void connect_handler(void) {
    //Things to do when STA connects to some Access Point.
}

void disconnect_handler(void) {
    //Things to do when STA loses connection with Access Point.
}

void app_main(void)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    /*STA example*/

    //Init Wifi
    ESP_ERROR_CHECK(wifi_c_init_wifi(WIFI_C_MODE_STA));

    //Register connect handler (optional)
    ESP_ERROR_CHECK(wifi_c_sta_register_connect_handler(connect_handler));

    //Register connect handler (optional)
    ESP_ERROR_CHECK(wifi_c_sta_register_disconnect_handler(disconnect_handler));

    //Start STA and connect to AP:
    ESP_ERROR_CHECK(wifi_c_start_sta("SSID", "PASSWORD"));

}