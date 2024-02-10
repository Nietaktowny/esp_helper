#include "nvs_flash.h"
#include "esp_err.h"
#include "wifi_controller.h"

void connect_handler(void) {
    //Things to do each time new STA connects to our Access Point.
}

void disconnect_handler(void) {
    //Things to do each time some STA disconnects from our Access Point.
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

    /*AP example*/

    //Init Wifi
    ESP_ERROR_CHECK(wifi_c_init_wifi(WIFI_C_MODE_AP));

    //Register connect handler (optional)
    ESP_ERROR_CHECK(wifi_c_ap_register_connect_handler(connect_handler));

    //Register connect handler (optional)
    ESP_ERROR_CHECK(wifi_c_ap_register_disconnect_handler(disconnect_handler));

    //Start AP with passed credentials:
    ESP_ERROR_CHECK(wifi_c_start_ap("SSID", "PASSWORD"));

}