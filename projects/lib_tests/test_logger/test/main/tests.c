#include "unity.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include "test_logger.h"

/**
 * For ESP-IDF framework
 */
void app_main(void)
{
    runUnityTests();
    vTaskDelay(200);
    fflush(stdout);
    esp_restart();
}