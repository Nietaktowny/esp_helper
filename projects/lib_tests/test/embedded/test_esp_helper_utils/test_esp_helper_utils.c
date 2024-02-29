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
#include "err_controller.h"
#include "esp_helper_utils.h"

void setUp(void) {
  // set stuff up here
  logger_init();
}

void tearDown(void) {
  // clean stuff up here
}

void test_if_helper_prepare_url_prepares_good_url(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  const char* expected = "http://localhost:8081/esp_helper/ota.php?device_id=111111";
  const char* ota_url = "http://localhost:8081/esp_helper/ota.php";
  char url[100] = {0};
  char device_id[20] = "111111";

  //when
  helper_prepare_url_with_device_id(ota_url, device_id, url, sizeof(url));

  //then
  TEST_ASSERT_EQUAL_STRING_MESSAGE(expected, url, "helper_prepare_url_with_device_id returned different url from expected");
}

void test_if_helper_prepare_url_returns_zero(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  int err = -1;
  const char* ota_url = "http://localhost:8081/esp_helper/ota.php";
  char url[100] = {0};
  char device_id[20] = "111111";

  //when
  err = helper_prepare_url_with_device_id(ota_url, device_id, url, sizeof(url));

  //then
  TEST_ASSERT_EQUAL_MESSAGE(0, err, "helper_prepare_url_with_device_id should return 0.");
}

void test_if_helper_prepare_url_returns_err_on_null_ota_url(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  int err = 0;
  char url[100] = {0};
  const char device_id[20] = "111111";

  //when
  err = helper_prepare_url_with_device_id(NULL, device_id, url, sizeof(url));

  //then
  LOG_ERROR("ERROR %#x, %s: ", err, error_to_name(err));
  TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "helper_prepare_url_with_device_id should return err on NULL ota URL");
}

void test_if_helper_prepare_url_returns_err_on_null_device_id(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  int err = 0;
  const char* ota_url = "http://localhost:8081/esp_helper/ota.php";
  char url[100] = {0};

  //when
  err = helper_prepare_url_with_device_id(ota_url, NULL, url, sizeof(url));

  //then
  TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "helper_prepare_url_with_device_id should return err on NULL device_id");
}

void test_if_helper_prepare_url_returns_err_on_null_buffer(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  int err = 0;
  const char* ota_url = "http://localhost:8081/esp_helper/ota.php";
  const char device_id[20] = "111111";

  //when
  err = helper_prepare_url_with_device_id(ota_url, device_id, NULL, 120);

  //then
  TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "helper_prepare_url_with_device_id should return err on NULL buffer");
}

void test_if_helper_prepare_url_returns_err_on_too_small_buffer(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  int err = 0;
  const char* ota_url = "http://localhost:8081/esp_helper/ota.php";
  const char device_id[20] = "111111";
  char url[100] = {0};

  //when
  err = helper_prepare_url_with_device_id(ota_url, device_id, url, 50);

  //then
  TEST_ASSERT_EQUAL_MESSAGE(ERR_C_MEMORY_ERR, err, "helper_prepare_url_with_device_id should return err on too small buffer");
}

void test_if_helper_perform_ota_returns_err_on_not_connected_wifi(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  int err = 0;

  //when
  err = helper_perform_ota();

  //then
  TEST_ASSERT_EQUAL_MESSAGE(ESP_HELPER_ERR_NOT_CONNECTED, err, "helper_perform_ota should return err on not connected wifi");  
}

int runUnityTests(void) {
  UNITY_BEGIN();
  RUN_TEST(test_if_helper_perform_ota_returns_err_on_not_connected_wifi);
  RUN_TEST(test_if_helper_perform_ota_returns_err_on_not_connected_wifi);
  RUN_TEST(test_if_helper_prepare_url_returns_err_on_too_small_buffer);
  RUN_TEST(test_if_helper_prepare_url_returns_err_on_null_buffer);
  RUN_TEST(test_if_helper_prepare_url_returns_err_on_null_device_id);
  RUN_TEST(test_if_helper_prepare_url_prepares_good_url);
  RUN_TEST(test_if_helper_prepare_url_returns_zero);
  RUN_TEST(test_if_helper_prepare_url_returns_err_on_null_ota_url);
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