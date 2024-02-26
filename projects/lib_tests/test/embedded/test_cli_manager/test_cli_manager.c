#include "unity.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

//Change fatal color to purple, to use it to log test names.
#include "ansi_colors.h"
#define FATAL_COLOR PURPLE_BOLD

#include "cli_manager.h"
#include "errors_list.h"
#include "logger.h"


void setUp(void) {
  // set stuff up here
  logger_init();
  logger_set_log_level(LOG_LEVEL_VERBOSE);
}

void tearDown(void) {
  // clean stuff up here
}

void test_if_cli_set_remote_log_returns_err_on_null_address(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = -1;

    //when
    err = cli_set_remote_logging(27015, NULL);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_NULL_POINTER, err, "cli_set_remote_logging should return err on NULL address");
}

void test_if_cli_set_remote_log_returns_err_on_reserved_port(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = -1;
    uint16_t reserved = 256;

    //when
    err = cli_set_remote_logging(reserved, NULL);

    //then
    TEST_ASSERT_NOT_EQUAL_MESSAGE(0, err, "cli_set_remote_logging should return err on reserved port.");
}

void test_if_cli_set_remote_log_returns_err_on_no_internet_address(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = -1;
    const char* no_address = "0.0.0.0";

    //when
    err = cli_set_remote_logging(27015, no_address);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(CLI_ERR_NO_INTERNET, err, "cli_set_remote_logging should return CLI_ERR_NO_INTERNET on 0.0.0.0 address");
}

int runUnityTests(void) {
  UNITY_BEGIN();
  RUN_TEST(test_if_cli_set_remote_log_returns_err_on_no_internet_address);
  RUN_TEST(test_if_cli_set_remote_log_returns_err_on_reserved_port);
  RUN_TEST(test_if_cli_set_remote_log_returns_err_on_null_address);
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