#include "unity.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include <string.h>

//Change fatal color to purple, to use it to log test names.
#include "ansi_colors.h"
#define FATAL_COLOR PURPLE_BOLD

#include "logger.h"
#include "err_c_errors.h"
#include "sys_utils.h"

void setUp(void) {
  // set stuff up here
  logger_init();
}

void tearDown(void) {
  // clean stuff up here
}

void test_if_get_base_mac_returns_not_zero(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  uint8_t mac[6] = {0};

  //when
  sysutil_get_chip_base_mac(mac);

  //then
  for (uint8_t i = 0; i < 6; i++)
  {
    TEST_ASSERT_NOT_EQUAL_MESSAGE(0, mac[i], "mac address shouldn't be 0");
  }
  
}

void test_if_get_base_mac_as_str_has_non_zero_length(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  char mac[20] = {0};

  //when
  sysutil_get_chip_base_mac_as_str(mac, sizeof(mac));

  //then
  TEST_ASSERT_TRUE_MESSAGE(strlen(mac) > 0, "mac address should be set as string");
}

void test_if_get_base_mac_as_str_returns_err_on_null_buffer(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  int err = 0;

  //when
  err = sysutil_get_chip_base_mac_as_str(NULL, 23);

  //then
  TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "sysutil_get_chip_base_mac_as_str should return err on NULL pointer");
}

void test_if_get_base_mac_as_str_returns_err_on_too_smalL_buffer(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  int err = 0;
  char buffer[17] = {0};

  //when
  err = sysutil_get_chip_base_mac_as_str(buffer, sizeof(buffer));

  //then
  TEST_ASSERT_EQUAL_MESSAGE(ERR_C_MEMORY_ERR, err, "sysutil_get_chip_base_mac_as_str should return err on too small buffer");
}

int runUnityTests(void) {
  UNITY_BEGIN();
  RUN_TEST(test_if_get_base_mac_as_str_returns_err_on_too_smalL_buffer);
  RUN_TEST(test_if_get_base_mac_as_str_returns_err_on_null_buffer);
  RUN_TEST(test_if_get_base_mac_as_str_has_non_zero_length);
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