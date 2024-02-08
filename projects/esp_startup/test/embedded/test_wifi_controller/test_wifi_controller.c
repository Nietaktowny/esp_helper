#include "unity.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include "esp_err.h"

#include "logger.h"
#include "errors_list.h"
#include "nvs_flash.h"

#include "wifi_controller.h"

void setUp(void) {
    int err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    logger_create_semphr();
}

void tearDown(void) {
  // clean stuff up here
}

void afterEach (void) {
  wifi_c_deinit();
  vTaskDelay(300);
}

void test_check_if_netif_status_is_initialized(void) {
  //given
  wifi_c_status_t* wifi_status = wifi_c_get_status();
  TEST_ASSERT_EQUAL_MESSAGE(false, wifi_status->wifi_initialized, "netif_initialized should be false before initialization.");

  //then
  wifi_c_init_wifi(WIFI_C_MODE_STA);
  

  //then
  TEST_ASSERT_EQUAL_MESSAGE(true, wifi_status->netif_initialized, "netif_initialized should be true after initialization.");

  //after
  afterEach();
}

void test_check_if_wifi_status_is_initialized_as_sta(void) {
  //given
  wifi_c_status_t* wifi_status = wifi_c_get_status();
  TEST_ASSERT_EQUAL_MESSAGE(false, wifi_status->wifi_initialized, "wifi_initialized should be false before initialization.");

  //then
  wifi_c_init_wifi(WIFI_C_MODE_STA);
  

  //then
  TEST_ASSERT_EQUAL_MESSAGE(true, wifi_status->wifi_initialized, "wifi_initialized should be true after initialization.");

  //after
  afterEach();
}

void test_check_if_wifi_status_is_initialized_as_ap(void) {
  //given
  wifi_c_status_t* wifi_status = wifi_c_get_status();
  TEST_ASSERT_EQUAL_MESSAGE(false, wifi_status->wifi_initialized, "wifi_initialized should be false before initialization.");

  //then
  wifi_c_init_wifi(WIFI_C_MODE_AP);
  

  //then
  TEST_ASSERT_EQUAL_MESSAGE(true, wifi_status->wifi_initialized, "wifi_initialized should be true after initialization.");

  //after
  afterEach();
}

void test_check_if_wifi_status_is_initialized_as_apsta(void) {
  //given
  wifi_c_status_t* wifi_status = wifi_c_get_status();
  TEST_ASSERT_EQUAL_MESSAGE(false, wifi_status->wifi_initialized, "wifi_initialized should be false before initialization.");

  //then
  wifi_c_init_wifi(WIFI_C_MODE_APSTA);
  

  //then
  TEST_ASSERT_EQUAL_MESSAGE(true, wifi_status->wifi_initialized, "wifi_initialized should be true after initialization.");

  //after
  afterEach();
}

void test_check_initial_ap_ip_value(void) {
   //given
  wifi_c_status_t* wifi_status = wifi_c_get_status();
  const char* expected = "0.0.0.0";

  //then
  TEST_ASSERT_EQUAL_STRING_MESSAGE(expected, wifi_status->ap.ip, "Wifi initial value of AP IP different than expected");
}

void test_check_initial_sta_ip_value(void) {
   //given
  wifi_c_status_t* wifi_status = wifi_c_get_status();
  const char* expected = "0.0.0.0";

  //then
  TEST_ASSERT_EQUAL_STRING_MESSAGE(expected, wifi_status->sta.ip, "Wifi initial value of STA IP different than expected");
}

void test_check_initial_ap_ssid_value(void) {
   //given
  wifi_c_status_t* wifi_status = wifi_c_get_status();
  const char* expected = "none";

  //then
  TEST_ASSERT_EQUAL_STRING_MESSAGE(expected, wifi_status->ap.ssid, "Wifi initial value of AP SSID different than expected");
}

void test_check_initial_sta_ssid_value(void) {
   //given
  wifi_c_status_t* wifi_status = wifi_c_get_status();
  const char* expected = "none";

  //when
  TEST_ASSERT_EQUAL_STRING_MESSAGE(expected, wifi_status->sta.ssid, "Wifi initial value of STA SSID different than expected");
}

