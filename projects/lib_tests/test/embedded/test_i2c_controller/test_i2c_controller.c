#include "unity.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

//Change fatal color to purple, to use it to log test names.
#include "ansi_colors.h"
#define FATAL_COLOR PURPLE_BOLD

#include "i2c_controller_internal.h"
#include "logger.h"

void setUp(void) {
  // set stuff up here
  logger_init();
}

void tearDown(void) {
  // clean stuff up here
}

void test_if_get_base_mac_returns_not_zero (void) {
  
}

int runUnityTests(void) {
  UNITY_BEGIN();
  RUN_TEST(test_if_get_base_mac_returns_not_zero);
  return UNITY_END();
}


/**
  * For ESP-IDF framework
  */
void app_main(void) {
  runUnityTests();
  vTaskDelay(200);
  fflush(stdout);
  esp_restart();
}