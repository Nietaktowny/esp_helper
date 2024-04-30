#include "adc_controller.h"
#include "cli_manager.h"
#include "err_c_errors.h"
#include "err_controller.h"
#include "esp_helper_utils.h"
#include "freertos/projdefs.h"
#include "http_client.h"
#include "logger.h"
#include "memory_utils.h"
#include "nvs_controller.h"
#include "sys_utils.h"
#include "wifi_controller.h"
#include "wifi_manager.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <driver/gpio.h>

#ifdef ESP_WROVER_KIT
#define ESP_DEVICE_WIFI_LED GPIO_NUM_2
#define SOIL_ADC_READ_GPIO ADC_C_GPIO_32
#elif ESP_DEV_MODULE
#define ESP_DEVICE_WIFI_LED GPIO_NUM_2
#define SOIL_ADC_READ_GPIO ADC_C_GPIO_2
#elif ESP32_C3_SUPERMINI
#define SOIL_ADC_READ_GPIO ADC_C_GPIO_4
#endif

///< ADC read value when sensor is put into water.
#define SOIL_MOISTURE_WET 900
///< ADC read value when sensor is in the air.
#define SOIL_MOISTURE_DRY 2300
///< Time of delay before next reading soil moisture value.
#define SOIL_MOISTURE_DELAY_TIME 60000
///< Number of times soil moisture will be read, before calculating average
///< value and sending it to database.
#define SOIL_MOISTURE_READINGS_NUMBER 5
///< Server address
#define SERVER_ADDRESS "wmytych.usermd.net"
///< Path to script in server that will process SOIL INPUT request
#define SERVER_READINGS_INPUT_ACTION "modules/setters/add_moisture_read.php"
///< Path to script in server that will process WIFI INFO INPUT request
#define SERVER_WIFI_INFO_INPUT_ACTION "modules/setters/update_wifi_info.php"

long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// This function will read soil moisture each iteration with delay between
// iterations specified by SOIL_MOISTURE_DELAY_TIME After that average value
// from number of readings will be calculated and send to the database. So time
// between each HTTP POST request to database is: number of read values * delay
// between each read = SOIL_MOISTURE_READINGS_NUMBER * SOIL_MOISTURE_DELAY_TIME
// Due to long time between requests, connection is not keep opened between
// requests.
void read_soil_moisture_task(void *args) {
  err_c_t err = ERR_C_OK;
  int voltage = 0;
  adc_c_oneshot_handle_t adc = NULL;
  char post_data[256] = {0};
  char device_id[64] = {0};
  uint8_t iteration = 0;
  long moisture[SOIL_MOISTURE_READINGS_NUMBER] = {0};
  long moisture_avg = 0;

  err = sysutil_get_chip_base_mac_as_str(device_id, sizeof(device_id));
  if (err != ERR_C_OK) {
    LOG_ERROR("error %d, cannot obtain device_id: %s", err, error_to_name(err));
    vTaskDelete(NULL);
  }
  LOG_VERBOSE("obtained device_id=%s", device_id);

  err = adc_c_init_oneshot(&adc, SOIL_ADC_READ_GPIO);
  if (err != ERR_C_OK) {
    LOG_ERROR("error %d when trying to init ADC Oneshot Controller, "
              "read_soil_moisture_task cannot continue: %s",
              err, error_to_name(err));
    vTaskDelete(NULL);
  }

  while (1) {
    err = adc_c_oneshot_read_gpio(adc, SOIL_ADC_READ_GPIO, &voltage);
    if (err != ERR_C_OK) {
      LOG_WARN("error %d during reading soil reading moisture: %s", err,
               error_to_name(err));
      continue;
    }

    // calculate moisture and map it to percentage
    moisture[iteration] =
        map(voltage, SOIL_MOISTURE_DRY, SOIL_MOISTURE_WET, 0, 100);
    LOG_INFO("calculated soil moisture: %ld%%", moisture[iteration]);

    if (iteration < SOIL_MOISTURE_READINGS_NUMBER) {
      iteration++;
      LOG_VERBOSE("number of collected soil moisture values: %d",
                  iteration + 1);
      vTaskDelay(pdMS_TO_TICKS(SOIL_MOISTURE_DELAY_TIME));
      continue;
    }
    // sum all values - use for loop, because it needs to be able to cope with
    // changing number of readings to do
    for (uint8_t i = 0; i < SOIL_MOISTURE_READINGS_NUMBER; i++) {
      moisture_avg += moisture[i];
      LOG_VERBOSE("read value number %u is: %ld", i, moisture[i]);
    }
    moisture_avg = moisture_avg / SOIL_MOISTURE_READINGS_NUMBER;
    LOG_DEBUG("average soil moisture from %d read values is: %ld", iteration,
              moisture_avg);

    // reset collected moisture values counter
    iteration = 0;

    err = sysutil_get_chip_base_mac_as_str(device_id, sizeof(device_id));
    if (err != ERR_C_OK) {
      LOG_ERROR("error %d, cannot obtain device_id: %s", err,
                error_to_name(err));
      vTaskDelete(NULL);
    }

    // here we got calculated average soil moisture, send it to database
    snprintf(post_data, sizeof(post_data), "device_id=%s&moisture=%ld",
             device_id, moisture_avg);
    LOG_VERBOSE("data to send to database: %s", post_data);
    err = http_client_post(SERVER_ADDRESS, SERVER_READINGS_INPUT_ACTION,
                           post_data, HTTP_CLIENT_POST_USE_STRLEN);
    if (err != ERR_C_OK) {
      LOG_ERROR("client POST request returned error %d: %s", err,
                error_to_name(err));
      LOG_WARN("trying again to make POST request...");
      err = http_client_post(SERVER_ADDRESS, SERVER_READINGS_INPUT_ACTION,
                             post_data, HTTP_CLIENT_POST_USE_STRLEN);
      if (err != ERR_C_OK) {
        LOG_ERROR("error %d when trying to post moisture data to database: %s",
                  err, error_to_name(err));
        vTaskDelay(pdMS_TO_TICKS(SOIL_MOISTURE_DELAY_TIME));
        continue;
      }
    }
    LOG_VERBOSE("Client POST request returned: %d", err);
    vTaskDelay(pdMS_TO_TICKS(SOIL_MOISTURE_DELAY_TIME));
  }
  // if ever task got here, delete it
  adc_c_deinit_oneshot(&adc);
  memutil_zero_memory(post_data, sizeof(post_data));
  memutil_zero_memory(device_id, sizeof(device_id));
  vTaskDelete(NULL);
}