void test_check_initial_wifi_initialized_value(void) {
   //given
  wifi_c_status_t* wifi_status = wifi_c_get_status();

  //then
  TEST_ASSERT_EQUAL_MESSAGE(false, wifi_status->wifi_initialized, "Wifi initialized should be false before wifi_c_init");
}

void test_check_initial_netif_initialized_value(void) {
   //given
  wifi_c_status_t* wifi_status = wifi_c_get_status();

  //then
  TEST_ASSERT_EQUAL_MESSAGE(false, wifi_status->netif_initialized, "Netif initialized should be false before wifi_c_init");
}

void test_check_initial_event_loop_started_value(void) {
   //given
  wifi_c_status_t* wifi_status = wifi_c_get_status();

  //then
  TEST_ASSERT_EQUAL_MESSAGE(false, wifi_status->even_loop_started, "event_loop_started value should be false before wifi_c_init");
}

void test_check_initial_sta_started_value(void) {
   //given
  wifi_c_status_t* wifi_status = wifi_c_get_status();

  //then
  TEST_ASSERT_EQUAL_MESSAGE(false, wifi_status->sta_started, "sta_started value should be false before wifi_c_init");
}

void test_check_initial_ap_started_value(void) {
   //given
  wifi_c_status_t* wifi_status = wifi_c_get_status();

  //then
  TEST_ASSERT_EQUAL_MESSAGE(false, wifi_status->ap_started, "ap_started value should be false before wifi_c_init");
}

void test_check_initial_scan_done_value(void) {
   //given
  wifi_c_status_t* wifi_status = wifi_c_get_status();

  //then
  TEST_ASSERT_EQUAL_MESSAGE(false, wifi_status->scan_done, "scan_done value should be false before wifi_c_init");
}

void test_check_initial_sta_connected_value(void) {
   //given
  wifi_c_status_t* wifi_status = wifi_c_get_status();

  //then
  TEST_ASSERT_EQUAL_MESSAGE(false, wifi_status->sta_connected, "sta_connected value should be false before wifi_c_init");
}

void test_check_initial_ap_connect_handler_value(void) {
   //given
  wifi_c_status_t* wifi_status = wifi_c_get_status();

  //then
  TEST_ASSERT_NULL_MESSAGE(wifi_status->ap.connect_handler, "ap.connect_handler value should be false before setting it.");
}

void test_check_initial_sta_connect_handler_value(void) {
   //given
  wifi_c_status_t* wifi_status = wifi_c_get_status();

  //then
  TEST_ASSERT_NULL_MESSAGE(wifi_status->sta.connect_handler, "sta.connect_handler value should be false before setting it.");
}

void test_if_wifi_status_is_initalized_when_init_as_ap(void) {
  //given
  wifi_c_status_t* wifi = NULL;

  //when
  wifi_c_init_wifi(WIFI_C_MODE_AP);
  wifi = wifi_c_get_status();
  
  //then
  TEST_ASSERT_NOT_NULL(wifi);
  TEST_ASSERT_EQUAL_MESSAGE(true, wifi->wifi_initialized, "wifi_initialized should be true after wifi_c_init");

  //after
  afterEach();
}

void test_if_wifi_status_is_initalized_when_init_as_sta(void) {
  //given
  wifi_c_status_t* wifi = NULL;

  //when
  wifi_c_init_wifi(WIFI_C_MODE_STA);
  wifi = wifi_c_get_status();
  
  //then
  TEST_ASSERT_NOT_NULL(wifi);
  TEST_ASSERT_EQUAL_MESSAGE(true, wifi->wifi_initialized, "wifi_initialized should be true after wifi_c_init");

  //after
  afterEach();
}

