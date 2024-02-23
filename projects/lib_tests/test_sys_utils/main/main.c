#include <stdio.h>
#include "logger.h"
#include "sys_utils.h"


#define MY_SSID "TP-LINK_AD8313"
#define MY_PSK "20232887"
#define SOL_SSID "OstNet-952235"
#define SOL_PSK "Solonka106"

void app_main(void)
{
    // Init logger library
    logger_init();
    LOG_INFO("TESTING SYSTEM UTILS");

    uint8_t mac[6] = {0};
    sysutil_get_chip_base_mac(mac);
    LOG_INFO("MAC address: %02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]); 
}