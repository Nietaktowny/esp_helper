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

#include "nvs_controller.h"

#include <string.h>

#include "err_controller.h"
#include "esp_err.h"
#include "logger.h"
#include "memory_utils.h"
#include "nvs.h"
#include "nvs_flash.h"

int nvs_c_init_default_partition(void) {
	err_c_t err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		err = nvs_flash_init();
		ERR_C_CHECK_ERROR(err, LOG_ERROR("error %d when initializing nvs partition: %s", err, error_to_name(err)));
	}

	if (err == ESP_ERR_NOT_FOUND) {
		LOG_ERROR("default nvs partition was not found.");
		return NVS_C_ERR_PARTITION_NOT_FOUND;
	}

	LOG_DEBUG("default nvs partition was init.");
	return err;
}

int nvs_c_init_partition(const char *partition_label) {
	err_c_t err = ERR_C_OK;
	ERR_C_CHECK_NULL_PTR(partition_label, LOG_ERROR("nvs partition label cannot be NULL"));

	err = strlen(partition_label);
	if (err > NVS_PART_NAME_MAX_SIZE) {
		LOG_ERROR(
			"nvs partition label exceeds maximum length, maximum is: %d, "
			"passed partition length is: %d",
			NVS_PART_NAME_MAX_SIZE, err);
		return ERR_C_WRONG_ARGS;
	}

	err = nvs_flash_init_partition(partition_label);
	if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		err = nvs_flash_init();
		ERR_C_CHECK_ERROR(err, LOG_ERROR("error %d when initializing nvs partition: %s", err, error_to_name(err)));
	}

	if (err == ESP_ERR_NOT_FOUND) {
		LOG_WARN("nvs partition \'%s\' was not found.", partition_label);
		return NVS_C_ERR_PARTITION_NOT_FOUND;
	}

	LOG_DEBUG("nvs partition \'%s\' was init.", partition_label);
	return err;
}