void test_if_wifi_status_is_initalized_when_init_as_apsta(void) {
  //given
  wifi_c_status_t* wifi = NULL;

  //when
  wifi_c_init_wifi(WIFI_C_MODE_APSTA);
  wifi = wifi_c_get_status();
  
  //then
  TEST_ASSERT_NOT_NULL(wifi);
  TEST_ASSERT_EQUAL_MESSAGE(true, wifi->wifi_initialized, "wifi_initialized should be true after wifi_c_init");

  //after
  afterEach();
}

void test_if_netif_status_is_initalized_when_init_as_ap(void) {
  //given
  wifi_c_status_t* wifi = NULL;

  //when
  wifi_c_init_wifi(WIFI_C_MODE_AP);
  wifi = wifi_c_get_status();
  
  //then
  TEST_ASSERT_NOT_NULL(wifi);
  TEST_ASSERT_EQUAL_MESSAGE(true, wifi->netif_initialized, "netif_initialized should be true after wifi_c_init");

  //after
  afterEach();
}

void test_if_netif_status_is_initalized_when_init_as_sta(void) {
  //given
  wifi_c_status_t* wifi = NULL;

  //when
  wifi_c_init_wifi(WIFI_C_MODE_STA);
  wifi = wifi_c_get_status();
  
  //then
  TEST_ASSERT_NOT_NULL(wifi);
  TEST_ASSERT_EQUAL_MESSAGE(true, wifi->netif_initialized, "netif_initialized should be true after wifi_c_init");

  //after
  afterEach();
}

void test_if_netif_status_is_initalized_when_init_as_apsta(void) {
  //given
  wifi_c_status_t* wifi = NULL;

  //when
  wifi_c_init_wifi(WIFI_C_MODE_APSTA);
  wifi = wifi_c_get_status();
  
  //then
  TEST_ASSERT_NOT_NULL(wifi);
  TEST_ASSERT_EQUAL_MESSAGE(true, wifi->netif_initialized, "netif_initialized should be true after wifi_c_init");

  //after
  afterEach();
}

void test_if_sta_started_is_false_when_init_as_ap(void) {
  //given
  wifi_c_status_t* wifi = NULL;

  //when
  wifi_c_init_wifi(WIFI_C_MODE_AP);
  wifi = wifi_c_get_status();
  
  //then
  TEST_ASSERT_NOT_NULL(wifi);
  TEST_ASSERT_EQUAL_MESSAGE(false, wifi->sta_started, "sta_started should be false after wifi_c_init as AP");

  //after
  afterEach();
}

void test_if_sta_started_is_true_when_init_as_sta(void) {
  //given
  wifi_c_status_t* wifi = NULL;

  //when
  wifi_c_init_wifi(WIFI_C_MODE_STA);
  wifi = wifi_c_get_status();
  
  //then
  TEST_ASSERT_NOT_NULL(wifi);
  TEST_ASSERT_EQUAL_MESSAGE(true, wifi->sta_started, "sta_started should be true after wifi_c_init as STA");

  //after
  afterEach();
}

void test_if_sta_started_is_true_when_init_as_apsta(void) {
  //given
  wifi_c_status_t* wifi = NULL;

  //when
  wifi_c_init_wifi(WIFI_C_MODE_APSTA);
  wifi = wifi_c_get_status();
  
  //then
  TEST_ASSERT_NOT_NULL(wifi);
  TEST_ASSERT_EQUAL_MESSAGE(true, wifi->sta_started, "sta_started should be true after wifi_c_init as APSTA");

  //after
  afterEach();
}

void test_if_wifi_init_throws_err_on_wrong_mode_value(void) {
  //given
  int err = 0;

  //when
  err = wifi_c_init_wifi(-2);
  
  //then
  TEST_ASSERT_EQUAL_MESSAGE(WIFI_C_ERR_NETIF_INIT_FAILED, err, "on wrong wifi mode value wifi init should throw error");

  //after
  afterEach();  
}

void test_if_wifi_init_throws_err_on_multiple_calls(void) {
  //given
  int err = 0;

  //when
  err = wifi_c_init_wifi(WIFI_C_MODE_AP);
  
  //then
  TEST_ASSERT_EQUAL_MESSAGE(0, err, "on first wifi_init should return 0");

  //when
  err = wifi_c_init_wifi(WIFI_C_MODE_AP);
  
  //then
  TEST_ASSERT_EQUAL_MESSAGE(WIFI_C_ERR_WIFI_ALREADY_INIT, err, "on second wifi_c_init_wifi call with the same mode, should return err");

  //after
  afterEach();  
}

