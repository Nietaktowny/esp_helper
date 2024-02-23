#include "unity.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include "esp_err.h"
#include "esp_wifi_types.h"

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

    logger_init();
}

void tearDown(void) {
  // clean stuff up here
}

void afterEach (void) {
  wifi_c_deinit();
  vTaskDelay(300);
}

void test_check_if_netif_status_is_initialized(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

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
  //before
  LOG_FATAL("RUNNING: %s", __func__);

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
  //before
  LOG_FATAL("RUNNING: %s", __func__);

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
  //before
  LOG_FATAL("RUNNING: %s", __func__);

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
  //before
  LOG_FATAL("RUNNING: %s", __func__);

   //given
  wifi_c_status_t* wifi_status = wifi_c_get_status();
  const char* expected = "0.0.0.0";

  //then
  TEST_ASSERT_EQUAL_STRING_MESSAGE(expected, wifi_status->ap.ip, "Wifi initial value of AP IP different than expected");
}

void test_check_initial_sta_ip_value(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

   //given
  wifi_c_status_t* wifi_status = wifi_c_get_status();
  const char* expected = "0.0.0.0";

  //then
  TEST_ASSERT_EQUAL_STRING_MESSAGE(expected, wifi_status->sta.ip, "Wifi initial value of STA IP different than expected");
}

void test_check_initial_ap_ssid_value(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

   //given
  wifi_c_status_t* wifi_status = wifi_c_get_status();
  const char* expected = "none";

  //then
  TEST_ASSERT_EQUAL_STRING_MESSAGE(expected, wifi_status->ap.ssid, "Wifi initial value of AP SSID different than expected");
}

void test_check_initial_sta_ssid_value(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

   //given
  wifi_c_status_t* wifi_status = wifi_c_get_status();
  const char* expected = "none";

  //when
  TEST_ASSERT_EQUAL_STRING_MESSAGE(expected, wifi_status->sta.ssid, "Wifi initial value of STA SSID different than expected");
}

void test_check_initial_wifi_initialized_value(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

   //given
  wifi_c_status_t* wifi_status = wifi_c_get_status();

  //then
  TEST_ASSERT_EQUAL_MESSAGE(false, wifi_status->wifi_initialized, "Wifi initialized should be false before wifi_c_init");
}

void test_check_initial_netif_initialized_value(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

   //given
  wifi_c_status_t* wifi_status = wifi_c_get_status();

  //then
  TEST_ASSERT_EQUAL_MESSAGE(false, wifi_status->netif_initialized, "Netif initialized should be false before wifi_c_init");
}

void test_check_initial_event_loop_started_value(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

   //given
  wifi_c_status_t* wifi_status = wifi_c_get_status();

  //then
  TEST_ASSERT_EQUAL_MESSAGE(false, wifi_status->even_loop_started, "event_loop_started value should be false before wifi_c_init");
}

void test_check_initial_sta_started_value(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

   //given
  wifi_c_status_t* wifi_status = wifi_c_get_status();

  //then
  TEST_ASSERT_EQUAL_MESSAGE(false, wifi_status->sta_started, "sta_started value should be false before wifi_c_init");
}

void test_check_initial_ap_started_value(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

   //given
  wifi_c_status_t* wifi_status = wifi_c_get_status();

  //then
  TEST_ASSERT_EQUAL_MESSAGE(false, wifi_status->ap_started, "ap_started value should be false before wifi_c_init");
}

void test_check_initial_scan_done_value(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

   //given
  wifi_c_status_t* wifi_status = wifi_c_get_status();

  //then
  TEST_ASSERT_EQUAL_MESSAGE(false, wifi_status->scan_done, "scan_done value should be false before wifi_c_init");
}

void test_check_initial_sta_connected_value(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

   //given
  wifi_c_status_t* wifi_status = wifi_c_get_status();

  //then
  TEST_ASSERT_EQUAL_MESSAGE(false, wifi_status->sta_connected, "sta_connected value should be false before wifi_c_init");
}

void test_check_initial_ap_connect_handler_value(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

   //given
  wifi_c_status_t* wifi_status = wifi_c_get_status();

  //then
  TEST_ASSERT_NULL_MESSAGE(wifi_status->ap.connect_handler, "ap.connect_handler value should be false before setting it.");
}

void test_check_initial_sta_connect_handler_value(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

   //given
  wifi_c_status_t* wifi_status = wifi_c_get_status();

  //then
  TEST_ASSERT_NULL_MESSAGE(wifi_status->sta.connect_handler, "sta.connect_handler value should be false before setting it.");
}

