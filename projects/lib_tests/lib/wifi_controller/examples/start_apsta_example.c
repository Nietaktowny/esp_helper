#include "nvs_flash.h"
#include "esp_err.h"
#include "wifi_controller.h"

void ap_connect_handler(void) {
    //Things to do each time new STA connects to our Access Point.
}

void ap_disconnect_handler(void) {
    //Things to do each time some STA disconnects from our Access Point.
}

void sta_connect_handler(void) {
    //Things to do when STA connects to some Access Point.
}

void sta_disconnect_handler(void) {
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

    /*AP+STA example*/

    //Init Wifi
    ESP_ERROR_CHECK(wifi_c_init_wifi(WIFI_C_MODE_APSTA));

    //Register STA connect handler (optional)
    ESP_ERROR_CHECK(wifi_c_ap_register_connect_handler(sta_connect_handler));

    //Register STA connect handler (optional)
    ESP_ERROR_CHECK(wifi_c_ap_register_disconnect_handler(sta_disconnect_handler));

    //Start STA and connect to AP:
    ESP_ERROR_CHECK(wifi_c_start_sta("STA_SSID", "STA_PASSWORD"));

    //Register AP connect handler (optional)
    ESP_ERROR_CHECK(wifi_c_ap_register_connect_handler(ap_connect_handler));

    //Register AP connect handler (optional)
    ESP_ERROR_CHECK(wifi_c_ap_register_disconnect_handler(ap_disconnect_handler));

    //Start AP with passed credentials:
    ESP_ERROR_CHECK(wifi_c_start_ap("AP_SSID", "AP_PASSWORD"));
}