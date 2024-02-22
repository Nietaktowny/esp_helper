#include "unity.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include "logger.h"

void setUp(void) {
  // set stuff up here
}

void tearDown(void) {
  // clean stuff up here
}

void test_if_log_verbose_format_is_defined (void) {
    //before
    logger_create_semphr();
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    const char* verbose_format_expected = "%s%s | %-5s | %-15s:%d | %s:%d | ";

    //then
    TEST_ASSERT_EQUAL_STRING_MESSAGE(verbose_format_expected, LOG_VERBOSE_FORMAT, "LOG_VERBOSE_FORMAT different from expected.");
}

int runUnityTests(void) {
  UNITY_BEGIN();
  RUN_TEST(test_if_log_verbose_format_is_defined);
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