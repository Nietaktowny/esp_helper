#include "nvs.h"
#include "nvs_flash.h"
#include "nvs_controller.h"
#include "err_controller.h"
#include "memory_utils.h"
#include "logger.h"

#include <string.h>
#include "esp_err.h"


struct nvs_c_handle_obj {
    nvs_handle_t esp_nvs;
    char namespace[16];
    nvs_open_mode_t mode;
};


/**
 * @brief Initialize NVS.
 * 
 * @return esp_c_t Error code if something goes wrong.
 */
int nvs_c_init_nvs (void) {
    
    err_c_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    LOG_INFO("nvs was initialized");
    return err;
}

int nvs_c_open(nvs_c_handle_t* handle, char* namespace) {
    err_c_t err = 0;
    ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("location of nvs_handle cannot be NULL"));
    ERR_C_CHECK_NULL_PTR(namespace, LOG_ERROR("namespace cannot be NULL"));
    
    //alloc memory
    NEW((*handle), struct nvs_c_handle_obj);
    memutil_zero_memory((*handle), sizeof(struct nvs_c_handle_obj));

    err = nvs_open(namespace, NVS_READWRITE, &((*handle)->esp_nvs));
    if(err != ERR_C_OK) {
        LOG_ERROR("error %d when opening %s namespace: %s", err, namespace, error_to_name(err));
        return err;
    }
    //store mode
    (*handle)->mode = NVS_READWRITE;

    //store namespace name in handle
    memcpy(&((*handle)->namespace), namespace, strlen(namespace));

    LOG_DEBUG("opened namespace %s for read and write operations", namespace);
    return err;  
}

int nvs_c_open_read_only(nvs_c_handle_t* handle, char* namespace) {
    err_c_t err = 0;
    ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("location of nvs_handle cannot be NULL"));
    ERR_C_CHECK_NULL_PTR(namespace, LOG_ERROR("namespace cannot be NULL"));
    
    //alloc memory
    NEW((*handle), struct nvs_c_handle_obj);
    memutil_zero_memory((*handle), sizeof(struct nvs_c_handle_obj));

    err = nvs_open(namespace, NVS_READONLY, &((*handle)->esp_nvs));
    if(err != ERR_C_OK) {
        LOG_ERROR("error %d when opening %s namespace: %s", err, namespace, esp_err_to_name(err));
        return err;
    }
    //store mode
    (*handle)->mode = NVS_READONLY;

    //store namespace name in handle
    memcpy(&((*handle)->namespace), namespace, strlen(namespace));

    LOG_DEBUG("opened namespace %s for read only operations", namespace);
    return err;    
}

int nvs_c_close(nvs_c_handle_t handle) {
    err_c_t err = 0;
    ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("nvs handle cannot be NULL"));

    nvs_close(handle->esp_nvs);
    LOG_DEBUG("nvs namespace: %s was closed", &(handle->namespace));

    return err;
}

int nvs_c_write_string(nvs_c_handle_t handle, const char* key, const char* buffer) {
    err_c_t err = 0;
    ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("nvs handle cannot be NULL"));
    ERR_C_CHECK_NULL_PTR(buffer, LOG_ERROR("data to write in NVS cannot be NULL"));

    if(handle->mode == NVS_READONLY) {
        LOG_ERROR("nvs namespace %s opened as READ ONLY, cannot write to it");
        return NVS_C_ERR_WRONG_MODE;
    }

    err = nvs_set_str(handle->esp_nvs, key, buffer);
    if(err != ERR_C_OK) {
        LOG_ERROR("error %d when writing data with key: %s into %s namespace: %s", err, key, handle->namespace, error_to_name(err));
        return err;
    }

    err = nvs_commit(handle->esp_nvs);
    if(err != ERR_C_OK) {
        LOG_ERROR("error %d when committing data with key: %s into %s namespace: %s", err, key, handle->namespace, error_to_name(err));
        return err;
    }

    LOG_INFO("written %d bytes data into namespace %s", strlen(buffer), handle->namespace);
    return err;
}

int nvs_c_read_string(nvs_c_handle_t handle, const char* key, char* buffer, size_t buflen) {
    err_c_t err = 0;
    size_t temp_buflen = buflen;
    ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("nvs handle cannot be NULL"));
    ERR_C_CHECK_NULL_PTR(buffer, LOG_ERROR("data to write in NVS cannot be NULL"));
    
    err = nvs_get_str(handle->esp_nvs, key, buffer, &temp_buflen);
    if(err != ERR_C_OK) {
        LOG_ERROR("error %d when writing data with key: %s into %s namespace: %s", err, key, &(handle->namespace[0]), error_to_name(err));
        return err;
    }

    LOG_INFO("read %d bytes from namespace: %s", temp_buflen, handle->namespace);
    return err;
}

int nvs_c_erase_namespace(nvs_c_handle_t handle) {
    err_c_t err = 0;
    err = nvs_erase_all(handle->esp_nvs);
    if(err != ERR_C_OK) {
        LOG_ERROR("error %d when erasing namespace %s: %s", err, &(handle->namespace[0]) ,error_to_name(err));
        return err;
    }

    LOG_DEBUG("erased all key & value pairs in namespace: %s", &(handle->namespace[0]));
    return err;
}