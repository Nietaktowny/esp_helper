#include "unity.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include "esp_err.h"

#include "nvs_flash.h"

#include "wifi_controller.h"

void setUp(void) {
    int err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
}

void tearDown(void) {
  // clean stuff up here
}

void afterEach (void) {
  //wifi_c_deinit();
  vTaskDelay(300);
  ESP_ERROR_CHECK(wifi_c_deinit());
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

void test_check_if_wifi_status_is_initialized(void) {
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

void run_wifi_controller_tests(void) {
  UNITY_BEGIN();
  //RUN_TEST(test_check_if_wifi_status_is_initialized);
  RUN_TEST(test_check_if_netif_status_is_initialized);
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