int nvs_c_open(nvs_c_handle_t *handle, const char *namespace, nvs_c_open_mode_t mode) {
	volatile err_c_t err = ERR_C_OK;
	ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("location of nvs_handle cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(namespace, LOG_ERROR("namespace cannot be NULL"));

	Try {
		// alloc memory
		NEW((*handle), struct nvs_c_handle_obj);
		ERR_C_CHECK_MEM_ALLOC((*handle), LOG_ERROR("cannot allocate memory for NVS Controller handle."));
		memutil_zero_memory((*handle), sizeof(struct nvs_c_handle_obj));

		ERR_C_CHECK_AND_THROW_ERR(nvs_open(namespace, (nvs_open_mode_t)mode, &((*handle)->esp_nvs)));
	}
	Catch(err) {
		switch (err) {
			case ESP_ERR_NVS_PART_NOT_FOUND:
				LOG_ERROR("not found default nvs partition");
				DELETE((*handle));
				LOG_VERBOSE("freed nvs namespace handle resources");
				return NVS_C_ERR_PARTITION_NOT_FOUND;
			case ESP_ERR_NVS_NOT_INITIALIZED:
				LOG_ERROR("default nvs partition is not initialized.");
				DELETE((*handle));
				LOG_VERBOSE("freed nvs namespace handle resources");
				return NVS_C_ERR_NOT_INIT;
			case ESP_ERR_NVS_NOT_FOUND:
				LOG_ERROR("namespace '%s' not found in default nvs partition", namespace);
				DELETE((*handle));
				LOG_VERBOSE("freed nvs namespace handle resources");
				return NVS_C_ERR_NAMESPACE_NOT_FOUND;
			default:
				break;
		}
	}

	// store mode
	(*handle)->mode = mode;

	// store partition name in handle
	memcpy((*handle)->partition, NVS_C_DEFAULT_NVS_PART_NAME, NVS_C_DEFAULT_NVS_PART_NAME_LEN);

	// store namespace name in handle
	memcpy((*handle)->namespace, namespace, strlen(namespace));

	if (mode == NVS_C_READONLY) {
		LOG_VERBOSE("opened namespace \'%s\' for read only operations", namespace);
	} else {
		LOG_VERBOSE("opened namespace \'%s\' for read and write operations", namespace);
	}

	return err;
}

int nvs_c_open_from_partition(nvs_c_handle_t *handle, const char *partition_label, const char *namespace,
							  nvs_c_open_mode_t mode) {
	volatile err_c_t err = ERR_C_OK;
	ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("location of nvs_handle cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(partition_label, LOG_ERROR("partition label cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(namespace, LOG_ERROR("namespace cannot be NULL"));

	Try {
		// alloc memory
		NEW((*handle), struct nvs_c_handle_obj);
		ERR_C_CHECK_MEM_ALLOC((*handle), LOG_ERROR("cannot allocate memory for NVS Controller handle."));
		memutil_zero_memory((*handle), sizeof(struct nvs_c_handle_obj));

		ERR_C_CHECK_AND_THROW_ERR(
			nvs_open_from_partition(partition_label, namespace, (nvs_open_mode_t)mode, &((*handle)->esp_nvs)));

		// store mode
		(*handle)->mode = mode;

		// store partition name in handle
		memcpy((*handle)->partition, partition_label, strlen(partition_label));

		// store namespace name in handle
		memcpy((*handle)->namespace, namespace, strlen(namespace));

		if (mode == NVS_C_READONLY) {
			LOG_VERBOSE(
				"opened namespace \'%s\' in nvs partition '%s' for read only "
				"operations",
				namespace, partition_label);
		} else {
			LOG_VERBOSE(
				"opened namespace \'%s\' in nvs partition '%s' for read and "
				"write operations",
				namespace, partition_label);
		}
	}
	Catch(err) {
		switch (err) {
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
				LOG_ERROR(
					"error %d when trying to open namespace '%s' from "
					"partition '%s': %s",
					err, namespace, partition_label, error_to_name(err));
				return err;
		}
	}
	return err;
}

int nvs_c_close(nvs_c_handle_t *handle) {
	err_c_t err = ERR_C_OK;
	ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("nvs handle cannot be NULL"));

	nvs_close((*handle)->esp_nvs);

	LOG_VERBOSE("nvs namespace: \'%s\' was closed", (*handle)->namespace);
	DELETE((*handle));
	return err;
}

int nvs_c_write_string(nvs_c_handle_t handle, const char *const key, const char *buffer) {
	err_c_t err = ERR_C_OK;
	ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("nvs handle cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(key, LOG_ERROR("key to write in NVS cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(buffer, LOG_ERROR("data to write in NVS cannot be NULL"));

	if (handle->mode == NVS_C_READONLY) {
		LOG_ERROR("nvs namespace \'%s\' opened as READ ONLY, cannot write to it", handle->namespace);
		return NVS_C_ERR_WRONG_MODE;
	}

	ERR_C_CHECK_ERROR(nvs_set_str(handle->esp_nvs, key, buffer),
					  LOG_ERROR("error %d when writing data with key '%s' into namespace '%s': %s", err, key,
								handle->namespace, error_to_name(err)));

	ERR_C_CHECK_ERROR(nvs_commit(handle->esp_nvs),
					  LOG_ERROR("error %d when committing data with key '%s' into namespace "
								"'%s': %s",
								err, key, handle->namespace, error_to_name(err)));

	LOG_VERBOSE("written %d bytes data into namespace '%s'", strlen(buffer), handle->namespace);
	return err;
}

int nvs_c_read_string(nvs_c_handle_t handle, const char *key, char *buffer, size_t buflen) {
	err_c_t err = ERR_C_OK;
	size_t temp_buflen = buflen;
	ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("nvs handle cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(key, LOG_ERROR("key to write in NVS cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(buffer, LOG_ERROR("data to write in NVS cannot be NULL"));

	err = nvs_get_str(handle->esp_nvs, key, NULL, &temp_buflen);
	if (err == ESP_ERR_NVS_NOT_FOUND) {
		LOG_WARN("error %d, not found key \'%s\' within namespace \'%s\', error: %s", err, key, handle->namespace,
				 error_to_name(err));
		return NVS_C_ERR_KEY_NOT_FOUND;
	} else if (temp_buflen != 0 && temp_buflen > buflen) {
		LOG_ERROR(
			"buffer size (%llu bytes) is to small to store string data, "
			"expected %llu bytes size.",
			buflen, temp_buflen);
		return ERR_C_NO_MEMORY;
	}

	temp_buflen = buflen;
	err = nvs_get_str(handle->esp_nvs, key, buffer, &temp_buflen);
	if (err == ESP_ERR_NVS_NOT_FOUND) {
		LOG_WARN("error %d, not found key \'%s\' within namespace \'%s\', error: %s", err, key, handle->namespace,
				 error_to_name(err));
		return NVS_C_ERR_KEY_NOT_FOUND;
	} else if (err != ERR_C_OK) {
		LOG_ERROR(
			"error %d when writing data with key: \'%s\' into \'%s\' "
			"namespace: %s",
			err, key, &(handle->namespace[0]), error_to_name(err));
		return err;
	}

	LOG_VERBOSE("read %d bytes from namespace: '%s'", temp_buflen, handle->namespace);
	return err;
}

int nvs_c_write_int8(nvs_c_handle_t handle, const char *const key, int8_t value) {
	err_c_t err = ERR_C_OK;
	ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("nvs handle cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(key, LOG_ERROR("key to write in NVS cannot be NULL"));

	if (handle->mode == NVS_C_READONLY) {
		LOG_ERROR("nvs namespace \'%s\' opened as READ ONLY, cannot write to it", handle->namespace);
		return NVS_C_ERR_WRONG_MODE;
	}

	ERR_C_CHECK_ERROR(nvs_set_i8(handle->esp_nvs, key, value),
					  LOG_ERROR("error %d when writing data with key '%s' into namespace '%s': %s", err, key,
								handle->namespace, error_to_name(err)));

	ERR_C_CHECK_ERROR(nvs_commit(handle->esp_nvs),
					  LOG_ERROR("error %d when committing data with key '%s' into namespace "
								"'%s': %s",
								err, key, handle->namespace, error_to_name(err)););

	LOG_VERBOSE("written value int8_t %d into namespace '%s'", value, handle->namespace);
	return err;
}

int nvs_c_read_int8(nvs_c_handle_t handle, const char *key, int8_t *buffer) {
	err_c_t err = ERR_C_OK;
	ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("nvs handle cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(key, LOG_ERROR("key to write in NVS cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(buffer, LOG_ERROR("buffer to store read data cannot be NULL"));

	err = nvs_get_i8(handle->esp_nvs, key, buffer);
	if (err == ESP_ERR_NVS_NOT_FOUND) {
		LOG_WARN("error %d, not found key \'%s\' within namespace \'%s\', error: %s", err, key, handle->namespace,
				 error_to_name(err));
		return NVS_C_ERR_KEY_NOT_FOUND;
	} else if (err != ERR_C_OK) {
		LOG_ERROR(
			"error %d when writing data with key: \'%s\' into \'%s\' "
			"namespace: %s",
			err, key, &(handle->namespace[0]), error_to_name(err));
		return err;
	}

	LOG_VERBOSE("read value int8_t %d from namespace: '%s'", *buffer, handle->namespace);
	return err;
}

int nvs_c_write_int16(nvs_c_handle_t handle, const char *const key, int16_t value) {
	err_c_t err = ERR_C_OK;
	ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("nvs handle cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(key, LOG_ERROR("key to write in NVS cannot be NULL"));

	if (handle->mode == NVS_C_READONLY) {
		LOG_ERROR("nvs namespace \'%s\' opened as READ ONLY, cannot write to it", handle->namespace);
		return NVS_C_ERR_WRONG_MODE;
	}

	ERR_C_CHECK_ERROR(nvs_set_i16(handle->esp_nvs, key, value),
					  LOG_ERROR("error %d when writing data with key '%s' into namespace '%s': %s", err, key,
								handle->namespace, error_to_name(err)));

	ERR_C_CHECK_ERROR(nvs_commit(handle->esp_nvs),
					  LOG_ERROR("error %d when committing data with key '%s' into namespace "
								"'%s': %s",
								err, key, handle->namespace, error_to_name(err)););

	LOG_VERBOSE("written value int16_t %d into namespace '%s'", value, handle->namespace);
	return err;
}

int nvs_c_read_int16(nvs_c_handle_t handle, const char *key, int16_t *buffer) {
	err_c_t err = ERR_C_OK;
	ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("nvs handle cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(key, LOG_ERROR("key to write in NVS cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(buffer, LOG_ERROR("buffer to store read data cannot be NULL"));

	err = nvs_get_i16(handle->esp_nvs, key, buffer);
	if (err == ESP_ERR_NVS_NOT_FOUND) {
		LOG_WARN("error %d, not found key \'%s\' within namespace \'%s\', error: %s", err, key, handle->namespace,
				 error_to_name(err));
		return NVS_C_ERR_KEY_NOT_FOUND;
	} else if (err != ERR_C_OK) {
		LOG_ERROR(
			"error %d when writing data with key: \'%s\' into \'%s\' "
			"namespace: %s",
			err, key, &(handle->namespace[0]), error_to_name(err));
		return err;
	}

	LOG_VERBOSE("read value int16_t %d from namespace: '%s'", *buffer, handle->namespace);
	return err;
}

int nvs_c_write_int32(nvs_c_handle_t handle, const char *const key, int32_t value) {
	err_c_t err = ERR_C_OK;
	ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("nvs handle cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(key, LOG_ERROR("key to write in NVS cannot be NULL"));

	if (handle->mode == NVS_C_READONLY) {
		LOG_ERROR("nvs namespace \'%s\' opened as READ ONLY, cannot write to it", handle->namespace);
		return NVS_C_ERR_WRONG_MODE;
	}

	ERR_C_CHECK_ERROR(nvs_set_i32(handle->esp_nvs, key, value),
					  LOG_ERROR("error %d when writing data with key '%s' into namespace '%s': %s", err, key,
								handle->namespace, error_to_name(err)));

	ERR_C_CHECK_ERROR(nvs_commit(handle->esp_nvs),
					  LOG_ERROR("error %d when committing data with key '%s' into namespace "
								"'%s': %s",
								err, key, handle->namespace, error_to_name(err)););

	LOG_VERBOSE("written value int32_t %ld into namespace '%s'", value, handle->namespace);
	return err;
}

int nvs_c_read_int32(nvs_c_handle_t handle, const char *key, int32_t *buffer) {
	err_c_t err = ERR_C_OK;
	ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("nvs handle cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(key, LOG_ERROR("key to write in NVS cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(buffer, LOG_ERROR("buffer to store read data cannot be NULL"));

	err = nvs_get_i32(handle->esp_nvs, key, buffer);
	if (err == ESP_ERR_NVS_NOT_FOUND) {
		LOG_WARN("error %d, not found key \'%s\' within namespace \'%s\', error: %s", err, key, handle->namespace,
				 error_to_name(err));
		return NVS_C_ERR_KEY_NOT_FOUND;
	} else if (err != ERR_C_OK) {
		LOG_ERROR(
			"error %d when writing data with key: \'%s\' into \'%s\' "
			"namespace: %s",
			err, key, &(handle->namespace[0]), error_to_name(err));
		return err;
	}

	LOG_VERBOSE("read value int32_t %ld from namespace: '%s'", *buffer, handle->namespace);
	return err;
}

int nvs_c_write_int64(nvs_c_handle_t handle, const char *const key, int64_t value) {
	err_c_t err = ERR_C_OK;
	ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("nvs handle cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(key, LOG_ERROR("key to write in NVS cannot be NULL"));

	if (handle->mode == NVS_C_READONLY) {
		LOG_ERROR("nvs namespace \'%s\' opened as READ ONLY, cannot write to it", handle->namespace);
		return NVS_C_ERR_WRONG_MODE;
	}

	ERR_C_CHECK_ERROR(nvs_set_i64(handle->esp_nvs, key, value),
					  LOG_ERROR("error %d when writing data with key '%s' into namespace '%s': %s", err, key,
								handle->namespace, error_to_name(err)));

	ERR_C_CHECK_ERROR(nvs_commit(handle->esp_nvs),
					  LOG_ERROR("error %d when committing data with key '%s' into namespace "
								"'%s': %s",
								err, key, handle->namespace, error_to_name(err)););

	LOG_VERBOSE("written value int64_t %lld into namespace '%s'", value, handle->namespace);
	return err;
}

int nvs_c_read_int64(nvs_c_handle_t handle, const char *key, int64_t *buffer) {
	err_c_t err = ERR_C_OK;
	ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("nvs handle cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(key, LOG_ERROR("key to write in NVS cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(buffer, LOG_ERROR("buffer to store read data cannot be NULL"));

	err = nvs_get_i64(handle->esp_nvs, key, buffer);
	if (err == ESP_ERR_NVS_NOT_FOUND) {
		LOG_WARN("error %d, not found key \'%s\' within namespace \'%s\', error: %s", err, key, handle->namespace,
				 error_to_name(err));
		return NVS_C_ERR_KEY_NOT_FOUND;
	} else if (err != ERR_C_OK) {
		LOG_ERROR(
			"error %d when writing data with key: \'%s\' into \'%s\' "
			"namespace: %s",
			err, key, &(handle->namespace[0]), error_to_name(err));
		return err;
	}

	LOG_VERBOSE("read value int64_t %lld from namespace: '%s'", *buffer, handle->namespace);
	return err;
}

int nvs_c_write_uint8(nvs_c_handle_t handle, const char *const key, uint8_t value) {
	err_c_t err = ERR_C_OK;
	ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("nvs handle cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(key, LOG_ERROR("key to write in NVS cannot be NULL"));

	if (handle->mode == NVS_C_READONLY) {
		LOG_ERROR("nvs namespace \'%s\' opened as READ ONLY, cannot write to it", handle->namespace);
		return NVS_C_ERR_WRONG_MODE;
	}

	ERR_C_CHECK_ERROR(nvs_set_u8(handle->esp_nvs, key, value),
					  LOG_ERROR("error %d when writing data with key '%s' into namespace '%s': %s", err, key,
								handle->namespace, error_to_name(err)));

	ERR_C_CHECK_ERROR(nvs_commit(handle->esp_nvs),
					  LOG_ERROR("error %d when committing data with key '%s' into namespace "
								"'%s': %s",
								err, key, handle->namespace, error_to_name(err)););

	LOG_VERBOSE("written value uint8_t %u into namespace '%s'", value, handle->namespace);
	return err;
}

int nvs_c_read_uint8(nvs_c_handle_t handle, const char *key, uint8_t *buffer) {
	err_c_t err = ERR_C_OK;
	ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("nvs handle cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(key, LOG_ERROR("key to write in NVS cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(buffer, LOG_ERROR("buffer to store read data cannot be NULL"));

	err = nvs_get_u8(handle->esp_nvs, key, buffer);
	if (err == ESP_ERR_NVS_NOT_FOUND) {
		LOG_WARN("error %d, not found key \'%s\' within namespace \'%s\', error: %s", err, key, handle->namespace,
				 error_to_name(err));
		return NVS_C_ERR_KEY_NOT_FOUND;
	} else if (err != ERR_C_OK) {
		LOG_ERROR(
			"error %d when writing data with key: \'%s\' into \'%s\' "
			"namespace: %s",
			err, key, &(handle->namespace[0]), error_to_name(err));
		return err;
	}

	LOG_VERBOSE("read value uint8_t %u from namespace: '%s'", *buffer, handle->namespace);
	return err;
}

int nvs_c_write_uint16(nvs_c_handle_t handle, const char *const key, uint16_t value) {
	err_c_t err = ERR_C_OK;
	ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("nvs handle cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(key, LOG_ERROR("key to write in NVS cannot be NULL"));

	if (handle->mode == NVS_C_READONLY) {
		LOG_ERROR("nvs namespace \'%s\' opened as READ ONLY, cannot write to it", handle->namespace);
		return NVS_C_ERR_WRONG_MODE;
	}

	ERR_C_CHECK_ERROR(nvs_set_u16(handle->esp_nvs, key, value),
					  LOG_ERROR("error %d when writing data with key '%s' into namespace '%s': %s", err, key,
								handle->namespace, error_to_name(err)));

	ERR_C_CHECK_ERROR(nvs_commit(handle->esp_nvs),
					  LOG_ERROR("error %d when committing data with key '%s' into namespace "
								"'%s': %s",
								err, key, handle->namespace, error_to_name(err)););

	LOG_VERBOSE("written value int16_t %u into namespace '%s'", value, handle->namespace);
	return err;
}

int nvs_c_read_uint16(nvs_c_handle_t handle, const char *key, uint16_t *buffer) {
	err_c_t err = ERR_C_OK;
	ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("nvs handle cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(key, LOG_ERROR("key to write in NVS cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(buffer, LOG_ERROR("buffer to store read data cannot be NULL"));

	err = nvs_get_u16(handle->esp_nvs, key, buffer);
	if (err == ESP_ERR_NVS_NOT_FOUND) {
		LOG_WARN("error %d, not found key \'%s\' within namespace \'%s\', error: %s", err, key, handle->namespace,
				 error_to_name(err));
		return NVS_C_ERR_KEY_NOT_FOUND;
	} else if (err != ERR_C_OK) {
		LOG_ERROR(
			"error %d when writing data with key: \'%s\' into \'%s\' "
			"namespace: %s",
			err, key, &(handle->namespace[0]), error_to_name(err));
		return err;
	}

	LOG_VERBOSE("read value uint16_t %u from namespace: '%s'", *buffer, handle->namespace);
	return err;
}

int nvs_c_write_uint32(nvs_c_handle_t handle, const char *const key, uint32_t value) {
	err_c_t err = ERR_C_OK;
	ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("nvs handle cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(key, LOG_ERROR("key to write in NVS cannot be NULL"));

	if (handle->mode == NVS_C_READONLY) {
		LOG_ERROR("nvs namespace \'%s\' opened as READ ONLY, cannot write to it", handle->namespace);
		return NVS_C_ERR_WRONG_MODE;
	}

	ERR_C_CHECK_ERROR(nvs_set_u32(handle->esp_nvs, key, value),
					  LOG_ERROR("error %d when writing data with key '%s' into namespace '%s': %s", err, key,
								handle->namespace, error_to_name(err)));

	ERR_C_CHECK_ERROR(nvs_commit(handle->esp_nvs),
					  LOG_ERROR("error %d when committing data with key '%s' into namespace "
								"'%s': %s",
								err, key, handle->namespace, error_to_name(err)););

	LOG_VERBOSE("written value uint32_t %lu into namespace '%s'", value, handle->namespace);
	return err;
}

int nvs_c_read_uint32(nvs_c_handle_t handle, const char *key, uint32_t *buffer) {
	err_c_t err = ERR_C_OK;
	ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("nvs handle cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(key, LOG_ERROR("key to write in NVS cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(buffer, LOG_ERROR("buffer to store read data cannot be NULL"));

	err = nvs_get_u32(handle->esp_nvs, key, buffer);
	if (err == ESP_ERR_NVS_NOT_FOUND) {
		LOG_WARN("error %d, not found key \'%s\' within namespace \'%s\', error: %s", err, key, handle->namespace,
				 error_to_name(err));
		return NVS_C_ERR_KEY_NOT_FOUND;
	} else if (err != ERR_C_OK) {
		LOG_ERROR(
			"error %d when writing data with key: \'%s\' into \'%s\' "
			"namespace: %s",
			err, key, &(handle->namespace[0]), error_to_name(err));
		return err;
	}

	LOG_VERBOSE("read value uint32_t %lu from namespace: '%s'", *buffer, handle->namespace);
	return err;
}

int nvs_c_write_uint64(nvs_c_handle_t handle, const char *const key, uint64_t value) {
	err_c_t err = ERR_C_OK;
	ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("nvs handle cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(key, LOG_ERROR("key to write in NVS cannot be NULL"));

	if (handle->mode == NVS_C_READONLY) {
		LOG_ERROR("nvs namespace \'%s\' opened as READ ONLY, cannot write to it", handle->namespace);
		return NVS_C_ERR_WRONG_MODE;
	}

	ERR_C_CHECK_ERROR(nvs_set_u64(handle->esp_nvs, key, value),
					  LOG_ERROR("error %d when writing data with key '%s' into namespace '%s': %s", err, key,
								handle->namespace, error_to_name(err)));

	ERR_C_CHECK_ERROR(nvs_commit(handle->esp_nvs),
					  LOG_ERROR("error %d when committing data with key '%s' into namespace "
								"'%s': %s",
								err, key, handle->namespace, error_to_name(err)););

	LOG_VERBOSE("written value uint64_t %llu into namespace '%s'", value, handle->namespace);
	return err;
}

int nvs_c_read_uint64(nvs_c_handle_t handle, const char *key, uint64_t *buffer) {
	err_c_t err = ERR_C_OK;
	ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("nvs handle cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(key, LOG_ERROR("key to write in NVS cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(buffer, LOG_ERROR("buffer to store read data cannot be NULL"));

	err = nvs_get_u64(handle->esp_nvs, key, buffer);
	if (err == ESP_ERR_NVS_NOT_FOUND) {
		LOG_WARN("error %d, not found key \'%s\' within namespace \'%s\', error: %s", err, key, handle->namespace,
				 error_to_name(err));
		return NVS_C_ERR_KEY_NOT_FOUND;
	} else if (err != ERR_C_OK) {
		LOG_ERROR(
			"error %d when writing data with key: \'%s\' into \'%s\' "
			"namespace: %s",
			err, key, &(handle->namespace[0]), error_to_name(err));
		return err;
	}

	LOG_VERBOSE("read value uint64_t %llu from namespace: '%s'", *buffer, handle->namespace);
	return err;
}

int nvs_c_write_blob(nvs_c_handle_t handle, const char *const key, const void *data, size_t temp_buflen) {
	err_c_t err = ERR_C_OK;
	ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("nvs handle cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(key, LOG_ERROR("key to write in NVS cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(data, LOG_ERROR("data to store cannot be NULL"));

	if (handle->mode == NVS_C_READONLY) {
		LOG_ERROR("nvs namespace \'%s\' opened as READ ONLY, cannot write to it", handle->namespace);
		return NVS_C_ERR_WRONG_MODE;
	}

	ERR_C_CHECK_ERROR(nvs_set_blob(handle->esp_nvs, key, data, temp_buflen),
					  LOG_ERROR("error %d when writing data with key '%s' into namespace '%s': %s", err, key,
								handle->namespace, error_to_name(err)));

	ERR_C_CHECK_ERROR(nvs_commit(handle->esp_nvs),
					  LOG_ERROR("error %d when committing data with key '%s' into namespace "
								"'%s': %s",
								err, key, handle->namespace, error_to_name(err)););

	LOG_VERBOSE("written blob data bytes %llu into namespace '%s'", temp_buflen, handle->namespace);
	return err;
}

int nvs_c_read_blob(nvs_c_handle_t handle, const char *key, void *buffer, size_t buflen) {
	err_c_t err = ERR_C_OK;
	size_t temp_buflen = 0;
	ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("nvs handle cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(key, LOG_ERROR("key to write in NVS cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(buffer, LOG_ERROR("buffer to store read data cannot be NULL"));

	err = nvs_get_blob(handle->esp_nvs, key, NULL, &temp_buflen);
	if (err == ESP_ERR_NVS_NOT_FOUND) {
		LOG_WARN("error %d, not found key \'%s\' within namespace \'%s\', error: %s", err, key, handle->namespace,
				 error_to_name(err));
		return NVS_C_ERR_KEY_NOT_FOUND;
	} else if (temp_buflen != 0 && temp_buflen > buflen) {
		LOG_ERROR(
			"buffer size (%llu bytes) is to small to store blob data, expected "
			"%llu bytes size.",
			buflen, temp_buflen);
		return ERR_C_NO_MEMORY;
	}

	temp_buflen = buflen;
	err = nvs_get_blob(handle->esp_nvs, key, buffer, &temp_buflen);
	if (err == ESP_ERR_NVS_NOT_FOUND) {
		LOG_WARN("error %d, not found key \'%s\' within namespace \'%s\', error: %s", err, key, handle->namespace,
				 error_to_name(err));
		return NVS_C_ERR_KEY_NOT_FOUND;
	} else if (err != ERR_C_OK) {
		LOG_ERROR(
			"error %d when writing data with key: \'%s\' into \'%s\' "
			"namespace: %s",
			err, key, &(handle->namespace[0]), error_to_name(err));
		return err;
	}

	LOG_VERBOSE("read blob data bytes %llu from namespace: '%s'", temp_buflen, handle->namespace);
	return err;
}

int nvs_c_read_blob_length(nvs_c_handle_t handle, const char *key, size_t *out_datalen) {
	err_c_t err = ERR_C_OK;
	ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("nvs handle cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(key, LOG_ERROR("key to write in NVS cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(out_datalen, LOG_ERROR("buffer to store read data length cannot be NULL"));

	err = nvs_get_blob(handle->esp_nvs, key, NULL, out_datalen);
	if (err == ESP_ERR_NVS_NOT_FOUND) {
		LOG_WARN("error %d, not found key \'%s\' within namespace \'%s\', error: %s", err, key, handle->namespace,
				 error_to_name(err));
		return NVS_C_ERR_KEY_NOT_FOUND;
	}

	LOG_VERBOSE("blob with key '%s' from namespace '%s' length is %llu", key, handle->namespace, *out_datalen);
	return err;
}

int nvs_c_read_string_length(nvs_c_handle_t handle, const char *key, size_t *out_datalen) {
	err_c_t err = ERR_C_OK;
	ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("nvs handle cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(key, LOG_ERROR("key to write in NVS cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(out_datalen, LOG_ERROR("buffer to store read data length cannot be NULL"));

	err = nvs_get_str(handle->esp_nvs, key, NULL, out_datalen);
	if (err == ESP_ERR_NVS_NOT_FOUND) {
		LOG_WARN("error %d, not found key \'%s\' within namespace \'%s\', error: %s", err, key, handle->namespace,
				 error_to_name(err));
		return NVS_C_ERR_KEY_NOT_FOUND;
	}

	LOG_VERBOSE("string with key '%s' from namespace '%s' length is %llu", key, handle->namespace, *out_datalen);
	return err;
}

int nvs_c_erase_key(nvs_c_handle_t handle, const char *key) {
	err_c_t err = ERR_C_OK;
	ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("NVS handle cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(key, LOG_ERROR("key cannot be NULL"));

	if (handle->mode == NVS_C_READONLY) {
		LOG_WARN("cannot erase key, namespace '%s' was opened in read only mode", handle->namespace);
		return NVS_C_ERR_WRONG_MODE;
	}

	err = nvs_erase_key(handle->esp_nvs, key);
	if (err == ESP_ERR_NVS_NOT_FOUND) {
		LOG_ERROR("key '%s' not found in nvs namespace '%s'", key, handle->namespace);
		return NVS_C_ERR_KEY_NOT_FOUND;
	} else if (err != ERR_C_OK) {
		LOG_ERROR("error %d when erasing key '%s' from namespace '%s': %s", err, key, handle->namespace,
				  error_to_name(err));
		return err;
	}

	ERR_C_CHECK_ERROR(nvs_commit(handle->esp_nvs), LOG_ERROR("error %d when committing to namespace '%s': %s", err,
															 handle->namespace, error_to_name(err)));

	LOG_DEBUG("erased key '%s' in namespace: '%s'", key, handle->namespace);
	return err;
}

int nvs_c_erase_namespace(nvs_c_handle_t handle) {
	err_c_t err = ERR_C_OK;
	ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("NVS handle cannot be NULL"));

	if (handle->mode == NVS_C_READONLY) {
		LOG_WARN("cannot erase, namespace '%s' was opened in read only mode", handle->namespace);
		return NVS_C_ERR_WRONG_MODE;
	}

	ERR_C_CHECK_ERROR(nvs_erase_all(handle->esp_nvs), LOG_ERROR("error %d when erasing namespace '%s': %s", err,
																handle->namespace, error_to_name(err)));

	ERR_C_CHECK_ERROR(nvs_commit(handle->esp_nvs), LOG_ERROR("error %d when committing to namespace '%s': %s", err,
															 handle->namespace, error_to_name(err)));

	LOG_DEBUG("erased all key & value pairs in namespace: '%s'", handle->namespace);
	return err;
}

int nvs_c_erase_default_partition(void) {
	err_c_t err = ERR_C_OK;

	err = nvs_flash_erase();
	if (err == ESP_ERR_NOT_FOUND) {
		LOG_ERROR("not found default nvs partition");
		return NVS_C_ERR_PARTITION_NOT_FOUND;
	} else if (err != ERR_C_OK) {
		LOG_ERROR("error %d when trying to erase default nvs partition: %s", err, error_to_name(err));
		return err;
	}

	LOG_DEBUG("erased all data in default nvs partition");
	return err;
}

int nvs_c_erase_partition(const char *partition_label) {
	err_c_t err = ERR_C_OK;
	ERR_C_CHECK_NULL_PTR(partition_label, LOG_ERROR("partition_label cannot be NULL"));

	err = nvs_flash_erase_partition(partition_label);
	if (err == ESP_ERR_NOT_FOUND) {
		LOG_ERROR("not found '%s' nvs partition", partition_label);
		return NVS_C_ERR_PARTITION_NOT_FOUND;
	} else if (err != ERR_C_OK) {
		LOG_ERROR("error %d when trying to erase '%s' nvs partition", err, partition_label);
		return err;
	}

	LOG_DEBUG("erased all data in '%s' nvs partition", partition_label);
	return err;
}

int nvs_c_deinit_default_partition(void) {
	err_c_t err = ERR_C_OK;

	err = nvs_flash_deinit();
	if (err == ESP_ERR_NVS_NOT_INITIALIZED) {
		LOG_WARN("default nvs partition not initialized");
		return NVS_C_ERR_NOT_INIT;
	}

	LOG_VERBOSE("default nvs partition was deinit");
	return err;
}

int nvs_c_deinit_partition(const char *partition_label) {
	err_c_t err = ERR_C_OK;

	ERR_C_CHECK_NULL_PTR(partition_label, LOG_ERROR("nvs partition label cannot be NULL"));

	err = nvs_flash_deinit_partition(partition_label);
	if (err == ESP_ERR_NVS_NOT_INITIALIZED) {
		LOG_WARN("nvs partition '%s' not initialized", partition_label);
		return NVS_C_ERR_NOT_INIT;
	}

	LOG_VERBOSE("nvs partition '%s' was deinit", partition_label);
	return err;
}

#endif	// ESP_PLATFORM
