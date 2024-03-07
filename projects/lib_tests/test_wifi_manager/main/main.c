#include <stdio.h>

#include "nvs_flash.h"
#include "esp_err.h"

#include "logger.h"
#include "wifi_manager.h"
#include "nvs_controller.h"


void app_main(void)
{
    // Init logger library
    logger_init();
    logger_set_log_level(LOG_LEVEL_VERBOSE);

	//Init NVS library.
	nvs_c_init_default_partition();

    //Reset previously stored AP (just to be sure)
    //wifi_manager_erase_ap();

    // Init wifi manager
    wifi_manager_init();
}