void update_wifi_info_task(void *args) {
  err_c_t err = 0;
  char wifi_c_info[300] = {0};
  char device_info[350] = {0};
  char device_id[20] = {0};

  sysutil_get_chip_base_mac_as_str(device_id, sizeof(device_id));

  while (1) {

    err = wifi_c_get_status_as_json(wifi_c_info, 300);

    snprintf(device_info, 350, "device_id=%s&wifi_info=%s", device_id,
             wifi_c_info);
    err = http_client_post(SERVER_ADDRESS, SERVER_WIFI_INFO_INPUT_ACTION,
                           device_info, HTTP_CLIENT_POST_USE_STRLEN);
    if (err != ERR_C_OK) {
      LOG_ERROR("error %d when posting device info data: %s",
                error_to_name(err));
      memutil_zero_memory(wifi_c_info, sizeof(wifi_c_info));
      memutil_zero_memory(device_info, sizeof(device_info));
      vTaskDelay(pdMS_TO_TICKS(60000 * 3));
      continue;
    }

    LOG_VERBOSE("sent wifi_c_status to the server");
    vTaskDelay(pdMS_TO_TICKS(
        60000 * 3)); /** @todo Make it a constant or configuration variable? */
  }
}

void inspect_heap_task(void *args) {
  while (1) {
    uint32_t free_heap = esp_get_free_heap_size();
    uint32_t ever_free_heap = esp_get_minimum_free_heap_size();
    LOG_VERBOSE("Currently available heap: %lu", free_heap);
    LOG_VERBOSE("The minimum heap size that was ever available: %lu",
                ever_free_heap);

    if (free_heap < 8000) {
      LOG_ERROR("Currently free heap very low, restarting...");
      esp_restart();
    }
    sysutil_get_system_event_free_stack_size();
    vTaskDelay(pdMS_TO_TICKS(60000));
  }
}

void on_connect_handler(void) {
#ifndef ESP32_C3_SUPERMINI
  gpio_set_direction(ESP_DEVICE_WIFI_LED, GPIO_MODE_OUTPUT);
  gpio_set_level(ESP_DEVICE_WIFI_LED, 1);
  LOG_INFO("Onboard LED turned on!");
#endif

  //   helper_perform_ota();

  sysutil_setup_ntp_server("pool.ntp.org", 1);

  xTaskCreate(inspect_heap_task, "inspect_task", 1024 * 5, NULL, 4, NULL);
  xTaskCreate(read_soil_moisture_task, "read_moisture_task", 1024 * 6, NULL, 2,
              NULL);
  xTaskCreate(update_wifi_info_task, "inspect_heap_task", 1024 * 6, NULL, 3,
              NULL);
}

#ifdef ESP_WROVER_KIT
void switch_off_all_leds(void) {
  // switch off all LEDs
  gpio_set_direction(GPIO_NUM_0, GPIO_MODE_OUTPUT);
  gpio_set_level(GPIO_NUM_0, 0);
  gpio_set_direction(GPIO_NUM_4, GPIO_MODE_OUTPUT);
  gpio_set_level(GPIO_NUM_4, 0);
  gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
  gpio_set_level(GPIO_NUM_2, 0);
}
#endif

void app_main() {

  // Allow other core to finish initialization
  vTaskDelay(pdMS_TO_TICKS(100));

  // Create semaphores to synchronize logs
  logger_init();
  logger_set_log_level(LOG_LEVEL_VERBOSE);

  // Initialize NVS
  nvs_c_init_default_partition();

  char device_id[20] = {0};
  sysutil_get_chip_base_mac_as_str(device_id, sizeof(device_id));
  LOG_DEBUG("this device ID is: %s", device_id);

// ESP-WROVE-KIT has 3 LEDs, turn all off
#ifdef ESP_WROVER_KIT
  switch_off_all_leds();
#endif

  wifi_c_sta_register_connect_handler(on_connect_handler);

  wifi_manager_init();

  cli_set_remote_logging(27015, wifi_c_get_sta_ipv4());
}