void test_check_initial_ap_disconnect_handler_value(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

   //given
  wifi_c_status_t* wifi_status = wifi_c_get_status();

  //then
  TEST_ASSERT_NULL_MESSAGE(wifi_status->ap.disconnect_handler, "ap.disconnect_handler value should be NULL before setting it.");
}

void test_check_initial_sta_disconnect_handler_value(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

   //given
  wifi_c_status_t* wifi_status = wifi_c_get_status();

  //then
  TEST_ASSERT_NULL_MESSAGE(wifi_status->sta.disconnect_handler, "sta.disconnect_handler value should be NULL before setting it.");
}

void test_if_wifi_status_is_initalized_when_init_as_ap(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

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
  //before
  LOG_FATAL("RUNNING: %s", __func__);

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
  //before
  LOG_FATAL("RUNNING: %s", __func__);

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
  //before
  LOG_FATAL("RUNNING: %s", __func__);

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
  //before
  LOG_FATAL("RUNNING: %s", __func__);

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
  //before
  LOG_FATAL("RUNNING: %s", __func__);

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
  //before
  LOG_FATAL("RUNNING: %s", __func__);

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
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  wifi_c_status_t* wifi = NULL;

  //when
  wifi_c_init_wifi(WIFI_C_MODE_STA);
  wifi = wifi_c_get_status();
  wifi_c_start_sta("ssid", "pass");

  //then
  TEST_ASSERT_NOT_NULL(wifi);
  TEST_ASSERT_EQUAL_MESSAGE(true, wifi->sta_started, "sta_started should be true after wifi_c_init as STA");

  //after
  afterEach();
}

void test_if_sta_started_is_true_when_init_as_apsta(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  wifi_c_status_t* wifi = NULL;

  //when
  wifi_c_init_wifi(WIFI_C_MODE_APSTA);
  wifi = wifi_c_get_status();
  wifi_c_start_sta("ssid", "pass");

  //then
  TEST_ASSERT_NOT_NULL(wifi);
  TEST_ASSERT_EQUAL_MESSAGE(true, wifi->sta_started, "sta_started should be true after wifi_c_init as APSTA");

  //after
  afterEach();
}

void test_if_wifi_init_throws_err_on_wrong_mode_value(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

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
  //before
  LOG_FATAL("RUNNING: %s", __func__);

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
  //before
  LOG_FATAL("RUNNING: %s", __func__);

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
  //before
  LOG_FATAL("RUNNING: %s", __func__);

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
  //before
  LOG_FATAL("RUNNING: %s", __func__);

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
  //before
  LOG_FATAL("RUNNING: %s", __func__);

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
  //before
  LOG_FATAL("RUNNING: %s", __func__);

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
  //before
  LOG_FATAL("RUNNING: %s", __func__);

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
  //before
  LOG_FATAL("RUNNING: %s", __func__);

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
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  int err = 0;
  wifi_c_scan_result_t* result = NULL;

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
  //before
  LOG_FATAL("RUNNING: %s", __func__);

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
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  int err = 0;
  wifi_c_scan_result_t* result = NULL;

  //when
  wifi_c_deinit();
  err = wifi_c_scan_all_ap(&result);

  //then
  TEST_ASSERT_EQUAL_MESSAGE(WIFI_C_ERR_WIFI_NOT_INIT, err, "wifi_c_scan_all_ap should return err when wifi not init");

  //after
  afterEach();  
}

void test_if_wifi_initialized_is_false_after_deinit(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

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
  //before
  LOG_FATAL("RUNNING: %s", __func__);

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
  //before
  LOG_FATAL("RUNNING: %s", __func__);

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
  //before
  LOG_FATAL("RUNNING: %s", __func__);

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
  //before
  LOG_FATAL("RUNNING: %s", __func__);

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
  //before
  LOG_FATAL("RUNNING: %s", __func__);

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
  //before
  LOG_FATAL("RUNNING: %s", __func__);

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
  //before
  LOG_FATAL("RUNNING: %s", __func__);

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
  //before
  LOG_FATAL("RUNNING: %s", __func__);

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
  //before
  LOG_FATAL("RUNNING: %s", __func__);

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
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  wifi_c_status_t* status = NULL;
  wifi_c_init_wifi(WIFI_C_MODE_APSTA);

  //when
  wifi_c_deinit();
  status = wifi_c_get_status();

  //then
  TEST_ASSERT_EQUAL_STRING_MESSAGE("none", status->sta.ssid, "sta.ssid should be none after deinit");
}

