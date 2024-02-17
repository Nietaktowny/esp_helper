#include "unity.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include <string.h>

#include "logger.h"
#include "errors_list.h"
#include "esp_helper_utils.h"

void setUp(void) {
  // set stuff up here
}

void tearDown(void) {
  // clean stuff up here
}

void test_if_get_base_mac_returns_not_zero(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given


  //when


  //then

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