void test_if_wifi_init_not_throws_err_on_call_with_different_mode(void) {
  //given
  int err = 0;

  //when
  err = wifi_c_init_wifi(WIFI_C_MODE_AP);
  
  //then
  TEST_ASSERT_EQUAL_MESSAGE(0, err, "on first wifi_init should return 0");

  //when
  err = wifi_c_init_wifi(WIFI_C_MODE_STA);
  
  //then
  TEST_ASSERT_EQUAL_MESSAGE(0, err, "on second wifi_c_init_wifi call with different mode, should not return err");

  //after
  afterEach();  
}

void test_if_wifi_c_start_ap_returns_zero_on_success(void) {
  //given
  int err = 0;

  //when
  wifi_c_init_wifi(WIFI_C_MODE_AP);
  err = wifi_c_start_ap("esp", "password");
  
  //then
  TEST_ASSERT_EQUAL_MESSAGE(0, err, "wifi_c_start_ap should return 0 on success");

  //after
  afterEach();  
}

void test_if_wifi_c_start_ap_returns_err_when_wifi_mode_is_not_ap(void) {
  //given
  int err = 0;

  //when
  wifi_c_init_wifi(WIFI_C_MODE_STA);
  err = wifi_c_start_ap("esp", "password");
  
  //then
  TEST_ASSERT_EQUAL_MESSAGE(WIFI_C_ERR_WRONG_MODE, err, "wifi_c_start_ap should return err on wrong mode");

  //after
  afterEach();  
}

void test_if_wifi_c_start_sta_returns_connect_fail_on_wrong_credentials(void) {
  //given
  int err = 0;

  //when
  wifi_c_init_wifi(WIFI_C_MODE_STA);
  err = wifi_c_start_sta("3323rdad", "sdadwqe2");
  
  //then
  TEST_ASSERT_EQUAL_MESSAGE(WIFI_C_ERR_STA_CONNECT_FAIL, err, "wifi_c_start_sta should return 0 on success");

  //after
  afterEach();  
}

void test_if_wifi_c_start_sta_returns_err_when_wifi_mode_is_not_sta(void) {
  //given
  int err = 0;

  //when
  wifi_c_init_wifi(WIFI_C_MODE_AP);
  err = wifi_c_start_sta("esp", "password");
  
  //then
  TEST_ASSERT_EQUAL_MESSAGE(WIFI_C_ERR_WRONG_MODE, err, "wifi_c_start_sta should return err on wrong mode");

  //after
  afterEach();  
}

void test_if_wifi_c_start_ap_returns_zero_on_zero_len_pass(void) {
  //given
  int err = 0;

  //when
  wifi_c_init_wifi(WIFI_C_MODE_AP);
  err = wifi_c_start_ap("esp", "");
  
  //then
  TEST_ASSERT_EQUAL_MESSAGE(0, err, "wifi_c_start_ap should return 0 on success even with 0 len password");

  //after
  afterEach();  
}

void test_if_wifi_c_start_ap_returns_zero_on_null_pass(void) {
  //given
  int err = 0;

  //when
  wifi_c_init_wifi(WIFI_C_MODE_AP);
  err = wifi_c_start_ap("esp", NULL);
  
  //then
  TEST_ASSERT_EQUAL_MESSAGE(0, err, "wifi_c_start_ap should return 0 on success even with NULL password");

  //after
  afterEach();  
}

void test_if_wifi_c_scan_all_ap_returns_zero(void) {
  //given
  int err = 0;
  wifi_c_scan_result_t result;

  //when
  wifi_c_init_wifi(WIFI_C_MODE_STA);
  wifi_c_start_sta("asdas", "test");
  err = wifi_c_scan_all_ap(&result);

  //then
  TEST_ASSERT_EQUAL_MESSAGE(0, err, "wifi_c_scan_all_ap should return 0 on success");

  //after
  afterEach();  
}

