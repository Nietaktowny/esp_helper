#ifdef ESP_PLATFORM
#include <driver/gpio.h>
#include <string.h>

#include "cJSON.h"
#include "cli_manager.h"
#include "err_controller.h"
#include "esp_netif_sntp.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "http_client.h"
#include "logger.h"
#include "memory_utils.h"
#include "nvs_controller.h"
#include "ota_controller.h"
#include "sys_utils.h"
#include "wifi_controller.h"
#include "wifi_manager.h"

void task(void *args) {
  while (1) {
    sysutil_get_system_event_free_stack_size();
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

void app_main() {
  // Allow other core to finish initialization
  vTaskDelay(pdMS_TO_TICKS(100));

  // Create semaphores to synchronize logs
  logger_init();
  logger_set_log_level(LOG_LEVEL_VERBOSE);

  // test sys_utils
  xTaskCreate(task, "task", 4096, NULL, 3, NULL);
}

#else
#include "err_controller.h"
#include "logger.h"

int main() {
  logger_init();
  LOG_DEBUG("Hello world!");
  int err = 2134;
  ERR_C_CHECK_ERROR(err, LOG_ERROR("%s", error_to_name(err)));
  return 0;
}
#endif
