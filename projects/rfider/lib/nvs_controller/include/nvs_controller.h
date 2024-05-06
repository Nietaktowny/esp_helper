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
#include <stdint.h>
#include "nvs_c_errors.h"

#define NVS_C_DEFAULT_NVS_PART_NAME "nvs" ///< Name of default NVS partition.
#define NVS_C_DEFAULT_NVS_PART_NAME_LEN 4 ///< Length od default NVS partition name.

/**
 * @brief Mode of opening the non-volatile storage
 */
typedef enum
{
	NVS_C_READONLY, 	///< Read only NVS namespace mode.
    NVS_C_READWRITE, 	///< Read and write NVS namespace mode.
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
 * This function also frees resources allocated for NVS Controller namespace handle.
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
 * @param key String key of data to store.
 * @param buffer Buffer with data to be written.
 * 
 * @retval ERR_C_OK on success.
 * @retval ERR_C_NULL_POINTER on NULL handle, key or buffer.
 * @retval NVS_C_ERR_WRONG_MODE When trying to write namespace opened in read only mode.
 * @retval esp specific errors.
 */
int nvs_c_write_string(nvs_c_handle_t handle, const char* const key, const char *buffer);

/**
 * @brief Read string with passed key from NVS namespace.
 * 
 * @param handle NVS Controller namespace handle.
 * @param key String key of stored data.
 * @param buffer Buffer to store data.
 * @param buflen Buffer length.
 * 
 * @retval ERR_C_OK on success.
 * @retval ERR_C_NULL_POINTER on NULL handle, key or buffer.
 * @retval ERR_C_NO_MEMORY If provided buflen is too small to contain queried data.
 * @retval esp specific errors.
 */
int nvs_c_read_string(nvs_c_handle_t handle, const char* key, char *buffer, size_t buflen);

/**
 * @brief Write int8_t value to NVS namespace.
 * 
 * @param handle NVS Controller namespace handle.
 * @param key String key of data to store.
 * @param value Value to be written.
 * 
 * @retval ERR_C_OK on success.
 * @retval ERR_C_NULL_POINTER on NULL handle or key.
 * @retval NVS_C_ERR_WRONG_MODE When mode is read only.
 * @retval esp specific errors. 
 */
int nvs_c_write_int8(nvs_c_handle_t handle, const char* const key, int8_t value);

/**
 * @brief Read int8_t value from NVS namespace.
 * 
 * @param handle NVS Controller namespace handle.
 * @param key String key of stored data.
 * @param buffer Buffer where value will be stored.
 * 
 * @retval ERR_C_OK on success.
 * @retval ERR_C_NULL_POINTER on NULL handle, key or buffer.
 * @retval NVS_C_ERR_KEY_NOT_FOUND When key is not found.
 * @retval esp specific errors. 
 */
int nvs_c_read_int8(nvs_c_handle_t handle, const char* key, int8_t* buffer);

/**
 * @brief Write int16_t value to NVS namespace.
 * 
 * @param handle NVS Controller namespace handle.
 * @param key String key of data to store.
 * @param value Value to be written.
 * 
 * @retval ERR_C_OK on success.
 * @retval ERR_C_NULL_POINTER on NULL handle or key.
 * @retval NVS_C_ERR_WRONG_MODE When mode is read only.
 * @retval esp specific errors. 
 */
int nvs_c_write_int16(nvs_c_handle_t handle, const char *const key, int16_t value);

/**
 * @brief Read int16_t value from NVS namespace.
 * 
 * @param handle NVS Controller namespace handle.
 * @param key String key of stored data.
 * @param buffer Buffer where value will be stored.
 * 
 * @retval ERR_C_OK on success.
 * @retval ERR_C_NULL_POINTER on NULL handle, key or buffer.
 * @retval NVS_C_ERR_KEY_NOT_FOUND When key is not found.
 * @retval esp specific errors. 
 */
int nvs_c_read_int16(nvs_c_handle_t handle, const char *key, int16_t *buffer);

/**
 * @brief Write int32_t value to NVS namespace.
 * 
 * @param handle NVS Controller namespace handle.
 * @param key String key of data to store.
 * @param value Value to be written.
 * 
 * @retval ERR_C_OK on success.
 * @retval ERR_C_NULL_POINTER on NULL handle or key.
 * @retval NVS_C_ERR_WRONG_MODE When mode is read only.
 * @retval esp specific errors. 
 */
int nvs_c_write_int32(nvs_c_handle_t handle, const char *const key, int32_t value);

/**
 * @brief Read int32_t value from NVS namespace.
 * 
 * @param handle NVS Controller namespace handle.
 * @param key String key of stored data.
 * @param buffer Buffer where value will be stored.
 * 
 * @retval ERR_C_OK on success.
 * @retval ERR_C_NULL_POINTER on NULL handle, key or buffer.
 * @retval NVS_C_ERR_KEY_NOT_FOUND When key is not found.
 * @retval esp specific errors. 
 */
int nvs_c_read_int32(nvs_c_handle_t handle, const char *key, int32_t *buffer);

/**
 * @brief Write int64_t value to NVS namespace.
 * 
 * @param handle NVS Controller namespace handle.
 * @param key String key of data to store.
 * @param value Value to be written.
 * 
 * @retval ERR_C_OK on success.
 * @retval ERR_C_NULL_POINTER on NULL handle or key.
 * @retval NVS_C_ERR_WRONG_MODE When mode is read only.
 * @retval esp specific errors. 
 */
int nvs_c_write_int64(nvs_c_handle_t handle, const char *const key, int64_t value);

/**
 * @brief Read int64_t value from NVS namespace.
 * 
 * @param handle NVS Controller namespace handle.
 * @param key String key of stored data.
 * @param buffer Buffer where value will be stored.
 * 
 * @retval ERR_C_OK on success.
 * @retval ERR_C_NULL_POINTER on NULL handle, key or buffer.
 * @retval NVS_C_ERR_KEY_NOT_FOUND When key is not found.
 * @retval esp specific errors. 
 */
int nvs_c_read_int64(nvs_c_handle_t handle, const char *key, int64_t *buffer);

/**
 * @brief Write uint8_t value to NVS namespace.
 * 
 * @param handle NVS Controller namespace handle.
 * @param key String key of data to store.
 * @param value Value to be written.
 * 
 * @retval ERR_C_OK on success.
 * @retval ERR_C_NULL_POINTER on NULL handle or key.
 * @retval NVS_C_ERR_WRONG_MODE When mode is read only.
 * @retval esp specific errors. 
 */
int nvs_c_write_uint8(nvs_c_handle_t handle, const char* const key, uint8_t value);

/**
 * @brief Read uint8_t value from NVS namespace.
 * 
 * @param handle NVS Controller namespace handle.
 * @param key String key of stored data.
 * @param buffer Buffer where value will be stored.
 * 
 * @retval ERR_C_OK on success.
 * @retval ERR_C_NULL_POINTER on NULL handle, key or buffer.
 * @retval NVS_C_ERR_KEY_NOT_FOUND When key is not found.
 * @retval esp specific errors. 
 */
int nvs_c_read_uint8(nvs_c_handle_t handle, const char* key, uint8_t* buffer);

/**
 * @brief Write uint16_t value to NVS namespace.
 * 
 * @param handle NVS Controller namespace handle.
 * @param key String key of data to store.
 * @param value Value to be written.
 * 
 * @retval ERR_C_OK on success.
 * @retval ERR_C_NULL_POINTER on NULL handle or key.
 * @retval NVS_C_ERR_WRONG_MODE When mode is read only.
 * @retval esp specific errors. 
 */
int nvs_c_write_uint16(nvs_c_handle_t handle, const char *const key, uint16_t value);

/**
 * @brief Read uint16_t value from NVS namespace.
 * 
 * @param handle NVS Controller namespace handle.
 * @param key String key of stored data.
 * @param buffer Buffer where value will be stored.
 * 
 * @retval ERR_C_OK on success.
 * @retval ERR_C_NULL_POINTER on NULL handle, key or buffer.
 * @retval NVS_C_ERR_KEY_NOT_FOUND When key is not found.
 * @retval esp specific errors. 
 */
int nvs_c_read_uint16(nvs_c_handle_t handle, const char *key, uint16_t *buffer);

/**
 * @brief Write uint32_t value to NVS namespace.
 * 
 * @param handle NVS Controller namespace handle.
 * @param key String key of data to store.
 * @param value Value to be written.
 * 
 * @retval ERR_C_OK on success.
 * @retval ERR_C_NULL_POINTER on NULL handle or key.
 * @retval NVS_C_ERR_WRONG_MODE When mode is read only.
 * @retval esp specific errors. 
 */
int nvs_c_write_uint32(nvs_c_handle_t handle, const char *const key, uint32_t value);

/**
 * @brief Read uint32_t value from NVS namespace.
 * 
 * @param handle NVS Controller namespace handle.
 * @param key String key of stored data.
 * @param buffer Buffer where value will be stored.
 * 
 * @retval ERR_C_OK on success.
 * @retval ERR_C_NULL_POINTER on NULL handle, key or buffer.
 * @retval NVS_C_ERR_KEY_NOT_FOUND When key is not found.
 * @retval esp specific errors. 
 */
int nvs_c_read_uint32(nvs_c_handle_t handle, const char *key, uint32_t *buffer);

/**
 * @brief Write uint64_t value to NVS namespace.
 * 
 * @param handle NVS Controller namespace handle.
 * @param key String key of data to store.
 * @param value Value to be written.
 * 
 * @retval ERR_C_OK on success.
 * @retval ERR_C_NULL_POINTER on NULL handle or key.
 * @retval NVS_C_ERR_WRONG_MODE When mode is read only.
 * @retval esp specific errors. 
 */
int nvs_c_write_uint64(nvs_c_handle_t handle, const char *const key, uint64_t value);

/**
 * @brief Read uint64_t value from NVS namespace.
 * 
 * @param handle NVS Controller namespace handle.
 * @param key String key of stored data.
 * @param buffer Buffer where value will be stored.
 * 
 * @retval ERR_C_OK on success.
 * @retval ERR_C_NULL_POINTER on NULL handle, key or buffer.
 * @retval NVS_C_ERR_KEY_NOT_FOUND When key is not found.
 * @retval esp specific errors. 
 */
int nvs_c_read_uint64(nvs_c_handle_t handle, const char *key, uint64_t *buffer);

/**
 * @brief Write blob data to NVS namespace.
 * 
 * @param handle NVS Controller namespace handle.
 * @param key String key of data to store.
 * @param data Blob data to be written.
 * @param datalen Size of data to be written.
 * 
 * @retval ERR_C_OK on success.
 * @retval ERR_C_NULL_POINTER on NULL handle, key or data.
 * @retval NVS_C_ERR_WRONG_MODE When mode is read only.
 * @retval esp specific errors. 
 */
int nvs_c_write_blob(nvs_c_handle_t handle, const char *const key, const void *data, size_t datalen);

/**
 * @brief Read blob data from NVS namespace.
 * 
 * @param handle NVS Controller namespace handle.
 * @param key String key of stored data.
 * @param buffer Buffer to store blob data.
 * @param buflen Size of buffer to store blob data.
 * 
 * @retval ERR_C_OK on success.
 * @retval ERR_C_NULL_POINTER on NULL handle, key or buffer.
 * @retval NVS_C_ERR_KEY_NOT_FOUND When key is not found.
 * @retval ERR_C_NO_MEMORY When buffer size is not enough to store data.
 * @retval esp specific errors. 
 */
int nvs_c_read_blob(nvs_c_handle_t handle, const char *key, void *buffer, size_t buflen);

/**
 * @brief Read length of stored blob data in NVS namespace.
 * 
 * @param handle NVS Controller namespace handle.
 * @param key String key of stored data.
 * @param out_datalen Buffer to store blob size.
 * 
 * @retval ERR_C_OK on success.
 * @retval ERR_C_NULL_POINTER on NULL handle, key or buffer.
 * @retval NVS_C_ERR_KEY_NOT_FOUND When key is not found.
 * @retval esp specific errors. 
 */
int nvs_c_read_blob_length(nvs_c_handle_t handle, const char *key, size_t* out_datalen);

/**
 * @brief Read length of stored string data in NVS namespace.
 * 
 * @param handle NVS Controller namespace handle.
 * @param key String key of stored data.
 * @param out_datalen Buffer to store blob size.
 * 
 * @retval ERR_C_OK on success.
 * @retval ERR_C_NULL_POINTER on NULL handle, key or buffer.
 * @retval NVS_C_ERR_KEY_NOT_FOUND When key is not found.
 * @retval esp specific errors. 
 */
int nvs_c_read_string_length(nvs_c_handle_t handle, const char *key, size_t* out_datalen);

/**
 * @brief Delete single key-value pair from NVS namespace.
 * 
 * @param handle NVS Controller namespace handle.
 * @param key String key of stored data.
 * 
 * @retval ERR_C_OK on success.
 * @retval ERR_C_NULL_POINTER on NULL handle or key.
 * @retval NVS_C_ERR_WRONG_MODE When mode is read only.
 * @retval NVS_C_ERR_KEY_NOT_FOUND When key is not found.
 * @retval esp specific errors. 
 */
int nvs_c_erase_key(nvs_c_handle_t handle, const char* key);

/**
 * @brief Erase all key-value pairs in specified NVS namespace.
 * 
 * @param handle NVS Controller namespace handle.
 * 
 * @retval ERR_C_OK on success.
 * @retval NVS_C_ERR_WRONG_MODE When namespace is opened in READ ONLY mode.
 * @retval ERR_C_NULL_POINTER When handle is NULL.
 * @retval esp specific errors. 
 */
int nvs_c_erase_namespace(nvs_c_handle_t handle);

/**
 * @brief Erse all data in default NVS partition.
 * 
 * @warning Partition after erasing needs to be initialized again.
 * 
 * @retval ERR_C_OK on success.
 * @retval NVS_C_ERR_PARTITION_NOT_FOUND Default nvs partition was not found.
 * @retval esp specific errors. 
 */
int nvs_c_erase_default_partition(void);

/**
 * @brief Erase all data in specified NVS partition.
 * 
 * @warning Partition after erasing needs to be initialized again.
 * 
 * @param partition_label Label of NVS partition to erase.
 * 
 * @retval ERR_C_OK on success.
 * @retval ERR_C_NULL_POINTER When partition label is NULL.
 * @retval NVS_C_ERR_PARTITION_NOT_FOUND Default nvs partition was not found.
 * @retval esp specific errors. 
 */
int nvs_c_erase_partition(const char* partition_label);

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
 * @retval ERR_C_NULL_POINTER When partition label is NULL.
 * @retval ESP_ERR_NVS_NOT_INITIALIZED If nvs partition was not init.
 */
int nvs_c_deinit_partition(const char *partition_label);
