#include "unity.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include "esp_err.h"





void test_basic_test(void) {
    TEST_PASS();
}






/**
  * For ESP-IDF framework
  */
void app_main(void) {
  UNITY_BEGIN();
  test_basic_test(); 
  UNITY_END();
  vTaskDelay(200);
  fflush(stdout);
  esp_restart();
}