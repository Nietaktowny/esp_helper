#include "wifi_manager.h"
#include "logger.h"
#include "err_controller.h"
#include "errors_list.h"
#include "wifi_http_server.h"
#include "wifi_controller.h"
#include "memory_utils.h"
#include "nvs_controller.h"

#include <string.h>
#include <stdbool.h>


#define WIFI_MANAGER_NVS_NAMESPACE "nvs"

int wifi_manager_get_stored_ap_as_json(char* buffer, size_t bufflen) {
    err_c_t err = 0;
    char ssid[64];
    ERR_C_CHECK_NULL_PTR(buffer, LOG_ERROR("location to store stored APs JSON cannot be NULL"));
    
    err = wifi_manager_get_stored_ap(&ssid[0], sizeof(ssid), NULL, 0);
    if(err != ERR_C_OK && err != 4354) {
        LOG_ERROR("cannot generate AP JSON, error %d: %s", err, error_to_name(err));
        return err;
    } else if(err == 4354 ) {   //ESP_ERR_NVS_NOT_FOUND
        LOG_WARN("not found any stored AP, generating empty list message...");
        snprintf(buffer, bufflen, "{\"stored_ssid\": \"%s\"}", "empty");
        return err;
    }

    snprintf(buffer, bufflen, "{\"stored_ssid\": \"%s\"}", &ssid[0]);

    LOG_DEBUG("generated stored AP as JSON");
    return err;
}

int wifi_manager_store_ap(char* ssid, size_t ssid_len, char* password, size_t password_len) {
    err_c_t err = 0;
    nvs_c_handle_t nvs = NULL;
    char namespace[15] = WIFI_MANAGER_NVS_NAMESPACE;

    ERR_C_CHECK_NULL_PTR(ssid, LOG_ERROR("ssid key cannot be NULL"));
    ERR_C_CHECK_NULL_PTR(password, LOG_ERROR("password key cannot be NULL"));

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
    volatile err_c_t err = 0;
    nvs_c_handle_t nvs = NULL;
    char namespace[15] = WIFI_MANAGER_NVS_NAMESPACE;
    ERR_C_CHECK_NULL_PTR(ssid, LOG_ERROR("location to store SSID cannot be NULL"));


    Try {
        ERR_C_CHECK_AND_THROW_ERR(nvs_c_open_read_only(&nvs, &namespace[0]));
        ERR_C_CHECK_AND_THROW_ERR(nvs_c_read_string(nvs, "ssid", ssid, ssid_len));
        LOG_DEBUG("found stored SSID in NVS: %s", ssid);
        //conditionally read password 
        if(password != NULL && password_len != 0) {
            ERR_C_CHECK_AND_THROW_ERR(nvs_c_read_string(nvs, "password", password, password_len));
            LOG_DEBUG("found stored password in NVS: %s", password);
        }
        nvs_c_close(nvs);
    } Catch(err) {
        switch (err)
        {
        case 4354:  //ESP_ERR_NVS_NOT_FOUND
            LOG_WARN("Cannot find any stored AP in NVS");
            return err;
        default:
            LOG_ERROR("Cannot get stored AP for wifi manager, error %d : %s", err, error_to_name(err));
            return err;
        }
    }
    /*
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
    LOG_DEBUG("found stored SSID in NVS: %s", ssid);
    
    //conditionally read password 
    if(password != NULL && password_len != 0) {
        err = nvs_c_read_string(nvs, "password", password, password_len);
        if(err != ERR_C_OK) {
            LOG_ERROR("error %d, wifi manager could not read PASSWORD from %s NVS namespace: %s", err, WIFI_MANAGER_NVS_NAMESPACE, error_to_name(err));
            return err;
        }

        LOG_DEBUG("found stored password in NVS: %s", password);
    }

    nvs_c_close(nvs);
    */
    return err;
}

int wifi_manager_fetch_ap_list(char* out_buffer, size_t buflen) {
    err_c_t err = 0;
    wifi_c_scan_result_t* scan_results;
    ERR_C_CHECK_NULL_PTR(out_buffer, LOG_ERROR("location to store scanned APs list cannot be NULL"));

    err = wifi_c_scan_all_ap(&scan_results);
    if(err != ERR_C_OK) {
        LOG_ERROR("error %d when scanning APs: %s", err, error_to_name(err));
        return err;
    }

    wifi_c_store_scan_result_as_json(out_buffer, buflen);
    
    //LOG_DEBUG("stored scanning result: %s", &buffer[0]);
    LOG_DEBUG("wifi manager scan complete, scanned: %d APs", scan_results->ap_count);
    return err;
}

int wifi_manager_erase_ap(void) {
    err_c_t err = 0;
    nvs_c_handle_t nvs = NULL;

    err = nvs_c_open(&nvs, WIFI_MANAGER_NVS_NAMESPACE);
    if(err != ERR_C_OK) {
        LOG_ERROR("error %d, wifi manager could not open %s NVS namespace: %s", err, WIFI_MANAGER_NVS_NAMESPACE, error_to_name(err));
        return err;
    }

    err = nvs_c_erase_namespace(nvs);
    if(err != ERR_C_OK) {
        LOG_ERROR("error %d, wifi manager could not erase %s NVS namespace: %s", err, WIFI_MANAGER_NVS_NAMESPACE, error_to_name(err));
        return err;
    }

    nvs_c_close(nvs);

    LOG_INFO("Wifi manager stored AP erased");
    return err;
}

int wifi_manager_start_ap_and_server(void) {
    err_c_t err = 0;
    wifi_s_handle_t server = NULL;
    wifi_c_status_t* status = NULL;

    status = wifi_c_get_status();

    if(status->wifi_initialized == false) {
        err = nvs_c_init_nvs();
        if(err != ERR_C_OK) {
            LOG_ERROR("wifi manager cannot continue, nvs error %d: %s", err, error_to_name(err));
            return err;
        }

        err = wifi_c_init_wifi(WIFI_C_MODE_APSTA); 
        if(err != ERR_C_OK) {
            LOG_ERROR("wifi manager cannot continue, wifi init fails, error %d:%s", err, error_to_name(err));
            return err;
        }        
    } else {
        err = wifi_c_change_mode(WIFI_C_MODE_APSTA);
        if(err != ERR_C_OK) {
            LOG_ERROR("wifi manager cannot switch to APSTA, error %d: %s", err, error_to_name(err));
            return err;
        }
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

    err = wifi_c_init_wifi(WIFI_C_MODE_STA); 
    if(err != ERR_C_OK) {
        LOG_ERROR("wifi manager cannot continue, wifi init fails, error %d:%s", err, error_to_name(err));
        return err;
    }

    //if there are stored credentials, try to connect
    if(wifi_manager_get_stored_ap(&ssid[0], sizeof(ssid), &password[0], sizeof(password)) == ERR_C_OK) {
        LOG_INFO("found stored AP, connecting to: %s", &ssid[0]);
        
        err = wifi_c_start_sta(&ssid[0], &password[0]);
        if(err == ERR_C_OK) {
            return err;
        }
    }

    LOG_DEBUG("wifi manager didn't find stored credentials or couldn't connect, starting AP and HTTP server...");

    wifi_c_deinit();

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