void test_if_wifi_c_scan_all_ap_returns_err_on_null_buffer(void) {
  //given
  int err = 0;

  //when
  wifi_c_init_wifi(WIFI_C_MODE_STA);
  wifi_c_start_sta("asdas", "test");
  err = wifi_c_scan_all_ap(NULL);

  //then
  TEST_ASSERT_EQUAL_MESSAGE(ERR_NULL_POINTER, err, "wifi_c_scan_all_ap should return null pointer error on null buffer");

  //after
  afterEach();  
}

void test_if_wifi_c_scan_all_ap_returns_err_on_not_init_wifi(void) {
  //given
  int err = 0;
  wifi_c_scan_result_t result;

  //when
  wifi_c_deinit();
  err = wifi_c_scan_all_ap(&result);

  //then
  TEST_ASSERT_EQUAL_MESSAGE(WIFI_C_ERR_WIFI_NOT_INIT, err, "wifi_c_scan_all_ap should return err when wifi not init");

  //after
  afterEach();  
}

void test_if_wifi_initialized_is_false_after_deinit(void) {
  //given
  wifi_c_status_t* status = NULL;
  wifi_c_init_wifi(WIFI_C_MODE_APSTA);

  //when
  wifi_c_deinit();
  status = wifi_c_get_status();

  //then
  TEST_ASSERT_EQUAL_MESSAGE(false, status->wifi_initialized, "wifi_initialized should be false after deinit");
}

void test_if_netif_initialized_is_false_after_deinit(void) {
  //given
  wifi_c_status_t* status = NULL;
  wifi_c_init_wifi(WIFI_C_MODE_APSTA);

  //when
  wifi_c_deinit();
  status = wifi_c_get_status();

  //then
  TEST_ASSERT_EQUAL_MESSAGE(false, status->netif_initialized, "netif_initialized should be false after deinit");
}

void test_if_event_loop_started_is_false_after_deinit(void) {
  //given
  wifi_c_status_t* status = NULL;
  wifi_c_init_wifi(WIFI_C_MODE_APSTA);

  //when
  wifi_c_deinit();
  status = wifi_c_get_status();

  //then
  TEST_ASSERT_EQUAL_MESSAGE(false, status->even_loop_started, "even_loop_started should be false after deinit");
}

void test_if_sta_started_is_false_after_deinit(void) {
  //given
  wifi_c_status_t* status = NULL;
  wifi_c_init_wifi(WIFI_C_MODE_APSTA);

  //when
  wifi_c_deinit();
  status = wifi_c_get_status();

  //then
  TEST_ASSERT_EQUAL_MESSAGE(false, status->sta_started, "sta_started should be false after deinit");
}

void test_if_ap_started_is_false_after_deinit(void) {
  //given
  wifi_c_status_t* status = NULL;
  wifi_c_init_wifi(WIFI_C_MODE_APSTA);

  //when
  wifi_c_deinit();
  status = wifi_c_get_status();

  //then
  TEST_ASSERT_EQUAL_MESSAGE(false, status->ap_started, "ap_started should be false after deinit");
}

void test_if_sta_connected_is_false_after_deinit(void) {
  //given
  wifi_c_status_t* status = NULL;
  wifi_c_init_wifi(WIFI_C_MODE_APSTA);

  //when
  wifi_c_deinit();
  status = wifi_c_get_status();

  //then
  TEST_ASSERT_EQUAL_MESSAGE(false, status->sta_connected, "sta_connected should be false after deinit");
}

void test_if_wifi_mode_is_set_after_deinit(void) {
  //given
  wifi_c_status_t* status = NULL;
  wifi_c_init_wifi(WIFI_C_MODE_APSTA);

  //when
  wifi_c_deinit();
  status = wifi_c_get_status();

  //then
  TEST_ASSERT_EQUAL_MESSAGE(WIFI_C_NO_MODE, status->wifi_mode, "wifi_mode should be WIFI_C_NO_MODE after deinit");
}

