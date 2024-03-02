/**
 * @file nvs_controller.h
 * @author Wojciech Mytych
 * @brief NVS Controller library header file.
 * @version 0.1
 * @date 2024-03-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include <stddef.h>
#include "nvs_c_errors.h"

#define NVS_C_DEFAULT_NVS_PART_NAME "nvs" ///< Name of default NVS partition.
#define NVS_C_DEFAULT_NVS_PART_NAME_LEN 4 ///< Length od default NVS partition name.

/**
 * @brief Mode of opening the non-volatile storage
 */
typedef enum
{
    NVS_C_READONLY, ///< Read only
    NVS_C_READWRITE ///< Read and write
} nvs_c_open_mode_t;

/**
 * @brief NVS Controller object stuct containing needed context.
 *
 */
struct nvs_c_handle_obj
{
    uint32_t esp_nvs;       ///< ESP-IDF specific NVS handle.
    char partition[16];     ///< Name of currently used NVS partition.
    char namespace[16];     ///< Name of currently used NVS namespace.
    nvs_c_open_mode_t mode; ///< Type of open mode (read only or read-write).
};

/**
 * @brief NVS Controller namespace handle.
 * 
 * For each opened nvs namespace new NVS Controller handle should be used.
 *
 */
typedef struct nvs_c_handle_obj *nvs_c_handle_t;

/**
 * @brief Initialize default NVS partition.
 *
 * @retval ERR_C_OK On success.
 * @retval NVS_C_ERR_PARTITION_NOT_FOUND Default NVS partition was not found.
 * @retval esp specific errors.
 */
int nvs_c_init_default_partition(void);

/**
 * @brief Init NVS partition with specified label.
 *
 * @param partition_label Label of partition to init.
 *
 * @retval ERR_C_OK On success.
 * @retval NVS_C_ERR_NOT_FOUND If partition with specified label was not found.
 * @retval ERR_C_WRONG_ARGS If partition_label exceeds maximum partition name length.
 * @retval esp specific errors.
 */
int nvs_c_init_partition(const char *partition_label);

/**
 * @brief Open NVS namespace from default NVS partition for read and write operations.
 *
 * @param handle NVS Controller namespace handle.
 * @param namespace Namespace label.
 * @param mode Mode of opening namespace (read only or read and write).
 *
 * @retval ERR_C_OK On success.
 * @retval ERR_C_NULL_POINTER If handle or namespace is NULL.
 * @retval NVS_C_ERR_PARTITION_NOT_FOUND Default NVS partition was not found.
 * @retval NVS_C_ERR_NOT_INIT Default NVS partition is not initialized.
 * @retval esp specific errors.
 */
int nvs_c_open(nvs_c_handle_t *handle, const char *namespace, nvs_c_open_mode_t mode);

/**
 * @brief Open NVS namespace from specified NVS partition for read and write operations.
 *
 * @param handle NVS Controller namespace handle.
 * @param partition_label Label of nvs partition to use.
 * @param namespace Namespace label.
 * @param mode Mode of opening namespace (read only or read and write).
 *
 * @retval ERR_C_OK On success.
 * @retval ERR_C_NULL_POINTER If handle or namespace is NULL.
 * @retval NVS_C_ERR_PARTITION_NOT_FOUND Default NVS partition was not found.
 * @retval NVS_C_ERR_NOT_INIT Default NVS partition is not initialized.
 * @retval esp specific errors.
 */
int nvs_c_open_from_partition(nvs_c_handle_t *handle, const char *partition_label, const char *namespace, nvs_c_open_mode_t mode);

/**
 * @brief Close NVS namespace.
 * 
 * @param handle NVS Controller namespace handle.
 * 
 * @retval ERR_C_OK on success.
 * @retval ERR_C_NULL_POINTER on null handle.
 */
int nvs_c_close(nvs_c_handle_t* handle);

/**
 * @brief Write key-value pair string into nvs namespace.
 * 
 * @param handle NVS Controller namespace handle.
 * @param key String key, used later to retrieve written data.
 * @param buffer Buffer with data to be written.
 * 
 * @retval ERR_C_OK on success.
 * @retval ERR_C_NULL_POINTER on NULL handle, key or buffer.
 * @retval NVS_C_ERR_WRONG_MODE When trying to write namespace opened in read only mode.
 * @retval esp specific errors.
 */
int nvs_c_write_string(nvs_c_handle_t handle, const char* const key, const char *buffer);

/**
 * @brief Read data of passed key from NVS namespace.
 * 
 * @param handle NVS Controller namespace handle.
 * @param key String key of stored data.
 * @param buffer Buffer to store data.
 * @param buflen Buffer length.
 * 
 * @retval ERR_C_OK on success.
 * @retval ERR_C_NULL_POINTER on NULL handle, key or buffer.
 * @retval esp specific errors.
 */
int nvs_c_read_string(nvs_c_handle_t handle, const char* key, char *buffer, size_t buflen);

/**
 * @brief Erase all key value pairs in specified namespace.
 * 
 * @param handle NVS Controller namespace handle.
 * 
 * @retval ERR_C_OK on success.
 * @retval NVS_C_ERR_WRONG_MODE When namespace is opened in READ ONLY mode.
 * @retval ERR_C_NULL_POINTER When handle is NULL.
 */
int nvs_c_erase_namespace(nvs_c_handle_t handle);

/**
 * @brief Deinit default NVS partition.
 * 
 * @retval ERR_C_OK on success.
 * @retval ESP_ERR_NVS_NOT_INITIALIZED If default nvs partition was not init.
 */
int nvs_c_deinit_default_partition(void);

/**
 * @brief Deinit specified NVS partition.
 * 
 * @param partition_label Label of partition to deinit.
 * 
 * @retval ERR_C_OK on success.
 * @retval ESP_ERR_NVS_NOT_INITIALIZED If nvs partition was not init.
 */
int nvs_c_deinit_partition(const char *partition_label);