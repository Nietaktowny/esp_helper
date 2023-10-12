#include "cli_manager.h"
#include "nvs_controller.h"
#include "wifi_controller.h"
#include <string.h>
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"

const char* TAG = "main";

#define MY_SSID "TP-LINK_AD8313"
#define MY_PSK "20232887"

static char SSID[64];
static char PASSWORD[64];

static void cli_do_scan(void* args) {
    wifi_c_scan_result_t scan_result;
    char send_buffer[512];
    char temp_buf[312];
    wifi_c_scan_all_ap(&scan_result);

    memset(send_buffer, 0, sizeof(send_buffer));
    memset(temp_buf, 0, sizeof(temp_buf));

    strcat(&send_buffer[0], "Scanned APs:");
    wifi_c_store_scanned_ap(temp_buf, 312);

    strncat(&send_buffer[12], &temp_buf[0], strlen(temp_buf));

    cli_manager_write_to_send_buf(send_buffer, strlen(send_buffer));
    cli_manager_end_transmission();
}

void set_sta_password(void* args) {
    int len = (int) strlen((char*)args);

    memset(&PASSWORD, 0, sizeof(PASSWORD));
    memcpy(&PASSWORD, args, len);

    ESP_LOGI(TAG, "Set STA password to: %s", PASSWORD);
    cli_manager_log("Set STA password.");
    cli_manager_add_log(PASSWORD);
    cli_manager_end_transmission();
}

void set_sta_ssid(void* args) {
    int len = (int) strlen((char*)args);

    memset(&SSID, 0, sizeof(SSID));
    memcpy(&SSID, args, len);

    ESP_LOGI(TAG, "Set STA SSID to: %s", SSID);

    cli_manager_log("Set STA SSID.");
    cli_manager_add_log(SSID);
    cli_manager_end_transmission();
}

void do_connect (void* args) {
    cli_manager_log("Restarting");
    cli_manager_end_transmission();
    ESP_LOGI(TAG, "Prepared SSID: %s", SSID);
    ESP_LOGI(TAG, "Prepared password: %s", PASSWORD);

    if(wifi_c_check_if_sta_is_connected()) {
        wifi_c_sta_reconnect(SSID, PASSWORD);
    } else {
        wifi_c_start_sta(SSID, PASSWORD);
    }
}

void wifi_set_wifi_mode(void* args) {
    char* mode = (char*) args;

    if(strncmp(mode, "STA", 3)) {
        cli_manager_log("Setting wifi mode to STA");
        cli_manager_end_transmission();
        vTaskDelay(pdMS_TO_TICKS(300));

        wifi_c_init_wifi(WIFI_C_MODE_STA);
    } else {
        cli_manager_log("Wrong wifi mode.");
        cli_manager_end_transmission();
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}

void sta_get_wifi_status(void* args) {
    wifi_c_status_t* wifi_status = wifi_c_get_status();
    if(wifi_status->sta_connected) {
        cli_manager_log("STA connected to AP:");
        cli_manager_add_log(SSID);
    } else {
        cli_manager_log("STA not connected.");
    }

    cli_manager_end_transmission();
}

void wifi_disconnect_handler(void) {
    ESP_LOGI(TAG, "Lost WiFi");
}

void app_main(void)
{
    // Initialize NVS
    nvs_c_init_nvs();

    //Start client manager

    cli_manager_init();

    cli_manager_register_command('1', '1', cli_do_scan);
    cli_manager_register_command('1', '2', set_sta_ssid);
    cli_manager_register_command('1', '3', set_sta_password);
    cli_manager_register_command('1', '4', sta_get_wifi_status);
    cli_manager_register_command('1', '5', do_connect);


   /*
    wifi_c_init_wifi(WIFI_C_MODE_STA);

    wifi_c_start_sta(MY_SSID, MY_PSK);
    wifi_c_scan_result_t* scan_result = 2;
    wifi_c_scan_all_ap(scan_result);
    vTaskDelay(pdMS_TO_TICKS(1000));
    wifi_c_print_scanned_ap();
    */
}