void test_if_sta_ip_is_cleared_after_deinit(void) {
  //given
  wifi_c_status_t* status = NULL;
  wifi_c_init_wifi(WIFI_C_MODE_APSTA);

  //when
  wifi_c_deinit();
  status = wifi_c_get_status();

  //then
  TEST_ASSERT_EQUAL_STRING_MESSAGE("0.0.0.0", status->sta.ip, "sta.ip should be 0.0.0.0 after deinit");
}

void test_if_ap_ip_is_cleared_after_deinit(void) {
  //given
  wifi_c_status_t* status = NULL;
  wifi_c_init_wifi(WIFI_C_MODE_APSTA);

  //when
  wifi_c_deinit();
  status = wifi_c_get_status();

  //then
  TEST_ASSERT_EQUAL_STRING_MESSAGE("0.0.0.0", status->ap.ip, "ap.ip should be 0.0.0.0 after deinit");
}

void test_if_ap_ssid_is_cleared_after_deinit(void) {
  //given
  wifi_c_status_t* status = NULL;
  wifi_c_init_wifi(WIFI_C_MODE_APSTA);

  //when
  wifi_c_deinit();
  status = wifi_c_get_status();

  //then
  TEST_ASSERT_EQUAL_STRING_MESSAGE("none", status->ap.ssid, "ap.ssid should be none after deinit");
}

void test_if_sta_ssid_is_cleared_after_deinit(void) {
  //given
  wifi_c_status_t* status = NULL;
  wifi_c_init_wifi(WIFI_C_MODE_APSTA);

  //when
  wifi_c_deinit();
  status = wifi_c_get_status();

  //then
  TEST_ASSERT_EQUAL_STRING_MESSAGE("none", status->sta.ssid, "sta.ssid should be none after deinit");
}

void test_if_wifi_c_scan_all_ap_returns_err_on_not_started_sta(void) {
  //given
  int err = 0;
  wifi_c_scan_result_t result;
  wifi_c_status_t* status = NULL;
  wifi_c_deinit();

  //when
  wifi_c_init_wifi(WIFI_C_MODE_STA);
  status = wifi_c_get_status();
  TEST_ASSERT_EQUAL(WIFI_C_MODE_STA, status->wifi_mode);
  TEST_ASSERT_EQUAL(false, status->sta_started);
  err = wifi_c_scan_all_ap(&result);

  //then
  TEST_ASSERT_EQUAL_MESSAGE(WIFI_C_ERR_STA_NOT_STARTED, err, "wifi_c_scan_all_ap should return err when sta not started");

  //after
  afterEach();  
}

void test_if_wifi_c_scan_all_ap_returns_err_on_wrong_wifi_mode(void) {
  //given
  int err = 0;
  wifi_c_scan_result_t result;
  wifi_c_status_t* status = NULL;
  wifi_c_deinit();

  //when
  wifi_c_init_wifi(WIFI_MODE_AP);
  wifi_c_start_ap("ssid", NULL);
  status = wifi_c_get_status();
  TEST_ASSERT_EQUAL(WIFI_C_MODE_AP, status->wifi_mode);
  TEST_ASSERT_EQUAL(true, status->ap_started);
  TEST_ASSERT_EQUAL(false, status->sta_started);
  err = wifi_c_scan_all_ap(&result);

  //then
  TEST_ASSERT_EQUAL_MESSAGE(WIFI_C_ERR_WRONG_MODE, err, "wifi_c_scan_all_ap should return err when wifi mode is AP");

  //after
  afterEach();  
}

