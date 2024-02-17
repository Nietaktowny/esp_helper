#include <stdio.h>
#include "logger.h"
#include "esp_helper_utils.h"


#define MY_SSID "TP-LINK_AD8313"
#define MY_PSK "20232887"
#define SOL_SSID "OstNet-952235"
#define SOL_PSK "Solonka106"

void app_main(void)
{
    // Init logger library
    logger_create_semphr();
    LOG_INFO("TESTING ESP HELPER UTILS");
}