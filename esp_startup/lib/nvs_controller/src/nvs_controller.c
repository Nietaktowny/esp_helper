
#include "nvs.h"
#include "nvs_flash.h"
#include "nvs_controller.h"
#include "err_controller.h"


/**
 * @brief Initialize NVS.
 * 
 * @return esp_err_t Error code if something goes wrong.
 */
int nvs_c_init_nvs (void) {
    
    err_c_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    return err;
}