void test_if_wifi_c_scan_all_ap_returns_err_on_wrong_wifi_mode(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  int err = 0;
  wifi_c_scan_result_t* result = NULL;
  wifi_c_deinit();

  //when
  wifi_c_init_wifi(WIFI_C_MODE_AP);
  wifi_c_start_ap("ssid", NULL);
  err = wifi_c_scan_all_ap(&result);

  //then
  TEST_ASSERT_EQUAL_MESSAGE(WIFI_C_ERR_WRONG_MODE, err, "wifi_c_scan_all_ap should return err when wifi mode is AP");

  //after
  afterEach();  
}

void test_if_sta_connect_handler_is_set(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  wifi_c_status_t* status = NULL;
  wifi_c_deinit();

  //when
  wifi_c_sta_register_connect_handler(afterEach);
  status = wifi_c_get_status();

  //then
  TEST_ASSERT_EQUAL_PTR_MESSAGE(afterEach, status->sta.connect_handler, "sta connect handler different from expected");  
}

void test_if_ap_connect_handler_is_set(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  wifi_c_status_t* status = NULL;
  wifi_c_deinit();

  //when
  wifi_c_ap_register_connect_handler(afterEach);
  status = wifi_c_get_status();

  //then
  TEST_ASSERT_EQUAL_PTR_MESSAGE(afterEach, status->ap.connect_handler, "ap connect handler different from expected");  
}

void test_if_ap_connect_handler_register_returns_err_on_null(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  int err = -1;
  wifi_c_deinit();

  //when
  err = wifi_c_ap_register_connect_handler(NULL);

  //then
  TEST_ASSERT_EQUAL_MESSAGE(ERR_NULL_POINTER, err, "ap connect handler register should return err on null handler function");  
}

void test_if_sta_connect_handler_register_returns_err_on_null(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  int err = -1;
  wifi_c_deinit();

  //when
  err = wifi_c_sta_register_connect_handler(NULL);

  //then
  TEST_ASSERT_EQUAL_MESSAGE(ERR_NULL_POINTER, err, "sta connect handler register should return err on null handler function");  
}

void test_if_sta_disconnect_handler_is_set(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  wifi_c_status_t* status = NULL;
  wifi_c_deinit();

  //when
  wifi_c_sta_register_disconnect_handler(afterEach);
  status = wifi_c_get_status();

  //then
  TEST_ASSERT_EQUAL_PTR_MESSAGE(afterEach, status->sta.disconnect_handler, "sta disconnect handler different from expected");  
}

void test_if_ap_disconnect_handler_is_set(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  wifi_c_status_t* status = NULL;
  wifi_c_deinit();

  //when
  wifi_c_ap_register_disconnect_handler(afterEach);
  status = wifi_c_get_status();

  //then
  TEST_ASSERT_EQUAL_PTR_MESSAGE(afterEach, status->ap.disconnect_handler, "ap disconnect handler different from expected");  
}

void test_if_ap_disconnect_handler_register_returns_err_on_null(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  int err = -1;
  wifi_c_deinit();

  //when
  err = wifi_c_ap_register_disconnect_handler(NULL);

  //then
  TEST_ASSERT_EQUAL_MESSAGE(ERR_NULL_POINTER, err, "ap disconnect handler register should return err on null handler function");  
}

void test_if_sta_disconnect_handler_register_returns_err_on_null(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  int err = -1;
  wifi_c_deinit();

  //when
  err = wifi_c_sta_register_disconnect_handler(NULL);

  //then
  TEST_ASSERT_EQUAL_MESSAGE(ERR_NULL_POINTER, err, "sta disconnect handler register should return err on null handler function");  
}

void test_if_get_mode_as_str_works_on_ap_mode(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  char* mode = NULL;

  //when
  mode = wifi_c_get_wifi_mode_as_string(WIFI_C_MODE_AP);

  //then
  TEST_ASSERT_EQUAL_STRING_MESSAGE("WIFI_C_MODE_AP", mode, "translated AP mode different from expected");
}

void test_if_get_mode_as_str_works_on_sta_mode(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  char* mode = NULL;

  //when
  mode = wifi_c_get_wifi_mode_as_string(WIFI_C_MODE_STA);

  //then
  TEST_ASSERT_EQUAL_STRING_MESSAGE("WIFI_C_MODE_STA", mode, "translated STA mode different from expected");
}

