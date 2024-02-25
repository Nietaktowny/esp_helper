#include "ota_controller.h"
#include "logger.h"
#include "err_controller.h"
#include "errors_list.h"
#include "memory_utils.h"
#include "http_client.h"

#include "esp_ota_ops.h"
#include "esp_https_ota.h"
#include <string.h>
#include <inttypes.h>


static int ota_c_validate_image(esp_app_desc_t* new_app_info);

static void event_handler(void* arg, esp_event_base_t event_base,
                        int32_t event_id, void* event_data);

/* Event handler for catching system events */
static void event_handler(void* arg, esp_event_base_t event_base,
                        int32_t event_id, void* event_data)
{
    if (event_base == ESP_HTTPS_OTA_EVENT) {
        switch (event_id) {
            case ESP_HTTPS_OTA_START:
                LOG_INFO("OTA started");
                break;
            case ESP_HTTPS_OTA_CONNECTED:
                LOG_INFO("Connected to server");
                break;
            case ESP_HTTPS_OTA_GET_IMG_DESC:
                LOG_DEBUG("Reading Image Description");
                break;
            case ESP_HTTPS_OTA_VERIFY_CHIP_ID:
                LOG_DEBUG("Verifying chip id of new image: %d", *(esp_chip_id_t *)event_data);
                break;
            case ESP_HTTPS_OTA_DECRYPT_CB:
                LOG_DEBUG("Callback to decrypt function");
                break;
            case ESP_HTTPS_OTA_WRITE_FLASH:
                LOG_VERBOSE("Writing to flash: %d written", *(int *)event_data);
                break;
            case ESP_HTTPS_OTA_UPDATE_BOOT_PARTITION:
                LOG_INFO("Boot partition updated. Next Partition: %d", *(esp_partition_subtype_t *)event_data);
                break;
            case ESP_HTTPS_OTA_FINISH:
                LOG_INFO("OTA finish");
                break;
            case ESP_HTTPS_OTA_ABORT:
                LOG_WARN("OTA abort");
                break;
        }
    }
}

int ota_c_do_simple_ota(const char* url) {
    err_c_t err = 0;
    esp_http_client_config_t config = {
        .url = url,
        .skip_cert_common_name_check = true,
    };

    esp_https_ota_config_t ota_config = {
        .http_config = &config,
    };

    esp_event_handler_register(ESP_HTTPS_OTA_EVENT, ESP_EVENT_ANY_ID, event_handler, NULL);

    err = esp_https_ota(&ota_config);
    if(err != ERR_C_OK) {
        LOG_ERROR("error %d during https ota: %s", err, error_to_name(err));
        return err;
    }

    LOG_INFO("OTA successful, restarting!");
    esp_restart();
    return err;
}

static int ota_c_validate_image(esp_app_desc_t* new_app_info) {
    err_c_t err = ERR_C_OK;
    esp_app_desc_t running_app_info;
    memutil_zero_memory(&running_app_info, sizeof(running_app_info));

    ERR_C_CHECK_NULL_PTR(new_app_info, LOG_ERROR("new app image info cannot be NULL"));

    const esp_partition_t* running = esp_ota_get_running_partition();
    
    err = esp_ota_get_partition_description(running, &running_app_info);
    if(err == ERR_C_OK) {
        LOG_INFO("Running firmware version: %s", running_app_info.version);
    }

    err = memcmp(new_app_info->version, running_app_info.version, sizeof(new_app_info->version));
    if(err == 0) {
        LOG_WARN("Current running version is the same as a new. Will not update.");
        LOG_DEBUG("current firmware version: %s\nnew firmware version: %s", running_app_info.version, new_app_info->version);
        return OTA_C_ERR_IMAGES_THE_SAME;
    }

    LOG_DEBUG("validated new firmware version: %s", new_app_info->version);
    return 0;
}

int ota_c_update_device_data(const char* url, const char* path, const char* device_id, const char* board) {
    err_c_t err = 0;
    esp_app_desc_t app_info;
    char post[512];
    memutil_zero_memory(&app_info, sizeof(app_info));
    memutil_zero_memory(&post, sizeof(post));

    ERR_C_CHECK_NULL_PTR(url, LOG_ERROR("URL address of server cannot be NULL"));
    
    const esp_partition_t* running = esp_ota_get_running_partition();
    ERR_C_CHECK_NULL_PTR(running, LOG_ERROR("cannot get current partition structure"));

    err = esp_ota_get_partition_description(running, &app_info);
    if(err != ERR_C_OK) {
        LOG_ERROR("error %d, cannot get current partition description: %s", err, error_to_name(err));
        return err;
    }

    snprintf(post, sizeof(post), "device_id=%s&board=%s&project=%s&firmware_ver=%s&esp_idf=%s",
        device_id,
        board,
        app_info.project_name,
        app_info.version,
        app_info.idf_ver
    );

    err = http_client_post("192.168.0.108:8081", "bmp280/ota.php", &post, HTTP_CLIENT_POST_USE_STRLEN);
    if(err != ERR_C_OK) {
        LOG_ERROR("error %d when sending device data to server: %s", err, error_to_name(err));
        return err;
    }

    LOG_DEBUG("device data send successfully to server");
    return err;
}

int ota_c_start(const char* url) {
    err_c_t err = 0;
    esp_http_client_config_t client = {
        .url = url,
        .timeout_ms = 1000,
        .keep_alive_enable = true,
        .skip_cert_common_name_check = true,
    };

    esp_https_ota_config_t ota_config = {
        .http_config = &client,
    };
    esp_https_ota_handle_t https_ota_handle = NULL;
    
    err = esp_https_ota_begin(&ota_config, &https_ota_handle);
    if(err != ERR_C_OK) {
        LOG_ERROR("error %d, ESP HTTPS OTA Begin failed: %s", err, error_to_name(err));
        return err;
    }
    
    esp_app_desc_t app_desc;
    memutil_zero_memory(&app_desc, sizeof(app_desc));

    err = esp_https_ota_get_img_desc(https_ota_handle, &app_desc);
    if(err != ERR_C_OK) {
        LOG_ERROR("error %d during getting image description: %s", err, error_to_name(err));
        return err;
    }

    err = ota_c_validate_image(&app_desc);
    if(err != ERR_C_OK) {
        LOG_ERROR("image header verification failed");
        return err;
    }

    while (1)
    {
        err = esp_https_ota_perform(https_ota_handle);
        if(err != ESP_ERR_HTTPS_OTA_IN_PROGRESS) {
            break;
        }

        LOG_DEBUG("image bytes read: %d", esp_https_ota_get_image_len_read(https_ota_handle));
    }
    
    err = esp_https_ota_is_complete_data_received(https_ota_handle);
    if(err != true) {
        LOG_ERROR("complete image data was not received");
        return err;
    }

    err = esp_https_ota_finish(https_ota_handle);
    if(err == ERR_C_OK) {
        LOG_INFO("ESP HTTPS OTA upgrade successful. Rebooting...");
        vTaskDelay(pdMS_TO_TICKS(1000));
        esp_restart();
    }

    if(err == ESP_ERR_OTA_VALIDATE_FAILED) {
        LOG_ERROR("Image validation failed, image is corrupted");
    }

    LOG_ERROR("ESP HTTPS OTA upgrade failed 0x%x", err);
    return err;
}


