#include "unity.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

// Change fatal color to purple, to use it to log test names.
#include "ansi_colors.h"
#define FATAL_COLOR PURPLE_BOLD

#include "cli_manager.h"
#include "err_c_errors.h"
#include "logger.h"
#include "wifi_controller.h"
#include "nvs_controller.h"
#include "tcp_controller.h"
#include "tcp_c_errors.h"

#define MY_SSID "TP-LINK_AD8313"
#define MY_PSK "20232887"
#define SOL_SSID "OstNet-952235"
#define SOL_PSK "Solonka106"
#define MI_SSID "Mi"
#define MI_PSK "Solonka106"

void setUp(void)
{
  // set stuff up here
  logger_init();
  logger_set_log_level(LOG_LEVEL_VERBOSE);
}

void tearDown(void)
{
  // clean stuff up here
}

//Used only for testing.
int test_prepare_listen_socket(void)
{
  socket_t listen = INVALID_SOCKET;
  struct sockaddr_in addr = {0};

  tcp_create_socket(&listen);
  tcp_prepare_address(9215, wifi_c_get_sta_ipv4(), &addr);
  tcp_bind_socket(listen, &addr);
  tcp_socket_listen(listen, 1);

  return listen;
}

void test_if_cli_set_remote_log_returns_err_on_null_address(void)
{
  // before
  LOG_FATAL("RUNNING: %s", __func__);

  // given
  int err = -1;

  // when
  err = cli_set_remote_logging(27015, NULL);

  // then
  TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "cli_set_remote_logging should return err on NULL address");
}

void test_if_cli_set_remote_log_returns_err_on_reserved_port(void)
{
  // before
  LOG_FATAL("RUNNING: %s", __func__);

  // given
  int err = -1;
  uint16_t reserved = 256;

  // when
  err = cli_set_remote_logging(reserved, NULL);

  // then
  TEST_ASSERT_NOT_EQUAL_MESSAGE(0, err, "cli_set_remote_logging should return err on reserved port.");
}

void test_if_cli_set_remote_log_returns_err_on_no_internet_address(void)
{
  // before
  LOG_FATAL("RUNNING: %s", __func__);

  // given
  int err = -1;
  const char *no_address = "0.0.0.0";

  // when
  err = cli_set_remote_logging(27015, no_address);

  // then
  TEST_ASSERT_EQUAL_MESSAGE(CLI_ERR_NO_INTERNET, err, "cli_set_remote_logging should return CLI_ERR_NO_INTERNET on 0.0.0.0 address");
}

void test_if_cli_set_remote_log_returns_zero(void)
{
  // before
  LOG_FATAL("RUNNING: %s", __func__);

  // given
  int err = -1;
  const char *address = wifi_c_get_sta_ipv4();

  // when
  err = cli_set_remote_logging(27015, address);

  // then
  TEST_ASSERT_EQUAL_MESSAGE(0, err, "cli_set_remote_logging should return zero");
}

void test_if_cli_accept_task_deletes_itself_on_invalid_socket(void)
{
  // before
  LOG_FATAL("RUNNING: %s", __func__);

  // given
  int invalid_socket = INVALID_SOCKET;
  TaskHandle_t task = NULL;
  eTaskState state = eReady;

  // when
  xTaskCreate(cli_accept_task, "cli_task", 4096, (void *)invalid_socket, 2, &task);
  state = eTaskGetState(task);

  // then
  TEST_ASSERT_EQUAL_MESSAGE(eDeleted, state, "cli_accept_task should delete itself when passed invalid socket");
}

void test_if_cli_accept_task_runs(void)
{
  // before
  LOG_FATAL("RUNNING: %s", __func__);

  // given
  int listen = test_prepare_listen_socket();
  TaskHandle_t task = NULL;
  eTaskState state = eReady;

  // when
  xTaskCreate(cli_accept_task, "cli_task", 4096, (void *)listen, 2, &task);
  // leave some time, so that task can be created
  vTaskDelay(pdMS_TO_TICKS(200));
  state = eTaskGetState(task);

  // then
  TEST_ASSERT_EQUAL_MESSAGE(eBlocked, state, "cli_accept_task should exist.");

  //after
  vTaskDelete(task);
}

int runUnityTests(void)
{
  UNITY_BEGIN();
  RUN_TEST(test_if_cli_accept_task_runs);
  RUN_TEST(test_if_cli_accept_task_deletes_itself_on_invalid_socket);
  RUN_TEST(test_if_cli_set_remote_log_returns_zero);
  RUN_TEST(test_if_cli_set_remote_log_returns_err_on_no_internet_address);
  RUN_TEST(test_if_cli_set_remote_log_returns_err_on_reserved_port);
  RUN_TEST(test_if_cli_set_remote_log_returns_err_on_null_address);
  return UNITY_END();
}

/**
 * For ESP-IDF framework
 */
void app_main(void)
{
  nvs_c_init_nvs();
  wifi_c_init_wifi(WIFI_C_MODE_STA);
  wifi_c_start_sta(MY_SSID, MY_PSK);
  runUnityTests();
  vTaskDelay(200);
  fflush(stdout);
  esp_restart();
}