void test_if_get_mode_as_str_works_on_apsta_mode(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  char* mode = NULL;

  //when
  mode = wifi_c_get_wifi_mode_as_string(WIFI_C_MODE_APSTA);

  //then
  TEST_ASSERT_EQUAL_STRING_MESSAGE("WIFI_C_MODE_APSTA", mode, "translated AP+STA mode different from expected");
}

void test_if_get_mode_as_str_returns_err_on_wrong_mode(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  char* mode = "not null";

  //when
  mode = wifi_c_get_wifi_mode_as_string(223);

  //then
  TEST_ASSERT_NULL_MESSAGE(mode, "on wrong passed parameter returned translated mode should be NULL");
}

void test_if_wifi_c_ap_mode_has_value_as_esp_ap_mode(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //then
  TEST_ASSERT_EQUAL_MESSAGE(WIFI_MODE_AP, WIFI_C_MODE_AP, "WIFI_C_MODE_AP should have the same value as WIFI_MODE_AP");
}

void test_if_wifi_c_sta_mode_has_value_as_esp_sta_mode(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //then
  TEST_ASSERT_EQUAL_MESSAGE(WIFI_MODE_STA, WIFI_C_MODE_STA, "WIFI_C_MODE_STA should have the same value as WIFI_MODE_STA");
}

void test_if_wifi_c_apsta_mode_has_value_as_esp_apsta_mode(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //then
  TEST_ASSERT_EQUAL_MESSAGE(WIFI_MODE_APSTA, WIFI_C_MODE_APSTA, "WIFI_C_MODE_APSTA should have the same value as WIFI_MODE_APSTA");
}

void test_if_wifi_c_no_mode_has_value_as_esp_no_mode(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //then
  TEST_ASSERT_EQUAL_MESSAGE(WIFI_MODE_NULL, WIFI_C_NO_MODE, "WIFI_C_NO_MODE should have the same value as WIFI_MODE_NULL");
}

void test_if_disconnect_returns_err_on_not_connected_sta(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  int err = -1;
  wifi_c_deinit();

  //when
  err = wifi_c_disconnect();

  //then
  TEST_ASSERT_EQUAL_MESSAGE(WIFI_C_ERR_STA_NOT_CONNECTED, err, "wifi_c_disconnect should return WIFI_C_ERR_STA_NOT_CONNECTED when STA not connected");
}

void test_if_change_mode_changes_mode_from_sta_to_ap(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  wifi_c_deinit();
  wifi_c_status_t* status = NULL;

  //when
  wifi_c_init_wifi(WIFI_C_MODE_STA);
  wifi_c_change_mode(WIFI_C_MODE_AP);
  status = wifi_c_get_status();

  //then
  TEST_ASSERT_EQUAL_MESSAGE(WIFI_C_MODE_AP, status->wifi_mode, "wifi mode not changed, should be AP");

  //after
  wifi_c_deinit();
}

void test_if_change_mode_changes_mode_from_ap_to_sta(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  wifi_c_status_t* status = NULL;
  wifi_c_deinit();

  //when
  wifi_c_init_wifi(WIFI_C_MODE_AP);
  wifi_c_change_mode(WIFI_C_MODE_STA);
  status = wifi_c_get_status();

  //then
  TEST_ASSERT_EQUAL_MESSAGE(WIFI_C_MODE_STA, status->wifi_mode, "wifi mode not changed, should be STA");

  //after
  wifi_c_deinit();
}

void test_if_change_mode_returns_err_on_the_same_mode(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  int err = -1;
  wifi_c_deinit();

  //when
  wifi_c_init_wifi(WIFI_C_MODE_AP);
  err = wifi_c_change_mode(WIFI_C_MODE_AP);

  //then
  TEST_ASSERT_EQUAL_MESSAGE(WIFI_C_ERR_WRONG_MODE, err, "wifi_c_change_mode should return err on the same err");

  //after
  wifi_c_deinit();
}

void test_if_scan_all_ap_returns_err_on_null_result_to_return(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  int err = -1;
  wifi_c_deinit();

  //when
  wifi_c_init_wifi(WIFI_C_MODE_APSTA);
  err = wifi_c_scan_all_ap(NULL);

  //then
  TEST_ASSERT_EQUAL_MESSAGE(ERR_NULL_POINTER, err, "wifi_c_scan_all_ap should return err on null result_to_return");

  //after
  wifi_c_deinit();
}

