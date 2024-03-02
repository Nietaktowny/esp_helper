/**
 * @file nvs_controller.c
 * @author Wojciech Mytych
 * @brief NVS Controller library source file.
 * @version 0.1
 * @date 2024-03-01
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifdef ESP_PLATFORM

#include "nvs.h"
#include "nvs_flash.h"
#include "nvs_controller.h"
#include "err_controller.h"
#include "memory_utils.h"
#include "logger.h"

#include <string.h>
#include "esp_err.h"

int nvs_c_init_default_partition(void)
{

    err_c_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
        ERR_C_CHECK_ERROR(err, LOG_ERROR("error %d when initializing nvs partition: %s", err, error_to_name(err)));
    }

    if (err == ESP_ERR_NOT_FOUND)
    {
        LOG_ERROR("default nvs partition was not found.");
        return NVS_C_ERR_PARTITION_NOT_FOUND;
    }

    LOG_DEBUG("default nvs partition was init.");
    return err;
}

int nvs_c_init_partition(const char* partition_label)
{
    err_c_t err = ERR_C_OK;
    ERR_C_CHECK_NULL_PTR(partition_label, LOG_ERROR("nvs partition label cannot be NULL"));

    err = strlen(partition_label);
    if (err > NVS_PART_NAME_MAX_SIZE)
    {
        LOG_ERROR("nvs partition label exceeds maximum length, maximum is: %d, passed partition length is: %d", NVS_PART_NAME_MAX_SIZE, err);
        return ERR_C_WRONG_ARGS;
    }

    err = nvs_flash_init_partition(partition_label);
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
        ERR_C_CHECK_ERROR(err, LOG_ERROR("error %d when initializing nvs partition: %s", err, error_to_name(err)));
    }

    if (err == ESP_ERR_NOT_FOUND)
    {
        LOG_WARN("nvs partition \'%s\' was not found.", partition_label);
        return NVS_C_ERR_PARTITION_NOT_FOUND;
    }

    LOG_DEBUG("nvs partition \'%s\' was init.", partition_label);
    return err;
}

int nvs_c_open(nvs_c_handle_t* handle, const char* namespace, nvs_c_open_mode_t mode)
{
    volatile err_c_t err = ERR_C_OK;
    ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("location of nvs_handle cannot be NULL"));
    ERR_C_CHECK_NULL_PTR(namespace, LOG_ERROR("namespace cannot be NULL"));

    Try
    {
        // alloc memory
        NEW((*handle), struct nvs_c_handle_obj);
        ERR_C_CHECK_MEM_ALLOC((*handle), LOG_ERROR("cannot allocate memory for NVS Controller handle."));
        memutil_zero_memory((*handle), sizeof(struct nvs_c_handle_obj));

        ERR_C_CHECK_AND_THROW_ERR(nvs_open(namespace, (nvs_open_mode_t)mode, &((*handle)->esp_nvs)));

        // store mode
        (*handle)->mode = mode;

        // store partition name in handle
        memcpy((*handle)->partition, NVS_C_DEFAULT_NVS_PART_NAME, NVS_C_DEFAULT_NVS_PART_NAME_LEN);

        // store namespace name in handle
        memcpy((*handle)->namespace, namespace, strlen(namespace));

        if (mode == NVS_C_READONLY)
        {
            LOG_VERBOSE("opened namespace \'%s\' for read only operations", namespace);
        }
        else
        {
            LOG_VERBOSE("opened namespace \'%s\' for read and write operations", namespace);
        }
    }
    Catch(err)
    {
        switch (err)
        {
        case ESP_ERR_NVS_PART_NOT_FOUND:
            LOG_ERROR("not found default nvs partition");
            return NVS_C_ERR_PARTITION_NOT_FOUND;
        case ESP_ERR_NVS_NOT_INITIALIZED:
            LOG_ERROR("default nvs partition is not initialized.");
            return NVS_C_ERR_NOT_INIT;
        case ESP_ERR_NVS_NOT_FOUND:
            LOG_ERROR("namespace '%s' not found in default nvs partition", namespace);
            return NVS_C_ERR_NAMESPACE_NOT_FOUND;
        default:
            break;
        }
    }
    return err;
}

int nvs_c_open_from_partition(nvs_c_handle_t* handle, const char* partition_label, const char* namespace, nvs_c_open_mode_t mode)
{
    volatile err_c_t err = ERR_C_OK;
    ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("location of nvs_handle cannot be NULL"));
    ERR_C_CHECK_NULL_PTR(partition_label, LOG_ERROR("partition label cannot be NULL"));
    ERR_C_CHECK_NULL_PTR(namespace, LOG_ERROR("namespace cannot be NULL"));

    Try
    {
        // alloc memory
        NEW((*handle), struct nvs_c_handle_obj);
        ERR_C_CHECK_MEM_ALLOC((*handle), LOG_ERROR("cannot allocate memory for NVS Controller handle."));
        memutil_zero_memory((*handle), sizeof(struct nvs_c_handle_obj));

        ERR_C_CHECK_AND_THROW_ERR(nvs_open_from_partition(partition_label, namespace, (nvs_open_mode_t)mode, &((*handle)->esp_nvs)));

        // store mode
        (*handle)->mode = mode;

        // store partition name in handle
        memcpy((*handle)->partition, partition_label, strlen(partition_label));

        // store namespace name in handle
        memcpy((*handle)->namespace, namespace, strlen(namespace));

        if (mode == NVS_C_READONLY)
        {
            LOG_VERBOSE("opened namespace \'%s\' in nvs partition '%s' for read only operations", namespace, partition_label);
        }
        else
        {
            LOG_VERBOSE("opened namespace \'%s\' in nvs partition '%s' for read and write operations", namespace, partition_label);
        }
    }
    Catch(err)
    {
        switch (err)
        {
        case ESP_ERR_NVS_NOT_INITIALIZED:
            LOG_ERROR("nvs partition '%s' not init.", partition_label);
            return NVS_C_ERR_NOT_INIT;
        case ESP_ERR_NVS_PART_NOT_FOUND:
            LOG_ERROR("nvs partition '%s' was not found.", partition_label);
            return NVS_C_ERR_PARTITION_NOT_FOUND;
        case ESP_ERR_NVS_NOT_FOUND:
            LOG_ERROR("namespace '%s'not found in nvs partition '%s'", namespace, partition_label);
            return NVS_C_ERR_NAMESPACE_NOT_FOUND;
        default:
            LOG_ERROR("error %d when trying to open namespace '%s' from partition '%s': %s", err, namespace, partition_label, error_to_name(err));
            return err;
        }
    }
    return err;
}

int nvs_c_close(nvs_c_handle_t* handle)
{
    err_c_t err = ERR_C_OK;
    ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("nvs handle cannot be NULL"));

    nvs_close((*handle)->esp_nvs);
    
    LOG_VERBOSE("nvs namespace: \'%s\' was closed", (*handle)->namespace);
    DELETE((*handle));
    return err;
}

int nvs_c_write_string(nvs_c_handle_t handle, const char* const key, const char *buffer)
{
    err_c_t err = ERR_C_OK;
    ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("nvs handle cannot be NULL"));
    ERR_C_CHECK_NULL_PTR(key, LOG_ERROR("key to write in NVS cannot be NULL"));
    ERR_C_CHECK_NULL_PTR(buffer, LOG_ERROR("data to write in NVS cannot be NULL"));

    if (handle->mode == NVS_C_READONLY)
    {
        LOG_ERROR("nvs namespace \'%s\' opened as READ ONLY, cannot write to it", handle->namespace);
        return NVS_C_ERR_WRONG_MODE;
    }

    ERR_C_CHECK_ERROR(
        nvs_set_str(handle->esp_nvs, key, buffer),
        LOG_ERROR("error %d when writing data with key '%s' into namespace '%s': %s", err, key, handle->namespace, error_to_name(err)));

    ERR_C_CHECK_ERROR(
        nvs_commit(handle->esp_nvs),
        LOG_ERROR("error %d when committing data with key '%s' into namespace '%s': %s", err, key, handle->namespace, error_to_name(err)););

    LOG_VERBOSE("written %d bytes data into namespace '%s'", strlen(buffer), handle->namespace);
    return err;
}

int nvs_c_read_string(nvs_c_handle_t handle, const char* key, char *buffer, size_t buflen)
{
    err_c_t err = ERR_C_OK;
    size_t temp_buflen = buflen;
    ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("nvs handle cannot be NULL"));
    ERR_C_CHECK_NULL_PTR(key, LOG_ERROR("key to write in NVS cannot be NULL"));
    ERR_C_CHECK_NULL_PTR(buffer, LOG_ERROR("data to write in NVS cannot be NULL"));

    err = nvs_get_str(handle->esp_nvs, key, buffer, &temp_buflen);
    if (err == ESP_ERR_NVS_NOT_FOUND)
    {
        LOG_WARN("error %d, not found key \'%s\' within namespace \'%s\', error: %s", err, key, handle->namespace, error_to_name(err));
        return NVS_C_ERR_KEY_NOT_FOUND;
    }
    else if (err != ERR_C_OK)
    {
        LOG_ERROR("error %d when writing data with key: \'%s\' into \'%s\' namespace: %s", err, key, &(handle->namespace[0]), error_to_name(err));
        return err;
    }

    LOG_VERBOSE("read %d bytes from namespace: '%s'", temp_buflen, handle->namespace);
    return err;
}

int nvs_c_erase_namespace(nvs_c_handle_t handle)
{
    err_c_t err = ERR_C_OK;
    ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("NVS handle cannot be NULL"));

    if(handle->mode == NVS_C_READONLY) {
        LOG_WARN("cannot erase, namespace '%s' was opened in read only mode");
        return NVS_C_ERR_WRONG_MODE;
    }

    ERR_C_CHECK_ERROR(
        nvs_erase_all(handle->esp_nvs),
         LOG_ERROR("error %d when erasing namespace '%s': %s", err, handle->namespace, error_to_name(err)));

    LOG_DEBUG("erased all key & value pairs in namespace: '%s'", handle->namespace);
    return err;
}

int nvs_c_deinit_default_partition(void)
{
    err_c_t err = ERR_C_OK;

    err = nvs_flash_deinit();
    if (err == ESP_ERR_NVS_NOT_INITIALIZED)
    {
        LOG_WARN("default nvs partition not initialized");
        return NVS_C_ERR_NOT_INIT;
    }

    LOG_VERBOSE("default nvs partition was deinit");
    return err;
}

int nvs_c_deinit_partition(const char *partition_label)
{
    err_c_t err = ERR_C_OK;

    ERR_C_CHECK_NULL_PTR(partition_label, LOG_ERROR("nvs partition label cannot be NULL"));

    err = nvs_flash_deinit_partition(partition_label);
    if (err == ESP_ERR_NVS_NOT_INITIALIZED)
    {
        LOG_WARN("nvs partition '%s' not initialized", partition_label);
        return NVS_C_ERR_NOT_INIT;
    }

    LOG_VERBOSE("nvs partition '%s' was deinit", partition_label);
    return err;
}

#endif