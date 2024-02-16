#include <stdio.h>

#include "nvs_flash.h"
#include "esp_err.h"

#include "logger.h"
#include "wifi_manager.h"


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

    //Reset previously stored AP (just to be sure)
    wifi_manager_erase_ap();

    // Init wifi manager
    wifi_manager_init();
}