void test_if_scan_all_ap_returns_err_on_not_init_wifi(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  int err = -1;
  wifi_c_scan_result_t* scan = NULL;
  wifi_c_deinit();

  //when
  err = wifi_c_scan_all_ap(&scan);

  //then
  TEST_ASSERT_EQUAL_MESSAGE(WIFI_C_ERR_WIFI_NOT_INIT, err, "wifi_c_scan_all_ap should return err on not init wifi");

  //after
  wifi_c_deinit();
}

void test_if_scan_all_ap_returns_zero(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);  

  //given
  int err = -1;
  wifi_c_scan_result_t* scan = NULL;
  wifi_c_deinit();

  //when
  wifi_c_init_wifi(WIFI_C_MODE_STA);
  err = wifi_c_scan_all_ap(&scan);

  //then
  TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "wifi_c_scan_all_ap should return zero on success");

  //after
  wifi_c_deinit();
}

void test_if_scan_ssid_ap_returns_err_on_null_result_to_return(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__); 

  //given
  int err = -1;
  wifi_c_deinit();

  //when
  wifi_c_init_wifi(WIFI_C_MODE_APSTA);
  err = wifi_c_scan_for_ap_with_ssid("ssid", NULL);

  //then
  TEST_ASSERT_EQUAL_MESSAGE(ERR_NULL_POINTER, err, "wifi_c_scan_for_ap_with_ssid should return err on null ap_record");

  //after
  wifi_c_deinit();
}

void test_if_scan_ssid_ap_returns_err_on_null_ssid_to_search(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);  

  //given
  int err = -1;
  wifi_c_ap_record_t ap_record;
  wifi_c_deinit();

  //when
  wifi_c_init_wifi(WIFI_C_MODE_APSTA);
  err = wifi_c_scan_for_ap_with_ssid(NULL, &ap_record);

  //then
  TEST_ASSERT_EQUAL_MESSAGE(ERR_NULL_POINTER, err, "wifi_c_scan_for_ap_with_ssid should return err on null ap_record");

  //after
  wifi_c_deinit();
}

void test_if_scan_ssid_ap_returns_err_on_not_init_wifi(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  int err = -1;
  wifi_c_ap_record_t scan;
  wifi_c_deinit();

  //when
  err = wifi_c_scan_for_ap_with_ssid("ssid", &scan);

  //then
  TEST_ASSERT_EQUAL_MESSAGE(WIFI_C_ERR_WIFI_NOT_INIT, err, "wifi_c_scan_for_ap_with_ssid should return err on not init wifi");

  //after
  wifi_c_deinit();
}

void test_if_scan_ssid_ap_returns_not_found_ssid(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  int err = -1;
  wifi_c_ap_record_t scan;
  wifi_c_deinit();

  //when
  wifi_c_init_wifi(WIFI_C_MODE_STA);
  err = wifi_c_scan_for_ap_with_ssid("ssid", &scan);

  //then
  TEST_ASSERT_EQUAL_MESSAGE(WIFI_C_ERR_AP_NOT_FOUND, err, "wifi_c_scan_for_ap_with_ssid should return zero on success");

  //after
  wifi_c_deinit();
}

void test_if_get_status_as_json_returns_err_on_null_buffer(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  int err = 0;

  //when
  err = wifi_c_get_status_as_json(NULL, 0);

  //then
  TEST_ASSERT_EQUAL_MESSAGE(ERR_NULL_POINTER, err, "wifi_c_get_status_as_json should return err on NULL buffer");
}

void test_if_sta_connect_handler_is_null_after_deinit(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  wifi_c_status_t* status = NULL;

  //when
  wifi_c_sta_register_connect_handler(afterEach);
  wifi_c_deinit();
  status = wifi_c_get_status();

  //then
  TEST_ASSERT_NULL_MESSAGE(status->sta.connect_handler, "sta connect handler should be NULL after deinit.");
}

void test_if_ap_connect_handler_is_null_after_deinit(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  wifi_c_status_t* status = NULL;

  //when
  wifi_c_ap_register_connect_handler(afterEach);
  wifi_c_deinit();
  status = wifi_c_get_status();

  //then
  TEST_ASSERT_NULL_MESSAGE(status->ap.connect_handler, "ap connect handler should be NULL after deinit.");
}

