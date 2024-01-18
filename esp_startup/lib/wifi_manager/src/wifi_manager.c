#include "wifi_manager.h"
#include "logger.h"
#include "err_controller.h"
#include "errors_list.h"
#include "wifi_http_server.h"
#include "wifi_controller.h"
#include "memory_utils.h"
#include "nvs_controller.h"

#include <string.h>


#define WIFI_MANAGER_NVS_NAMESPACE "nvs"

int wifi_manager_store_ap(char* ssid, size_t ssid_len, char* password, size_t password_len) {
    err_c_t err = 0;
    nvs_c_handle_t nvs = NULL;
    char namespace[15] = WIFI_MANAGER_NVS_NAMESPACE;

    err = nvs_c_open(&nvs, &namespace[0]);
    if(err != ERR_C_OK) {
        LOG_ERROR("error %d, wifi manager could not open %s NVS namespace: %s", err, WIFI_MANAGER_NVS_NAMESPACE, error_to_name(err));
        return err;
    }

    err = nvs_c_write_string(nvs, "ssid", &ssid[0]);
    if(err != ERR_C_OK) {
        LOG_ERROR("error %d, wifi manager could not write SSID to %s NVS namespace: %s", err, WIFI_MANAGER_NVS_NAMESPACE, error_to_name(err));
        return err;
    }
    
    err = nvs_c_write_string(nvs, "password", &password[0]);
    if(err != ERR_C_OK) {
        LOG_ERROR("error %d, wifi manager could not write PASSWORD to %s NVS namespace: %s", err, WIFI_MANAGER_NVS_NAMESPACE, error_to_name(err));
        return err;
    }

    nvs_c_close(nvs);

    LOG_DEBUG("stored AP SSID and password in NVS:\nSSID: %s\nPassword: %s", ssid, password);
    return err;
}

int wifi_manager_get_stored_ap(char* ssid, size_t ssid_len, char* password, size_t password_len) {
    err_c_t err = 0;
    nvs_c_handle_t nvs = NULL;
    char namespace[15] = WIFI_MANAGER_NVS_NAMESPACE;

    err = nvs_c_open_read_only(&nvs, &namespace[0]);
    if(err != ERR_C_OK) {
        LOG_ERROR("error %d, wifi manager could not open %s NVS namespace: %s", err, WIFI_MANAGER_NVS_NAMESPACE, error_to_name(err));
        return err;
    }

    err = nvs_c_read_string(nvs, "ssid", ssid, ssid_len);
    if(err != ERR_C_OK) {
        LOG_ERROR("error %d, wifi manager could not read SSID from %s NVS namespace: %s", err, WIFI_MANAGER_NVS_NAMESPACE, error_to_name(err));
        return err;
    }
    
    err = nvs_c_read_string(nvs, "password", password, password_len);
    if(err != ERR_C_OK) {
        LOG_ERROR("error %d, wifi manager could not read PASSWORD from %s NVS namespace: %s", err, WIFI_MANAGER_NVS_NAMESPACE, error_to_name(err));
        return err;
    }

    nvs_c_close(nvs);

    LOG_DEBUG("found stored AP SSID and password in NVS:\nSSID: %s,\nPassword: %s", ssid, password);
    return err;
}

int wifi_manager_fetch_ap_list(char* out_buffer, size_t buflen) {
    err_c_t err = 0;
    wifi_c_scan_result_t scan_results;
    ERR_C_CHECK_NULL_PTR(out_buffer, LOG_ERROR("location to store scanned APs list cannot be NULL"));

    err = wifi_c_scan_all_ap(&scan_results);
    if(err != ERR_C_OK) {
        LOG_ERROR("error %d when scanning APs: %s", err, error_to_name(err));
        return err;
    }

    wifi_c_store_scan_result_as_json(out_buffer, buflen);
    
    //LOG_DEBUG("stored scanning result: %s", &buffer[0]);
    LOG_DEBUG("wifi manager scan complete, scanned: %d APs", scan_results.ap_count);
    return err;
}

int wifi_manager_init(void) {
    err_c_t err = 0;
    wifi_s_handle_t server = NULL;
    char ssid[64];
    char password[64];

    memutil_zero_memory(&ssid, sizeof(ssid));
    memutil_zero_memory(&password, sizeof(password));

    err = nvs_c_init_nvs();
    if(err != ERR_C_OK) {
        LOG_ERROR("wifi manager cannot continue, nvs error %d: %s", err, error_to_name(err));
        return err;
    }

    //if there are stored credentials, try to connect
    if(wifi_manager_get_stored_ap(&ssid[0], sizeof(ssid), &password[0], sizeof(password)) == ERR_C_OK) {
        err = wifi_c_init_wifi(WIFI_C_MODE_STA); 
        if(err != ERR_C_OK) {
            LOG_ERROR("wifi manager cannot continue, wifi init fails, error %d:%s", err, error_to_name(err));
            return err;
        }

        LOG_INFO("found stored AP, connecting to: %s", &ssid[0]);
        err = wifi_c_start_sta(&ssid[0], &password[0]);
        if(err == ERR_C_OK) {
            return err;
        }
    }

    err = wifi_c_init_wifi(WIFI_C_MODE_APSTA); 
    if(err != ERR_C_OK) {
        LOG_ERROR("wifi manager cannot continue, wifi init fails, error %d:%s", err, error_to_name(err));
        return err;
    }

    err = wifi_c_start_ap("ESP32", "12345678");
    if(err != ERR_C_OK) {
        LOG_ERROR("wifi_manager cannot continue, wifi ap start failed with error %d: %s", err, error_to_name(err));
        return err;
    }

    err = wifi_manager_server_init(&server);
    if(err != ERR_C_OK) {
        LOG_ERROR("error %d when trying to start wifi manager server: %s", err, error_to_name(err));
        return err;
    }

    return err;
}