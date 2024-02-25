#include "unity.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

//Change fatal color to purple, to use it to log test names.
#include "ansi_colors.h"
#define FATAL_COLOR PURPLE_BOLD

#include "http_client.h"
#include "logger.h"
#include "errors_list.h"
#include "wifi_controller.h"
#include "nvs_controller.h"

#define MY_SSID "TP-LINK_AD8313"
#define MY_PSK "20232887"
#define SOL_SSID "OstNet-952235"
#define SOL_PSK "Solonka106"
#define MI_SSID "Mi"
#define MI_PSK  "Solonka106"

void setUp(void) {
  // set stuff up here
  LOG_FATAL("RUNNING SETUP FOR TESTS");
  logger_init();
  logger_set_log_level(LOG_LEVEL_VERBOSE);
}

void tearDown(void) {
  // clean stuff up here
  LOG_FATAL("RUNNING TEARDOWN AFTER TESTS");
  wifi_c_deinit();
}


//Don't run this test without internet connection.
#define ABORT_IF_STA_CONNECTED_STATUS(status)  \
    wifi_c_status_t* wifi = wifi_c_get_status();      \
    if(wifi->sta_connected == status) {                \
      TEST_ABORT();                                   \
    }


void test_if_abort_if_not_connected_works(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  ABORT_IF_STA_CONNECTED_STATUS(false);

  TEST_FAIL(); // if it goes to here, then test was not aborted and macro doesn't work
}


void test_if_http_max_url_len_is_defined(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //then
    #ifdef HTTP_CLIENT_MAX_URL_LENGTH
    TEST_PASS();
    #elif
    TEST_FAIL();
    #endif
}

void test_if_http_max_output_buffer_is_defined(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //then
    #ifdef HTTP_CLIENT_MAX_OUTPUT_BUFFER_SIZE
    TEST_PASS();
    #elif
    TEST_FAIL();
    #endif
}

void test_if_http_get_returns_err_on_null_address(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = -1;
    const char* path = "/";
    char resp[250] = {0};

    //when
    err = http_client_get(NULL, path, resp, sizeof(resp));

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_NULL_POINTER, err, "http_client_get should return err on NULL address");
}

void test_if_http_get_returns_err_on_null_path(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = -1;
    const char* address = "google.com";
    char resp[250] = {0};

    //when
    err = http_client_get(address, NULL, resp, sizeof(resp));

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_NULL_POINTER, err, "http_client_get should return err on NULL path");
}

void test_if_http_get_returns_err_on_null_buffer(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = -1;
    const char* address = "google.com";
    const char* path = "/";

    //when
    err = http_client_get(address, path, NULL, 123);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_NULL_POINTER, err, "http_client_get should return err on NULL buffer");
}

void test_if_http_post_returns_err_on_null_address(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = -1;
    const char* path = "/";
    char req[250] = {0};

    //when
    err = http_client_post(NULL, path, req, sizeof(req));

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_NULL_POINTER, err, "http_client_post should return err on NULL address");
}

void test_if_http_post_returns_err_on_null_path(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = -1;
    const char* address = "google.com";
    char req[250] = {0};

    //when
    err = http_client_post(address, NULL, req, sizeof(req));

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_NULL_POINTER, err, "http_client_post should return err on NULL path");
}

void test_if_http_post_returns_err_on_null_buffer(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = -1;
    const char* address = "google.com";
    const char* path = "/";

    //when
    err = http_client_post(address, path, NULL, 123);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_NULL_POINTER, err, "http_client_post should return err on NULL buffer");
}

void test_if_http_post_use_strlen_is_defined(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //then
    #ifdef HTTP_CLIENT_POST_USE_STRLEN
    TEST_PASS();
    #elif
    TEST_FAIL();
    #endif
}

void test_if_http_get_returns_expected_response(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);
    ABORT_IF_STA_CONNECTED_STATUS(false);

    //given
    const char* address = "wmytych.usermd.net";
    const char* path = "modules/getters/get_gpio_state.php?device_id=222222";
    char* resp = malloc(1025);
    const char* const expected_resp = "[{\"gpio\":22,\"state\":1,\"gpio_alias\":\"Relay\"},{\"gpio\":2,\"state\":1,\"gpio_alias\":\"Onboard LED\"}]";

    //when
    http_client_get(address, path, resp, 1024);

    //then
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_resp, resp, "http_client_get response different from expected");
}

void test_if_http_get_returns_zero_on_success(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);
    ABORT_IF_STA_CONNECTED_STATUS(false);

    //given
    int err = -1;
    const char* address = "wmytych.usermd.net";
    const char* path = "modules/getters/get_gpio_state.php?device_id=222222";
    char* resp = malloc(1025);

    //when
    err = http_client_get(address, path, resp, 1024);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(0, err, "http_client_get should return 0");
}

void test_if_http_post_returns_zero_on_success(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);
    ABORT_IF_STA_CONNECTED_STATUS(false);

    //given
    int err = -1;
    const char* address = "wmytych.usermd.net";
    const char* path = "modules/getters/get_gpio_state.php";
    const char* req = "device_id=222222";

    //when
    err = http_client_post(address, path, req, HTTP_CLIENT_POST_USE_STRLEN);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(0, err, "http_client_post should return 0");
}

int runUnityTests(void) {
  UNITY_BEGIN();
  RUN_TEST(test_if_http_post_returns_zero_on_success);
  RUN_TEST(test_if_http_get_returns_zero_on_success);
  RUN_TEST(test_if_http_get_returns_expected_response);
  RUN_TEST(test_if_http_post_use_strlen_is_defined);
  RUN_TEST(test_if_http_post_returns_err_on_null_buffer);
  RUN_TEST(test_if_http_post_returns_err_on_null_path);
  RUN_TEST(test_if_http_post_returns_err_on_null_address);
  RUN_TEST(test_if_http_get_returns_err_on_null_buffer);
  RUN_TEST(test_if_http_get_returns_err_on_null_path);
  RUN_TEST(test_if_http_get_returns_err_on_null_address);
  RUN_TEST(test_if_http_max_url_len_is_defined);
  RUN_TEST(test_if_http_max_output_buffer_is_defined);
  RUN_TEST(test_if_abort_if_not_connected_works);
  return UNITY_END();
}


/**
  * For ESP-IDF framework
  */
void app_main(void) {
  nvs_c_init_nvs();
  wifi_c_init_wifi(WIFI_C_MODE_STA);
  wifi_c_start_sta(MY_SSID, MY_PSK);
  runUnityTests();
  vTaskDelay(200);
  fflush(stdout);
  esp_restart();
}