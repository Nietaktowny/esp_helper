#include <stdio.h>
#include "logger.h"
#include "nvs_controller.h"

void app_main(void)
{
    // Init logger library
    logger_init();
    logger_set_log_level(LOG_LEVEL_VERBOSE);

    LOG_INFO("TESTING NVS CONTROLLER");


    nvs_c_handle_t nvs = NULL;
    const char* written = "Hello World!";
    char read[15] = {0};

    nvs_c_init_default_partition();
    nvs_c_open(&nvs, "nvs", NVS_C_READWRITE);
    nvs_c_write_string(nvs, "data", written);
    nvs_c_read_string(nvs, "data", read, sizeof(read));
    nvs_c_close(&nvs);
    
}