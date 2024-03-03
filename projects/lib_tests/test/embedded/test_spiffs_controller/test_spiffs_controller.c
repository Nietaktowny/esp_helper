#include "unity.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

//Change fatal color to purple, to use it to log test names.
#include "ansi_colors.h"
#define FATAL_COLOR PURPLE_BOLD

#include "spiffs_controller.h"
#include "logger.h"

void setUp(void) {
  // set stuff up here
  logger_init();
  logger_set_log_level(LOG_LEVEL_VERBOSE);
}

void tearDown(void) {
  // clean stuff up here
}

void test_if_nvs_init_returns_zero (void) {
  
}

int runUnityTests(void) {
  UNITY_BEGIN();
  RUN_TEST(test_if_nvs_init_returns_zero);
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