void test_if_sta_disconnect_handler_is_null_after_deinit(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);


  //given
  wifi_c_status_t* status = NULL;

  //when
  wifi_c_sta_register_disconnect_handler(afterEach);
  wifi_c_deinit();
  status = wifi_c_get_status();

  //then
  TEST_ASSERT_NULL_MESSAGE(status->sta.disconnect_handler, "sta disconnect handler should be NULL after deinit.");
}

void test_if_ap_disconnect_handler_is_null_after_deinit(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  wifi_c_status_t* status = NULL;

  //when
  wifi_c_ap_register_disconnect_handler(afterEach);
  wifi_c_deinit();
  status = wifi_c_get_status();

  //then
  TEST_ASSERT_NULL_MESSAGE(status->ap.disconnect_handler, "ap disconnect handler should be NULL after deinit.");
}

void test_if_max_ap_buffer_is_bigger_than_ap_scan_count(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  wifi_c_scan_result_t* result = NULL;

  //when
  wifi_c_init_wifi(WIFI_C_MODE_STA);
  wifi_c_scan_all_ap(&result);

  //then
  TEST_ASSERT_TRUE_MESSAGE(result->ap_count <= WIFI_C_BUFFER_SCAN_SIZE, "Number of scanned AP cannot be bigger than max buffer scan size");
}

void run_wifi_controller_tests(void) {
  UNITY_BEGIN();
  RUN_TEST(test_if_max_ap_buffer_is_bigger_than_ap_scan_count);
  RUN_TEST(test_if_ap_disconnect_handler_is_null_after_deinit);
  RUN_TEST(test_if_sta_disconnect_handler_is_null_after_deinit);
  RUN_TEST(test_if_ap_connect_handler_is_null_after_deinit);
  RUN_TEST(test_if_sta_connect_handler_is_null_after_deinit);
  RUN_TEST(test_if_get_status_as_json_returns_err_on_null_buffer);
  RUN_TEST(test_if_scan_ssid_ap_returns_not_found_ssid);
  RUN_TEST(test_if_scan_ssid_ap_returns_err_on_not_init_wifi);
  RUN_TEST(test_if_scan_ssid_ap_returns_err_on_null_ssid_to_search);
  RUN_TEST(test_if_scan_ssid_ap_returns_err_on_null_result_to_return);
  RUN_TEST(test_if_scan_all_ap_returns_zero);
  RUN_TEST(test_if_scan_all_ap_returns_err_on_not_init_wifi);
  RUN_TEST(test_if_scan_all_ap_returns_err_on_null_result_to_return);
  RUN_TEST(test_if_change_mode_returns_err_on_the_same_mode);
  RUN_TEST(test_if_change_mode_changes_mode_from_ap_to_sta);
  RUN_TEST(test_if_change_mode_changes_mode_from_sta_to_ap);
  RUN_TEST(test_if_disconnect_returns_err_on_not_connected_sta);
  RUN_TEST(test_if_wifi_c_no_mode_has_value_as_esp_no_mode);
  RUN_TEST(test_if_wifi_c_apsta_mode_has_value_as_esp_apsta_mode);
  RUN_TEST(test_if_wifi_c_sta_mode_has_value_as_esp_sta_mode);
  RUN_TEST(test_if_wifi_c_ap_mode_has_value_as_esp_ap_mode);
  RUN_TEST(test_if_get_mode_as_str_returns_err_on_wrong_mode);
  RUN_TEST(test_if_get_mode_as_str_works_on_apsta_mode);
  RUN_TEST(test_if_get_mode_as_str_works_on_sta_mode);
  RUN_TEST(test_if_get_mode_as_str_works_on_ap_mode);
  RUN_TEST(test_if_sta_disconnect_handler_register_returns_err_on_null);
  RUN_TEST(test_if_ap_disconnect_handler_register_returns_err_on_null);
  RUN_TEST(test_if_ap_disconnect_handler_is_set);
  RUN_TEST(test_if_sta_disconnect_handler_is_set);
  RUN_TEST(test_if_sta_connect_handler_register_returns_err_on_null);
  RUN_TEST(test_if_ap_connect_handler_register_returns_err_on_null);
  RUN_TEST(test_if_ap_connect_handler_is_set);
  RUN_TEST(test_if_sta_connect_handler_is_set);
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
  RUN_TEST(test_check_initial_ap_disconnect_handler_value);
  RUN_TEST(test_check_initial_sta_disconnect_handler_value);
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