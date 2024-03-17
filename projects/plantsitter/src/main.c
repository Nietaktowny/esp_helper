#include "wifi_controller.h"
#include "memory_utils.h"
#include "err_controller.h"
#include "logger.h"
#include "http_client.h"
#include "wifi_manager.h"
#include "esp_helper_utils.h"
#include "sys_utils.h"
#include "nvs_controller.h"
#include "cli_manager.h"

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <driver/gpio.h>
#include <driver/adc.h>
#include <esp_adc_cal.h>

#define MY_SSID "TP-LINK_AD8313"
#define MY_PSK "20232887"
#define SOL_SSID "OstNet-952235"
#define SOL_PSK "Solonka106"

#ifdef ESP_WROVER_KIT
#define ESP_DEVICE_WIFI_LED GPIO_NUM_2
#define BUS_GPIO_SCL 22
#define BUS_GPIO_SDA 21
#elif ESP_DEV_MODULE
#define ESP_DEVICE_WIFI_LED GPIO_NUM_2
#define BUS_GPIO_SCL 22
#define BUS_GPIO_SDA 21
#elif ESP32_C3_SUPERMINI
#define BUS_GPIO_SCL 9
#define BUS_GPIO_SDA 8
#endif


static esp_adc_cal_characteristics_t adc1_chars;

static bool adc_calibration_init(void)
{
    esp_err_t ret;
    bool cali_enable = false;

    ret = esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF);
    if (ret == ESP_ERR_NOT_SUPPORTED) {
        LOG_WARN("Calibration scheme not supported, skip software calibration");
    } else if (ret == ESP_ERR_INVALID_VERSION) {
        LOG_WARN("eFuse not burnt, skip software calibration");
    } else if (ret == ESP_OK) {
        cali_enable = true;
        esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_DEFAULT, 0, &adc1_chars);
    } else {
        LOG_ERROR("Invalid arg");
    }

    return cali_enable;
}



void read_soil_moisture_task(void *args)
{
    err_c_t err = ERR_C_OK;
    int adc_raw[2][10] = {0};
    uint32_t voltage = 0;
    bool cali_enable = adc_calibration_init();
    err = adc1_config_width(ADC_WIDTH_BIT_DEFAULT);
    if(err != ERR_C_OK) {
        LOG_ERROR("error %d when setting channel ADC1 width: %s", err, error_to_name(err));
    }
     
    err = adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_11);
    if(err != ERR_C_OK) {
        LOG_ERROR("error %d when setting channel attenuation: %s", err, error_to_name(err));
    }
     
    while (1)
    {
        adc_raw[0][0] = adc1_get_raw(ADC1_CHANNEL_1);
        LOG_DEBUG("raw data: %d mV", adc_raw[0][0]);
        if(cali_enable) {
            voltage = esp_adc_cal_raw_to_voltage(adc_raw[0][0], &adc1_chars);
            LOG_INFO("calibrated data: %d mV", voltage);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void update_wifi_info_task(void *args)
{
    err_c_t err = ERR_C_OK;
    char wifi_c_info[300] = {0};
    char device_info[350] = {0};
    char device_id[20] = {0};

    sysutil_get_chip_base_mac_as_str(device_id, sizeof(device_id));

    while (1)
    {

        err = wifi_c_get_status_as_json(wifi_c_info, 300);

        snprintf(device_info, 350, "device_id=%s&wifi_info=%s", device_id, wifi_c_info);
        err = http_client_post("wmytych.usermd.net", "modules/setters/update_wifi_info.php", device_info, HTTP_CLIENT_POST_USE_STRLEN);
        if (err != ERR_C_OK)
        {
            LOG_ERROR("error %d when posting device info data: %s", error_to_name(err));
            memutil_zero_memory(wifi_c_info, sizeof(wifi_c_info));
            memutil_zero_memory(device_info, sizeof(device_info));
            vTaskDelay(pdMS_TO_TICKS(60000 * 3));
            continue;
        }

        LOG_VERBOSE("sent wifi_c_status to the server");
        vTaskDelay(pdMS_TO_TICKS(60000 * 3)); /** @todo Make it a constant or configuration variable? */
    }
}

void inspect_heap_task(void *args)
{
    while (1)
    {
        uint32_t free_heap = esp_get_free_heap_size();
        uint32_t ever_free_heap = esp_get_minimum_free_heap_size();
        LOG_DEBUG("Currently available heap: %lu", free_heap);
        LOG_DEBUG("The minimum heap size that was ever available: %lu", ever_free_heap);

        if (free_heap < 8000)
        {
            LOG_ERROR("Currently free heap very low, restarting...");
            esp_restart();
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void on_connect_handler(void)
{
#ifndef ESP32_C3_SUPERMINI
    gpio_set_direction(ESP_DEVICE_WIFI_LED, GPIO_MODE_OUTPUT);
    gpio_set_level(ESP_DEVICE_WIFI_LED, 1);
    LOG_INFO("Onboard LED turned on!");
#endif

    helper_perform_ota();

    sysutil_setup_ntp_server("pool.ntp.org", 1);

    xTaskCreate(inspect_heap_task, "inspect_task", 1024 * 2, NULL, 4, NULL);
    xTaskCreate(read_soil_moisture_task, "read_moisture_task", 1024*3, NULL, 2, NULL);
    xTaskCreate(update_wifi_info_task, "inspect_heap_task", 1024 * 6, NULL, 3, NULL);
}

#ifdef ESP_WROVER_KIT
void switch_off_all_leds(void)
{
    // switch off all LEDs
    gpio_set_direction(GPIO_NUM_0, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_0, 0);
    gpio_set_direction(GPIO_NUM_4, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_4, 0);
    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_2, 0);
}
#endif

void app_main()
{

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