void run_wifi_controller_tests(void) {
  UNITY_BEGIN();
  RUN_TEST(test_if_sta_ssid_is_cleared_after_deinit);
  RUN_TEST(test_if_ap_ssid_is_cleared_after_deinit);
  RUN_TEST(test_if_ap_ip_is_cleared_after_deinit);
  RUN_TEST(test_if_sta_ip_is_cleared_after_deinit);
  RUN_TEST(test_if_wifi_mode_is_set_after_deinit);
  RUN_TEST(test_if_sta_connected_is_false_after_deinit);
  RUN_TEST(test_if_ap_started_is_false_after_deinit);
  RUN_TEST(test_if_sta_started_is_false_after_deinit);
  RUN_TEST(test_if_event_loop_started_is_false_after_deinit);
  RUN_TEST(test_if_netif_initialized_is_false_after_deinit);
  RUN_TEST(test_if_wifi_initialized_is_false_after_deinit);
  RUN_TEST(test_if_wifi_c_scan_all_ap_returns_err_on_wrong_wifi_mode);
  RUN_TEST(test_if_wifi_c_scan_all_ap_returns_err_on_not_started_sta);
  RUN_TEST(test_if_wifi_c_scan_all_ap_returns_err_on_not_init_wifi);
  RUN_TEST(test_if_wifi_c_scan_all_ap_returns_err_on_null_buffer);
  RUN_TEST(test_if_wifi_c_scan_all_ap_returns_zero);
  RUN_TEST(test_if_wifi_c_start_ap_returns_zero_on_null_pass);
  RUN_TEST(test_if_wifi_c_start_ap_returns_zero_on_zero_len_pass);
  RUN_TEST(test_if_wifi_c_start_sta_returns_connect_fail_on_wrong_credentials);
  RUN_TEST(test_if_wifi_c_start_sta_returns_err_when_wifi_mode_is_not_sta);
  RUN_TEST(test_if_wifi_c_start_ap_returns_err_when_wifi_mode_is_not_ap);
  RUN_TEST(test_if_wifi_c_start_ap_returns_zero_on_success);
  RUN_TEST(test_if_wifi_init_not_throws_err_on_call_with_different_mode);
  RUN_TEST(test_if_wifi_init_throws_err_on_multiple_calls);
  RUN_TEST(test_if_wifi_init_throws_err_on_wrong_mode_value);
  RUN_TEST(test_if_sta_started_is_true_when_init_as_apsta);
  RUN_TEST(test_if_sta_started_is_true_when_init_as_sta);
  RUN_TEST(test_if_sta_started_is_false_when_init_as_ap);
  RUN_TEST(test_if_netif_status_is_initalized_when_init_as_apsta);
  RUN_TEST(test_if_netif_status_is_initalized_when_init_as_sta);
  RUN_TEST(test_if_netif_status_is_initalized_when_init_as_ap);
  RUN_TEST(test_if_wifi_status_is_initalized_when_init_as_apsta);
  RUN_TEST(test_if_wifi_status_is_initalized_when_init_as_sta);
  RUN_TEST(test_if_wifi_status_is_initalized_when_init_as_ap);
  RUN_TEST(test_check_initial_sta_connect_handler_value);
  RUN_TEST(test_check_initial_ap_connect_handler_value);
  RUN_TEST(test_check_initial_sta_connected_value);
  RUN_TEST(test_check_initial_scan_done_value);
  RUN_TEST(test_check_initial_ap_started_value);
  RUN_TEST(test_check_initial_sta_started_value);
  RUN_TEST(test_check_initial_event_loop_started_value);
  RUN_TEST(test_check_initial_netif_initialized_value);
  RUN_TEST(test_check_initial_wifi_initialized_value);
  RUN_TEST(test_check_initial_sta_ssid_value);
  RUN_TEST(test_check_initial_ap_ssid_value);
  RUN_TEST(test_check_initial_sta_ip_value);
  RUN_TEST(test_check_if_wifi_status_is_initialized_as_sta);
  RUN_TEST(test_check_if_wifi_status_is_initialized_as_ap);
  RUN_TEST(test_check_if_wifi_status_is_initialized_as_apsta);
  RUN_TEST(test_check_if_netif_status_is_initialized);
  RUN_TEST(test_check_initial_ap_ip_value);
  UNITY_END();
}

/**
  * For ESP-IDF framework
  */
void app_main(void) {
  run_wifi_controller_tests();
  vTaskDelay(200);
  fflush(stdout);
  esp_restart();
}