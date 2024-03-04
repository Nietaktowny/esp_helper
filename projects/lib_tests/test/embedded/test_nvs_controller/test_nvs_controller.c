#include "unity.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

// Change fatal color to purple, to use it to log test names.
#include "ansi_colors.h"
#define FATAL_COLOR PURPLE_BOLD

#include "nvs_controller.h"
#include "logger.h"
#include "err_controller.h"

#include "nvs.h"
#include "nvs_flash.h"
#include "string.h"

void setUp(void)
{
    // set stuff up here
    logger_init();
    logger_set_log_level(LOG_LEVEL_VERBOSE);
}

void tearDown(void)
{
    // clean stuff up here
}

void test_if_nvs_init_returns_zero(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = 0;

    // when
    err = nvs_c_init_default_partition();

    // then
    TEST_ASSERT_EQUAL_MESSAGE(0, err, "nvs_c_init_default_partition should return 0");

    // after
    nvs_c_deinit_default_partition();
}

void test_if_nvs_init_partition_returns_err_when_partition_not_found(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = 0;

    // when
    err = nvs_c_init_partition("partition");

    // then
    TEST_ASSERT_EQUAL_MESSAGE(NVS_C_ERR_PARTITION_NOT_FOUND, err, "nvs_c_init_partition should return NVS_C_ERR_NOT_FOUND when partition not found");
}

void test_if_nvs_init_partition_returns_zero(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = 0;

    // when
    err = nvs_c_init_partition("nvs");

    // then
    TEST_ASSERT_EQUAL_MESSAGE(0, err, "nvs_c_init_partition should return zero");

    // after
    nvs_c_deinit_default_partition();
}

void test_if_nvs_init_partition_returns_err_on_null_label(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = 0;

    // when
    err = nvs_c_init_partition(NULL);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_init_partition should return err on null label");
}

void test_if_nvs_err_base_is_defined(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

// then
#ifndef NVS_C_ERR_BASE
    TEST_FAIL_MESSAGE("NVS_C_ERR_BASE should be defined");
#endif
}

void test_if_nvs_err_wrong_mode_is_defined(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

// then
#ifndef NVS_C_ERR_WRONG_MODE
    TEST_FAIL_MESSAGE("NVS_C_ERR_WRONG_MODE should be defined");
#endif
}

void test_if_nvs_err_nvs_not_init_is_defined(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

// then
#ifndef NVS_C_ERR_NOT_INIT
    TEST_FAIL_MESSAGE("NVS_C_ERR_NOT_INIT should be defined");
#endif
}

void test_if_nvs_err_nvs_partition_not_found_is_defined(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

// then
#ifndef NVS_C_ERR_PARTITION_NOT_FOUND
    TEST_FAIL_MESSAGE("NVS_C_ERR_PARTITION_NOT_FOUND should be defined");
#endif
}

void test_if_err_to_name_returns_string_on_nvs_err_partition_not_found(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // when
    const char *exp_name = "NVS partition was not found.";
    const char *error_name = error_to_name(NVS_C_ERR_PARTITION_NOT_FOUND);

    // then
    TEST_ASSERT_EQUAL_STRING_MESSAGE(exp_name, error_name, "error_to_name error string different from expected");
}

void test_if_err_to_name_returns_string_on_nvs_err_wrong_mode(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // when
    const char *exp_name = "NVS namespace was opened in read only mode.";
    const char *error_name = error_to_name(NVS_C_ERR_WRONG_MODE);

    // then
    TEST_ASSERT_EQUAL_STRING_MESSAGE(exp_name, error_name, "error_to_name error string different from expected");
}

void test_if_err_to_name_returns_string_on_nvs_err_not_init(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // when
    const char *exp_name = "NVS was not init.";
    const char *error_name = error_to_name(NVS_C_ERR_NOT_INIT);

    // then
    TEST_ASSERT_EQUAL_STRING_MESSAGE(exp_name, error_name, "error_to_name error string different from expected");
}

void test_if_nvs_open_returns_err_on_null_handle(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    const char *namespace = "namespace";

    // when
    err = nvs_c_open(NULL, namespace, NVS_C_READWRITE);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_open should return err on NULL handle");
}

void test_if_nvs_open_returns_err_on_null_namespace(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    nvs_c_handle_t handle = NULL;

    // when
    err = nvs_c_open(&handle, NULL, NVS_C_READWRITE);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_open should return err on NULL namespace");
}

void test_if_nvs_open_returns_zero(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    nvs_c_handle_t handle = NULL;
    const char *namespace = "nvs";

    // when
    nvs_c_init_default_partition();
    err = nvs_c_open(&handle, namespace, NVS_C_READWRITE);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "nvs_c_open should return zero");

    // after
    nvs_c_deinit_default_partition();
}

void test_if_nvs_err_nvs_namespace_not_found_is_defined(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

// then
#ifndef NVS_C_ERR_NAMESPACE_NOT_FOUND
    TEST_FAIL_MESSAGE("NVS_C_ERR_NAMESPACE_NOT_FOUND should be defined");
#endif
}

void test_if_nvs_err_nvs_key_not_found_is_defined(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

// then
#ifndef NVS_C_ERR_KEY_NOT_FOUND
    TEST_FAIL_MESSAGE("NVS_C_ERR_KEY_NOT_FOUND should be defined");
#endif
}

void test_if_err_to_name_returns_string_on_nvs_err_namespace_not_found(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // when
    const char *exp_name = "NVS namespace was not found in partition.";
    const char *error_name = error_to_name(NVS_C_ERR_NAMESPACE_NOT_FOUND);

    // then
    TEST_ASSERT_EQUAL_STRING_MESSAGE(exp_name, error_name, "error_to_name error string different from expected");
}

void test_if_err_to_name_returns_string_on_nvs_err_key_not_found(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // when
    const char *exp_name = "NVS key was not found in namespace.";
    const char *error_name = error_to_name(NVS_C_ERR_KEY_NOT_FOUND);

    // then
    TEST_ASSERT_EQUAL_STRING_MESSAGE(exp_name, error_name, "error_to_name error string different from expected");
}

void test_if_nvs_open_returns_err_when_partition_not_init(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    nvs_c_handle_t handle = NULL;
    const char *namespace = "nvs";

    // when
    err = nvs_c_open(&handle, namespace, NVS_C_READWRITE);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(NVS_C_ERR_NOT_INIT, err, "nvs_c_open should return err when default partition not init");
}

void test_if_nvs_open_from_partition_returns_err_when_partition_not_init(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    nvs_c_handle_t handle = NULL;
    const char *namespace = "test";
    const char *partition_label = "nvs";

    // when
    err = nvs_c_open_from_partition(&handle, partition_label, namespace, NVS_C_READWRITE);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(NVS_C_ERR_NOT_INIT, err, "nvs_c_open_from_partition should return err when partition not init");
}

void test_if_nvs_open_from_partition_returns_zero(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    nvs_c_handle_t handle = NULL;
    const char *namespace = "test";
    const char *partition_label = "nvs";

    // when
    nvs_c_init_partition(partition_label);
    err = nvs_c_open_from_partition(&handle, partition_label, namespace, NVS_C_READWRITE);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "nvs_c_open_from_partition should return zero");

    // after
    nvs_c_deinit_partition(partition_label);
}

void test_if_nvs_open_from_partition_returns_err_on_null_handle(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    const char *namespace = "namespace";
    const char *partition_label = "nvs";

    // when
    err = nvs_c_open_from_partition(NULL, partition_label, namespace, NVS_C_READWRITE);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_open_from_partition should return err on NULL handle");
}

void test_if_nvs_open_from_partition_returns_err_on_null_namespace(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    nvs_c_handle_t handle = NULL;
    const char *partition_label = "nvs";

    // when
    err = nvs_c_open_from_partition(&handle, partition_label, NULL, NVS_C_READWRITE);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_open_from_partition should return err on NULL namespace");
}

void test_if_nvs_open_from_partition_returns_err_on_null_partition_label(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    nvs_c_handle_t handle = NULL;
    const char *namespace = "nvs";

    // when
    err = nvs_c_open_from_partition(&handle, NULL, namespace, NVS_C_READWRITE);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_open_from_partition should return err on NULL partition label");
}

void test_if_nvs_open_stores_correct_partition_name_in_handle(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    nvs_c_handle_t handle = NULL;
    const char *namespace = "nvs";

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, namespace, NVS_C_READWRITE);

    // then
    TEST_ASSERT_EQUAL_STRING_MESSAGE(NVS_C_DEFAULT_NVS_PART_NAME, (struct nvs_c_handle_obj *)handle->partition, "nvs_c_open should store correct default nvs partition name");

    // after
    nvs_c_deinit_default_partition();
}

void test_if_nvs_open_stores_correct_namespace_name_in_handle(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    nvs_c_handle_t handle = NULL;
    const char *namespace = "nvs";

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, namespace, NVS_C_READWRITE);

    // then
    TEST_ASSERT_EQUAL_STRING_MESSAGE(namespace, (struct nvs_c_handle_obj *)handle->namespace, "nvs_c_open should store correct nvs namespace name");

    // after
    nvs_c_deinit_default_partition();
}

void test_if_nvs_open_from_partition_stores_correct_partition_name_in_handle(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    nvs_c_handle_t handle = NULL;
    const char *namespace = "namespace";
    const char *partition_label = "nvs";

    // when
    nvs_c_init_partition(partition_label);
    TEST_ASSERT_FALSE(nvs_c_open_from_partition(&handle, partition_label, namespace, NVS_C_READWRITE));

    // then
    TEST_ASSERT_EQUAL_STRING_MESSAGE(partition_label, (struct nvs_c_handle_obj *)handle->partition, "nvs_c_open_from_partition should store correct nvs partition name");

    // after
    nvs_c_deinit_partition(partition_label);
}

void test_if_nvs_open_from_partition_stores_correct_namespace_name_in_handle(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    nvs_c_handle_t handle = NULL;
    const char *namespace = "namespace";
    const char *partition_label = "nvs";

    // when
    nvs_c_init_partition(partition_label);
    nvs_c_open_from_partition(&handle, partition_label, namespace, NVS_C_READWRITE);

    // then
    TEST_ASSERT_EQUAL_STRING_MESSAGE(namespace, (struct nvs_c_handle_obj *)handle->namespace, "nvs_c_open_from_partition should store correct nvs namespace name");

    // after
    nvs_c_deinit_partition(partition_label);
}

void test_if_nvs_init_partition_returns_wrong_args_err_name_on_too_long_partition_name(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    const char *partition_label = "asdasdasdasdasdafafwqfqfasadsasdasd";
    const char *expected_error_name = error_to_name(ERR_C_WRONG_ARGS);

    // when
    err = nvs_c_init_partition(partition_label);

    // then
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_error_name, error_to_name(err), "nvs_c_init_partition should return correct error name on too long partition name");
}

void test_if_nvs_init_partition_returns_wrong_args_err_on_too_long_partition_name(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    const char *partition_label = "asdasdasdasdasdafafwqfqfasadsasdasd";

    // when
    err = nvs_c_init_partition(partition_label);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_WRONG_ARGS, err, "nvs_c_init_partition should return ERR_C_WRONG_ARGS on too long partition name");
}

void test_if_nvs_open_from_partition_read_only_returns_err_on_partition_not_found(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    nvs_c_handle_t handle = NULL;
    const char *namespace = "poerteri";
    const char *partition_label = "nvs";

    // when
    nvs_c_init_partition(partition_label);
    err = nvs_c_open_from_partition(&handle, partition_label, namespace, NVS_C_READONLY);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(NVS_C_ERR_NAMESPACE_NOT_FOUND, err, "nvs_c_open_from_partition when read only mode is used should return err when namespace not found");

    // after
    nvs_c_deinit_partition(partition_label);
}

void test_if_nvs_close_should_return_err_on_null_handle(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;

    // when
    err = nvs_c_close(NULL);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_close should return err on NULL handle");
}

void test_if_nvs_close_should_return_zero(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;

    // when
    nvs_c_handle_t handle = malloc(sizeof(struct nvs_c_handle_obj));
    err = nvs_c_close(&handle);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "nvs_c_close should return zero");
}

void test_if_nvs_c_write_string_should_return_err_on_null_handle(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    const char *key = "key";
    char buffer[10] = {0};

    // when
    err = nvs_c_write_string(NULL, key, buffer);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_write_string should return err on NULL handle");
}

void test_if_nvs_c_write_string_should_return_err_on_null_key(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    char buffer[10] = {0};
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;

    // when
    err = nvs_c_write_string(handle, NULL, buffer);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_write_string should return err on NULL key");
}

void test_if_nvs_c_write_string_should_return_err_on_null_buffer(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    const char *key = "key";
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;

    // when
    err = nvs_c_write_string(handle, key, NULL);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_write_string should return err on NULL buffer");
}

void test_if_nvs_c_write_string_should_return_err_read_only_namespace(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    const char *key = "key";
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;
    char buffer[15] = {0};

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "nvs", NVS_C_READWRITE);
    nvs_c_close(&handle);
    nvs_c_open(&handle, "nvs", NVS_C_READONLY);
    err = nvs_c_write_string(handle, key, buffer);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(NVS_C_ERR_WRONG_MODE, err, "nvs_c_write_string should return err on read only mode");

    // after
    nvs_c_deinit_default_partition();
}

void test_if_nvs_c_write_string_should_return_zero(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    const char *key = "key";
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;
    char buffer[15] = "data";

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "nvs", NVS_C_READWRITE);
    err = nvs_c_write_string(handle, key, buffer);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "nvs_c_write_string should return ERR_C_OK");

    // after
    nvs_c_deinit_default_partition();
}

void test_if_written_string_can_be_read(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    const char *key = "key";
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;
    char written[] = "data";
    char read[15] = {0};

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "nvs", NVS_C_READWRITE);
    nvs_c_write_string(handle, key, written);
    nvs_c_read_string(handle, key, read, sizeof(read));
    

    // then
    TEST_ASSERT_EQUAL_STRING_MESSAGE(written, read, "nvs_c_write_string should return ERR_C_OK");

    // after
    nvs_c_erase_namespace(handle);
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_read_string_returns_zero(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    const char *key = "key";
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;
    char written[] = "data";
    char read[15] = {0};

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "nvs", NVS_C_READWRITE);
    nvs_c_write_string(handle, key, written);
    err = nvs_c_read_string(handle, key, read, sizeof(read));

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "nvs_c_read_string should return ERR_C_OK");

    // after
    nvs_c_erase_namespace(handle);
    nvs_c_close(&handle);
    nvs_c_deinit_default_partition();
}

void test_if_nvs_c_read_string_should_return_err_on_null_handle(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    const char *key = "key";
    char buffer[10] = {0};

    // when
    err = nvs_c_read_string(NULL, key, buffer, sizeof(buffer));

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_read_string should return err on NULL handle");
}

void test_if_nvs_c_read_string_should_return_err_on_null_key(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    char buffer[10] = {0};
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;

    // when
    err = nvs_c_read_string(handle, NULL, buffer, sizeof(buffer));

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_read_string should return err on NULL key");
}

void test_if_nvs_c_read_string_should_return_err_on_null_buffer(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    const char *key = "key";
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;

    // when
    err = nvs_c_read_string(handle, key, NULL, 0);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_read_string should return err on NULL buffer");
}

void test_if_nvs_c_erase_namespace_should_return_err_on_null_handle(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;

    // when
    err = nvs_c_erase_namespace(NULL);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_erase_namespace should return err on NULL handle");
}

void test_if_nvs_c_erase_namespace_should_return_zero(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    nvs_c_handle_t handle = NULL;

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "nvs", NVS_C_READWRITE);
    err = nvs_c_erase_namespace(handle);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "nvs_c_erase_namespace should return zero");

    // after
    nvs_c_deinit_default_partition();
}

void test_if_nvs_close_should_free_handle(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    nvs_c_handle_t handle = NULL;

    // when
    handle = malloc(sizeof(struct nvs_c_handle_obj));
    nvs_c_close(&handle);

    // then
    TEST_ASSERT_NULL_MESSAGE(handle, "nvs_c_close should free handle");
}

void test_if_nvs_c_erase_namespace_should_err_on_read_only_namespace(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    nvs_c_handle_t handle = NULL;

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "nvs", NVS_C_READWRITE);
    nvs_c_close(&handle);
    nvs_c_open(&handle, "nvs", NVS_C_READONLY);
    err = nvs_c_erase_namespace(handle);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(NVS_C_ERR_WRONG_MODE, err, "nvs_c_erase_namespace should return err on read only namespace");

    // after
    nvs_c_deinit_default_partition();
}

void test_if_nvs_c_deinit_default_partition_should_return_err_on_not_init_partition(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;

    // when
    err = nvs_c_deinit_default_partition();

    // then
    TEST_ASSERT_EQUAL_MESSAGE(NVS_C_ERR_NOT_INIT, err, "nvs_c_deinit_default_partition should return err on not init nvs partition");
}

void test_if_nvs_c_deinit_partition_should_return_err_on_not_init_partition(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    const char *partition_label = "nvs";

    // when
    err = nvs_c_deinit_partition(partition_label);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(NVS_C_ERR_NOT_INIT, err, "nvs_c_deinit_partition should return err on not init nvs partition");
}

void test_if_nvs_c_deinit_default_partition_should_return_zero(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;

    // when
    nvs_c_init_default_partition();
    err = nvs_c_deinit_default_partition();

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "nvs_c_deinit_default_partition should return zero");
}

void test_if_nvs_c_deinit_partition_should_return_zero(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    const char *partition_label = "nvs";

    // when
    nvs_c_init_partition(partition_label);
    err = nvs_c_deinit_partition(partition_label);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "nvs_c_deinit_partition should return zero");
}

void test_if_nvs_c_write_int8_should_return_err_on_null_handle(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    const char *key = "key";
    int8_t value = 12;

    // when
    err = nvs_c_write_int8(NULL, key, value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_write_int8 should return err on NULL handle");
}

void test_if_nvs_c_write_int8_should_return_err_on_null_key(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    int8_t value = 12;
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;

    // when
    err = nvs_c_write_int8(handle, NULL, value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_write_int8 should return err on NULL key");
}

void test_if_nvs_c_write_int8_returns_zero(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    int8_t value = 12;
    nvs_c_handle_t handle = NULL;
    const char *key = "key";

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "test", NVS_C_READWRITE);
    err = nvs_c_write_int8(handle, key, value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "nvs_c_write_int8 should return err on NULL key");

    // after
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_read_int8_should_return_err_on_null_handle(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    const char *key = "key";
    int8_t value = 12;

    // when
    err = nvs_c_read_int8(NULL, key, &value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_read_int8 should return err on NULL handle");
}

void test_if_nvs_c_read_int8_should_return_err_on_null_key(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    int8_t value = 12;
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;

    // when
    err = nvs_c_read_int8(handle, NULL, &value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_read_int8 should return err on NULL key");
}

void test_if_nvs_c_read_int8_return_err_on_null_buffer(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    nvs_c_handle_t handle = NULL;
    const char *key = "key";

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "test", NVS_C_READWRITE);
    err = nvs_c_read_int8(handle, key, NULL);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_read_int8 should return err on NULL buffer");

    // after
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_read_int8_returns_zero(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    int8_t value = 12;
    nvs_c_handle_t handle = NULL;
    const char *key = "key";

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "test", NVS_C_READWRITE);
    nvs_c_write_int8(handle, key, value);
    err = nvs_c_read_int8(handle, key, &value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "nvs_c_read_int8 should return err on NULL key");

    // after
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_write_int8_returns_err_on_read_only_namespace(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    int8_t value = 12;
    nvs_c_handle_t handle = NULL;

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "test", NVS_C_READWRITE);
    nvs_c_close(&handle);
    nvs_c_open(&handle, "test", NVS_C_READONLY);
    err = nvs_c_write_int8(handle, NULL, value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(NVS_C_ERR_WRONG_MODE, err, "nvs_c_write_int8 should return err on read only namespace mode");

    // after
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_write_int8_writes_data(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int8_t value = 12;
    int8_t buffer = 0;
    nvs_c_handle_t handle = NULL;
    const char *key = "key";

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "test", NVS_C_READWRITE);
    nvs_c_write_int8(handle, key, value);
    nvs_c_read_int8(handle, key, &buffer);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(value, buffer, "nvs_c_write_int8 should write correct value");

    // after
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_write_int16_should_return_err_on_null_handle(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    const char *key = "key";
    int16_t value = 12;

    // when
    err = nvs_c_write_int16(NULL, key, value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_write_int16 should return err on NULL handle");
}

void test_if_nvs_c_write_int16_should_return_err_on_null_key(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    int16_t value = 12;
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;

    // when
    err = nvs_c_write_int16(handle, NULL, value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_write_int16 should return err on NULL key");
}

void test_if_nvs_c_write_int16_returns_zero(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    int16_t value = 12;
    nvs_c_handle_t handle = NULL;
    const char *key = "key";

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "test", NVS_C_READWRITE);
    err = nvs_c_write_int16(handle, key, value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "nvs_c_write_int16 should return err on NULL key");

    // after
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_write_int16_returns_err_on_read_only_namespace(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    int16_t value = 12;
    nvs_c_handle_t handle = NULL;

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "test", NVS_C_READWRITE);
    nvs_c_close(&handle);
    nvs_c_open(&handle, "test", NVS_C_READONLY);
    err = nvs_c_write_int16(handle, NULL, value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(NVS_C_ERR_WRONG_MODE, err, "nvs_c_write_int16 should return err on read only namespace mode");

    // after
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_read_int16_should_return_err_on_null_handle(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    const char *key = "key";
    int16_t value = 12;

    // when
    err = nvs_c_read_int16(NULL, key, &value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_read_int16 should return err on NULL handle");
}

void test_if_nvs_c_read_int16_should_return_err_on_null_key(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    int16_t value = 12;
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;

    // when
    err = nvs_c_read_int16(handle, NULL, &value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_read_int16 should return err on NULL key");
}

void test_if_nvs_c_read_int16_returns_zero(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    int16_t value = 12;
    nvs_c_handle_t handle = NULL;
    const char *key = "key";

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "test", NVS_C_READWRITE);
    nvs_c_write_int16(handle, key, value);
    err = nvs_c_read_int16(handle, key, &value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "nvs_c_read_int16 should return err on NULL key");

    // after
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_read_int16_return_err_on_null_buffer(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    nvs_c_handle_t handle = NULL;
    const char *key = "key";

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "test", NVS_C_READWRITE);
    err = nvs_c_read_int16(handle, key, NULL);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_read_int16 should return err on NULL buffer");

    // after
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_write_int16_writes_data(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int16_t value = 12;
    int16_t buffer = 0;
    nvs_c_handle_t handle = NULL;
    const char *key = "key";

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "test", NVS_C_READWRITE);
    nvs_c_write_int16(handle, key, value);
    nvs_c_read_int16(handle, key, &buffer);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(value, buffer, "nvs_c_write_int16 should write correct value");

    // after
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_write_int32_should_return_err_on_null_handle(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    const char *key = "key";
    int32_t value = 12;

    // when
    err = nvs_c_write_int32(NULL, key, value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_write_int32 should return err on NULL handle");
}

void test_if_nvs_c_write_int32_should_return_err_on_null_key(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    int32_t value = 12;
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;

    // when
    err = nvs_c_write_int32(handle, NULL, value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_write_int32 should return err on NULL key");
}

void test_if_nvs_c_write_int32_returns_zero(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    int32_t value = 12;
    nvs_c_handle_t handle = NULL;
    const char *key = "key";

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "test", NVS_C_READWRITE);
    err = nvs_c_write_int32(handle, key, value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "nvs_c_write_int32 should return err on NULL key");

    // after
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_write_int32_returns_err_on_read_only_namespace(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    int32_t value = 12;
    nvs_c_handle_t handle = NULL;

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "test", NVS_C_READWRITE);
    nvs_c_close(&handle);
    nvs_c_open(&handle, "test", NVS_C_READONLY);
    err = nvs_c_write_int32(handle, NULL, value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(NVS_C_ERR_WRONG_MODE, err, "nvs_c_write_int32 should return err on read only namespace mode");

    // after
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_read_int32_should_return_err_on_null_handle(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    const char *key = "key";
    int32_t value = 12;

    // when
    err = nvs_c_read_int32(NULL, key, &value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_read_int32 should return err on NULL handle");
}

void test_if_nvs_c_read_int32_should_return_err_on_null_key(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    int32_t value = 12;
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;

    // when
    err = nvs_c_read_int32(handle, NULL, &value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_read_int32 should return err on NULL key");
}

void test_if_nvs_c_read_int32_returns_zero(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    int32_t value = 12;
    nvs_c_handle_t handle = NULL;
    const char *key = "key";

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "test", NVS_C_READWRITE);
    nvs_c_write_int32(handle, key, value);
    err = nvs_c_read_int32(handle, key, &value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "nvs_c_read_int32 should return err on NULL key");

    // after
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_read_int32_return_err_on_null_buffer(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    nvs_c_handle_t handle = NULL;
    const char *key = "key";

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "test", NVS_C_READWRITE);
    err = nvs_c_read_int32(handle, key, NULL);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_read_int32 should return err on NULL buffer");

    // after
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_write_int32_writes_data(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int32_t value = 12;
    int32_t buffer = 0;
    nvs_c_handle_t handle = NULL;
    const char *key = "key";

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "test", NVS_C_READWRITE);
    nvs_c_write_int32(handle, key, value);
    nvs_c_read_int32(handle, key, &buffer);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(value, buffer, "nvs_c_write_int32 should write correct value");

    // after
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_write_int64_should_return_err_on_null_handle(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    const char *key = "key";
    int64_t value = 12;

    // when
    err = nvs_c_write_int64(NULL, key, value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_write_int64 should return err on NULL handle");
}

void test_if_nvs_c_write_int64_should_return_err_on_null_key(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    int64_t value = 12;
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;

    // when
    err = nvs_c_write_int64(handle, NULL, value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_write_int64 should return err on NULL key");
}

void test_if_nvs_c_write_int64_returns_zero(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    int64_t value = 1255523;
    nvs_c_handle_t handle = NULL;
    const char *key = "key";

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "int64_t", NVS_C_READWRITE);
    err = nvs_c_write_int64(handle, key, value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "nvs_c_write_int64 should return err on NULL key");

    // after
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_write_int64_returns_err_on_read_only_namespace(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    int64_t value = 12;
    nvs_c_handle_t handle = NULL;

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "int64_t", NVS_C_READWRITE);
    nvs_c_close(&handle);
    nvs_c_open(&handle, "int64_t", NVS_C_READONLY);
    err = nvs_c_write_int64(handle, NULL, value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(NVS_C_ERR_WRONG_MODE, err, "nvs_c_write_int64 should return err on read only namespace mode");

    // after
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_read_int64_should_return_err_on_null_handle(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    const char *key = "key";
    int64_t value = 12;

    // when
    err = nvs_c_read_int64(NULL, key, &value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_read_int64 should return err on NULL handle");
}

void test_if_nvs_c_read_int64_should_return_err_on_null_key(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    int64_t value = 12;
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;

    // when
    err = nvs_c_read_int64(handle, NULL, &value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_read_int64 should return err on NULL key");
}

void test_if_nvs_c_read_int64_returns_zero(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    int64_t value = 12;
    nvs_c_handle_t handle = NULL;
    const char *key = "key";

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "int64_t", NVS_C_READWRITE);
    nvs_c_write_int64(handle, key, value);
    err = nvs_c_read_int64(handle, key, &value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "nvs_c_read_int64 should return err on NULL key");

    // after
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_read_int64_return_err_on_null_buffer(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    nvs_c_handle_t handle = NULL;
    const char *key = "key";

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "int64_t", NVS_C_READWRITE);
    err = nvs_c_read_int64(handle, key, NULL);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_read_int64 should return err on NULL buffer");

    // after
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_write_int64_writes_data(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int64_t value = 12;
    int64_t buffer = 0;
    nvs_c_handle_t handle = NULL;
    const char *key = "key";

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "int64_t", NVS_C_READWRITE);
    nvs_c_write_int64(handle, key, value);
    nvs_c_read_int64(handle, key, &buffer);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(value, buffer, "nvs_c_write_int64 should write correct value");

    // after
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_write_uint8_should_return_err_on_null_handle(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    const char *key = "key";
    uint8_t value = 12;

    // when
    err = nvs_c_write_uint8(NULL, key, value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_write_uint8 should return err on NULL handle");
}

void test_if_nvs_c_write_uint8_should_return_err_on_null_key(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    uint8_t value = 12;
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;

    // when
    err = nvs_c_write_uint8(handle, NULL, value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_write_uint8 should return err on NULL key");
}

void test_if_nvs_c_write_uint8_returns_zero(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    uint8_t value = 12;
    nvs_c_handle_t handle = NULL;
    const char *key = "key";

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "test", NVS_C_READWRITE);
    err = nvs_c_write_uint8(handle, key, value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "nvs_c_write_uint8 should return err on NULL key");

    // after
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_read_uint8_should_return_err_on_null_handle(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    const char *key = "key";
    uint8_t value = 12;

    // when
    err = nvs_c_read_uint8(NULL, key, &value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_read_uint8 should return err on NULL handle");
}

void test_if_nvs_c_read_uint8_should_return_err_on_null_key(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    uint8_t value = 12;
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;

    // when
    err = nvs_c_read_uint8(handle, NULL, &value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_read_uint8 should return err on NULL key");
}

void test_if_nvs_c_read_uint8_return_err_on_null_buffer(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    nvs_c_handle_t handle = NULL;
    const char *key = "key";

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "test", NVS_C_READWRITE);
    err = nvs_c_read_uint8(handle, key, NULL);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_read_uint8 should return err on NULL buffer");

    // after
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_read_uint8_returns_zero(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    uint8_t value = 12;
    nvs_c_handle_t handle = NULL;
    const char *key = "key";

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "test", NVS_C_READWRITE);
    nvs_c_write_uint8(handle, key, value);
    err = nvs_c_read_uint8(handle, key, &value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "nvs_c_read_uint8 should return err on NULL key");

    // after
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_write_uint8_returns_err_on_read_only_namespace(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    uint8_t value = 12;
    nvs_c_handle_t handle = NULL;

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "test", NVS_C_READWRITE);
    nvs_c_close(&handle);
    nvs_c_open(&handle, "test", NVS_C_READONLY);
    err = nvs_c_write_uint8(handle, NULL, value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(NVS_C_ERR_WRONG_MODE, err, "nvs_c_write_uint8 should return err on read only namespace mode");

    // after
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_write_uint8_writes_data(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    uint8_t value = 12;
    uint8_t buffer = 0;
    nvs_c_handle_t handle = NULL;
    const char *key = "key";

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "test", NVS_C_READWRITE);
    nvs_c_write_uint8(handle, key, value);
    nvs_c_read_uint8(handle, key, &buffer);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(value, buffer, "nvs_c_write_uint8 should write correct value");

    // after
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_write_uint16_should_return_err_on_null_handle(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    const char *key = "key";
    uint16_t value = 12;

    // when
    err = nvs_c_write_uint16(NULL, key, value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_write_uint16 should return err on NULL handle");
}

void test_if_nvs_c_write_uint16_should_return_err_on_null_key(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    uint16_t value = 12;
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;

    // when
    err = nvs_c_write_uint16(handle, NULL, value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_write_uint16 should return err on NULL key");
}

void test_if_nvs_c_write_uint16_returns_zero(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    uint16_t value = 12;
    nvs_c_handle_t handle = NULL;
    const char *key = "key";

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "test", NVS_C_READWRITE);
    err = nvs_c_write_uint16(handle, key, value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "nvs_c_write_uint16 should return err on NULL key");

    // after
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_write_uint16_returns_err_on_read_only_namespace(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    uint16_t value = 12;
    nvs_c_handle_t handle = NULL;

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "test", NVS_C_READWRITE);
    nvs_c_close(&handle);
    nvs_c_open(&handle, "test", NVS_C_READONLY);
    err = nvs_c_write_uint16(handle, NULL, value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(NVS_C_ERR_WRONG_MODE, err, "nvs_c_write_uint16 should return err on read only namespace mode");

    // after
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_read_uint16_should_return_err_on_null_handle(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    const char *key = "key";
    uint16_t value = 12;

    // when
    err = nvs_c_read_uint16(NULL, key, &value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_read_uint16 should return err on NULL handle");
}

void test_if_nvs_c_read_uint16_should_return_err_on_null_key(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    uint16_t value = 12;
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;

    // when
    err = nvs_c_read_uint16(handle, NULL, &value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_read_uint16 should return err on NULL key");
}

void test_if_nvs_c_read_uint16_returns_zero(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    uint16_t value = 12;
    nvs_c_handle_t handle = NULL;
    const char *key = "key";

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "test", NVS_C_READWRITE);
    nvs_c_write_uint16(handle, key, value);
    err = nvs_c_read_uint16(handle, key, &value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "nvs_c_read_uint16 should return err on NULL key");

    // after
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_read_uint16_return_err_on_null_buffer(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    nvs_c_handle_t handle = NULL;
    const char *key = "key";

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "test", NVS_C_READWRITE);
    err = nvs_c_read_uint16(handle, key, NULL);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_read_uint16 should return err on NULL buffer");

    // after
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_write_uint16_writes_data(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    uint16_t value = 12;
    uint16_t buffer = 0;
    nvs_c_handle_t handle = NULL;
    const char *key = "key";

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "test", NVS_C_READWRITE);
    nvs_c_write_uint16(handle, key, value);
    nvs_c_read_uint16(handle, key, &buffer);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(value, buffer, "nvs_c_write_uint16 should write correct value");

    // after
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_write_uint32_should_return_err_on_null_handle(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    const char *key = "key";
    uint32_t value = 12;

    // when
    err = nvs_c_write_uint32(NULL, key, value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_write_uint32 should return err on NULL handle");
}

void test_if_nvs_c_write_uint32_should_return_err_on_null_key(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    uint32_t value = 12;
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;

    // when
    err = nvs_c_write_uint32(handle, NULL, value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_write_uint32 should return err on NULL key");
}

void test_if_nvs_c_write_uint32_returns_zero(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    uint32_t value = 12;
    nvs_c_handle_t handle = NULL;
    const char *key = "key";

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "test", NVS_C_READWRITE);
    err = nvs_c_write_uint32(handle, key, value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "nvs_c_write_uint32 should return err on NULL key");

    // after
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_write_uint32_returns_err_on_read_only_namespace(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    uint32_t value = 12;
    nvs_c_handle_t handle = NULL;

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "test", NVS_C_READWRITE);
    nvs_c_close(&handle);
    nvs_c_open(&handle, "test", NVS_C_READONLY);
    err = nvs_c_write_uint32(handle, NULL, value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(NVS_C_ERR_WRONG_MODE, err, "nvs_c_write_uint32 should return err on read only namespace mode");

    // after
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_read_uint32_should_return_err_on_null_handle(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    const char *key = "key";
    uint32_t value = 12;

    // when
    err = nvs_c_read_uint32(NULL, key, &value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_read_uint32 should return err on NULL handle");
}

void test_if_nvs_c_read_uint32_should_return_err_on_null_key(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    uint32_t value = 12;
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;

    // when
    err = nvs_c_read_uint32(handle, NULL, &value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_read_uint32 should return err on NULL key");
}

void test_if_nvs_c_read_uint32_returns_zero(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    uint32_t value = 12;
    nvs_c_handle_t handle = NULL;
    const char *key = "key";

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "test", NVS_C_READWRITE);
    nvs_c_write_uint32(handle, key, value);
    err = nvs_c_read_uint32(handle, key, &value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "nvs_c_read_uint32 should return err on NULL key");

    // after
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_read_uint32_return_err_on_null_buffer(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    nvs_c_handle_t handle = NULL;
    const char *key = "key";

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "test", NVS_C_READWRITE);
    err = nvs_c_read_uint32(handle, key, NULL);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_read_uint32 should return err on NULL buffer");

    // after
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_write_uint32_writes_data(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    uint32_t value = 12;
    uint32_t buffer = 0;
    nvs_c_handle_t handle = NULL;
    const char *key = "key";

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "test", NVS_C_READWRITE);
    nvs_c_write_uint32(handle, key, value);
    nvs_c_read_uint32(handle, key, &buffer);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(value, buffer, "nvs_c_write_uint32 should write correct value");

    // after
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_write_uint64_should_return_err_on_null_handle(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    const char *key = "key";
    uint64_t value = 12;

    // when
    err = nvs_c_write_uint64(NULL, key, value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_write_uint64 should return err on NULL handle");
}

void test_if_nvs_c_write_uint64_should_return_err_on_null_key(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    uint64_t value = 12;
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;

    // when
    err = nvs_c_write_uint64(handle, NULL, value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_write_uint64 should return err on NULL key");
}

void test_if_nvs_c_write_uint64_returns_zero(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    uint64_t value = 1255523;
    nvs_c_handle_t handle = NULL;
    const char *key = "key";

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "uint64_t", NVS_C_READWRITE);
    err = nvs_c_write_uint64(handle, key, value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "nvs_c_write_uint64 should return err on NULL key");

    // after
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_write_uint64_returns_err_on_read_only_namespace(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    uint64_t value = 12;
    nvs_c_handle_t handle = NULL;

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "uint64_t", NVS_C_READWRITE);
    nvs_c_close(&handle);
    nvs_c_open(&handle, "uint64_t", NVS_C_READONLY);
    err = nvs_c_write_uint64(handle, NULL, value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(NVS_C_ERR_WRONG_MODE, err, "nvs_c_write_uint64 should return err on read only namespace mode");

    // after
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_read_uint64_should_return_err_on_null_handle(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    const char *key = "key";
    uint64_t value = 12;

    // when
    err = nvs_c_read_uint64(NULL, key, &value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_read_uint64 should return err on NULL handle");
}

void test_if_nvs_c_read_uint64_should_return_err_on_null_key(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    uint64_t value = 12;
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;

    // when
    err = nvs_c_read_uint64(handle, NULL, &value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_read_uint64 should return err on NULL key");
}

void test_if_nvs_c_read_uint64_returns_zero(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    uint64_t value = 12;
    nvs_c_handle_t handle = NULL;
    const char *key = "key";

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "uint64_t", NVS_C_READWRITE);
    nvs_c_write_uint64(handle, key, value);
    err = nvs_c_read_uint64(handle, key, &value);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "nvs_c_read_uint64 should return err on NULL key");

    // after
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_read_uint64_return_err_on_null_buffer(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    nvs_c_handle_t handle = NULL;
    const char *key = "key";

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "uint64_t", NVS_C_READWRITE);
    err = nvs_c_read_uint64(handle, key, NULL);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_read_uint64 should return err on NULL buffer");

    // after
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_write_uint64_writes_data(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    uint64_t value = 12;
    uint64_t buffer = 0;
    nvs_c_handle_t handle = NULL;
    const char *key = "key";

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "uint64_t", NVS_C_READWRITE);
    nvs_c_write_uint64(handle, key, value);
    nvs_c_read_uint64(handle, key, &buffer);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(value, buffer, "nvs_c_write_uint64 should write correct value");

    // after
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_write_blob_should_return_err_on_null_handle(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    const char *key = "key";
    char buffer[10] = {0};

    // when
    err = nvs_c_write_blob(NULL, key, buffer, sizeof(buffer));

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_write_blob should return err on NULL handle");
}

void test_if_nvs_c_write_blob_should_return_err_on_null_key(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    char buffer[10] = {0};
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;

    // when
    err = nvs_c_write_blob(handle, NULL, buffer, sizeof(buffer));

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_write_blob should return err on NULL key");
}

void test_if_nvs_c_write_blob_should_return_err_on_null_buffer(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    const char *key = "key";
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;

    // when
    err = nvs_c_write_blob(handle, key, NULL, 0);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_write_blob should return err on NULL buffer");
}

void test_if_nvs_c_write_blob_should_return_err_read_only_namespace(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    const char *key = "key";
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;
    char buffer[15] = {0};

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "nvs", NVS_C_READWRITE);
    nvs_c_close(&handle);
    nvs_c_open(&handle, "nvs", NVS_C_READONLY);
    err = nvs_c_write_blob(handle, key, buffer, sizeof(buffer));

    // then
    TEST_ASSERT_EQUAL_MESSAGE(NVS_C_ERR_WRONG_MODE, err, "nvs_c_write_blob should return err on read only mode");

    // after
    nvs_c_deinit_default_partition();
}

void test_if_nvs_c_write_blob_should_return_zero(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    const char *key = "key";
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;
    char buffer[15] = "data";

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "nvs", NVS_C_READWRITE);
    err = nvs_c_write_blob(handle, key, buffer, sizeof(buffer));

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "nvs_c_write_blob should return ERR_C_OK");

    // after
    nvs_c_deinit_default_partition();
}

void test_if_written_blob_can_be_read(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    const char *key = "key";
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;
    char written[] = "data";
    char read[15] = {0};

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "nvs", NVS_C_READWRITE);
    nvs_c_write_blob(handle, key, written, sizeof(written));
    nvs_c_read_blob(handle, key, read, sizeof(read));
    nvs_c_close(&handle);

    // then
    TEST_ASSERT_EQUAL_STRING_MESSAGE(written, read, "nvs_c_write_blob should return ERR_C_OK");

    // after
    nvs_c_deinit_default_partition();
}

void test_if_nvs_c_read_blob_returns_zero(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    const char *key = "key";
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;
    char written[] = "data";
    char read[15] = {0};

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "nvs", NVS_C_READWRITE);
    nvs_c_write_blob(handle, key, written, sizeof(written));
    err = nvs_c_read_blob(handle, key, read, sizeof(read));

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "nvs_c_read_blob should return ERR_C_OK");

    // after
    nvs_c_erase_namespace(handle);
    nvs_c_close(&handle);
    nvs_c_deinit_default_partition();
}

void test_if_nvs_c_read_blob_should_return_err_on_null_handle(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    const char *key = "key";
    char buffer[10] = {0};

    // when
    err = nvs_c_read_blob(NULL, key, buffer, sizeof(buffer));

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_read_blob should return err on NULL handle");
}

void test_if_nvs_c_read_blob_should_return_err_on_null_key(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    char buffer[10] = {0};
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;

    // when
    err = nvs_c_read_blob(handle, NULL, buffer, sizeof(buffer));

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_read_blob should return err on NULL key");
}

void test_if_nvs_c_read_blob_should_return_err_on_null_buffer(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    const char *key = "key";
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;

    // when
    err = nvs_c_read_blob(handle, key, NULL, 0);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_read_blob should return err on NULL buffer");
}

void test_if_nvs_c_read_string_returns_err_on_too_small_buffer(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    const char *key = "key";
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;
    char written[] = "datasssssssadadsawdasdasdasda";
    char read[15] = {0};

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "nvs", NVS_C_READWRITE);
    nvs_c_write_string(handle, key, written);
    err = nvs_c_read_string(handle, key, read, sizeof(read));
    nvs_c_close(&handle);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NO_MEMORY, err, "nvs_c_read_string should return err when buffer is to small to store string");

    // after
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_read_blob_returns_err_on_too_small_buffer(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    const char *key = "key";
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;
    char written[] = "datasssssssadadsawdasdasdasda";
    char read[15] = {0};

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "nvs", NVS_C_READWRITE);
    nvs_c_write_blob(handle, key, written, strlen(written));
    err = nvs_c_read_blob(handle, key, read, sizeof(read));
    nvs_c_close(&handle);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NO_MEMORY, err, "nvs_c_read_blob should return err when buffer is to small to store blob");

    // after
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_read_string_length_should_return_err_on_null_handle(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    const char *key = "key";
    size_t size = 0;

    // when
    err = nvs_c_read_string_length(NULL, key, &size);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_read_string_length should return err on NULL handle");
}

void test_if_nvs_c_read_string_length_should_return_err_on_null_key(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    size_t size = 0;
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;

    // when
    err = nvs_c_read_string_length(handle, NULL, &size);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_read_string_length should return err on NULL key");
}

void test_if_nvs_c_read_string_length_should_return_err_on_null_size(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    const char *key = "key";
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;

    // when
    err = nvs_c_read_string_length(handle, key, NULL);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_read_string_length should return err on NULL size");
}

void test_if_nvs_c_read_string_length_returns_zero(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    const char *key = "key";
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;
    char written[] = "data";
    size_t size = 0;
    int err = ERR_C_OK;

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "nvs", NVS_C_READWRITE);
    nvs_c_write_string(handle, key, written);
    err = nvs_c_read_string_length(handle, key, &size);
    
    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "nvs_c_read_string_length should return zero");

    // after
    nvs_c_erase_namespace(handle);
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_read_string_length_returns_written_string_size(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    const char *key = "key";
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;
    char written[] = "data";
    size_t size = 0;

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "nvs", NVS_C_READWRITE);
    nvs_c_write_string(handle, key, written);
    nvs_c_read_string_length(handle, key, &size);
    
    // then
    TEST_ASSERT_EQUAL_MESSAGE(strlen(written) + 1, size, "nvs_c_read_string_length should return correct written data size");

    // after
    nvs_c_erase_namespace(handle);
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_read_blob_length_should_return_err_on_null_handle(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    const char *key = "key";
    size_t size = 0;

    // when
    err = nvs_c_read_blob_length(NULL, key, &size);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_read_blob_length should return err on NULL handle");
}

void test_if_nvs_c_read_blob_length_should_return_err_on_null_key(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    size_t size = 0;
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;

    // when
    err = nvs_c_read_blob_length(handle, NULL, &size);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_read_blob_length should return err on NULL key");
}

void test_if_nvs_c_read_blob_length_should_return_err_on_null_size(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    int err = ERR_C_OK;
    const char *key = "key";
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;

    // when
    err = nvs_c_read_blob_length(handle, key, NULL);

    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_read_blob_length should return err on NULL size");
}

void test_if_nvs_c_read_blob_length_returns_zero(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    const char *key = "key";
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;
    char written[] = "data";
    size_t size = 0;
    int err = ERR_C_OK;

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "nvs", NVS_C_READWRITE);
    nvs_c_write_blob(handle, key, written, sizeof(written));
    err = nvs_c_read_blob_length(handle, key, &size);
    
    // then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "nvs_c_read_blob_length should return zero");

    // after
    nvs_c_erase_namespace(handle);
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_read_blob_length_returns_written_blob_size(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    // given
    const char *key = "key";
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;
    char written[] = "data";
    size_t size = 0;

    // when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "nvs", NVS_C_READWRITE);
    nvs_c_write_blob(handle, key, written, sizeof(written));
    nvs_c_read_blob_length(handle, key, &size);
    
    // then
    TEST_ASSERT_EQUAL_MESSAGE(strlen(written) + 1, size, "nvs_c_read_blob_length should return correct written data size");

    // after
    nvs_c_erase_namespace(handle);
    nvs_c_close(&handle);
    nvs_c_erase_default_partition();
}

void test_if_nvs_c_erase_key_returns_err_on_null_handle(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = 0;
    const char* key = "key";

    //when
    err = nvs_c_erase_key(NULL, key);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_erase_key should return err on NULL handle");
}

void test_if_nvs_c_erase_key_returns_err_on_null_key(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = 0;
    nvs_c_handle_t handle = NULL;

    //when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "testsdsda", NVS_C_READWRITE);
    err = nvs_c_erase_key(handle, NULL);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_erase_key should return err on NULL key");

    //after
    nvs_c_close(&handle);
    nvs_c_deinit_default_partition();
}

void test_if_nvs_c_erase_key_returns_err_on_read_only_namespace(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = 0;
    const char* key = "key";
    nvs_c_handle_t handle = NULL;

    //when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "testsdsda", NVS_C_READWRITE);
	nvs_c_close(&handle);

	nvs_c_open(&handle, "testsdsda", NVS_C_READONLY);
    err = nvs_c_erase_key(handle, key);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(NVS_C_ERR_WRONG_MODE, err, "nvs_c_erase_key should return err on read only namespace");

    //after
    nvs_c_close(&handle);
    nvs_c_deinit_default_partition();
}

void test_if_nvs_c_erase_key_returns_zero(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = 0;
    const char* key = "asdwadq";
    uint8_t data = 12;
    nvs_c_handle_t handle = NULL;

    //when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "test", NVS_C_READWRITE);
    nvs_c_write_uint8(handle, key, data);
    err = nvs_c_erase_key(handle, key);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "nvs_c_erase_key should return zero");

    //after
    nvs_c_close(&handle);
    nvs_c_deinit_default_partition();
}

void test_if_nvs_c_erase_key_deletes_key(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = 0;
    const char* key = "asdwadq";
    uint8_t data = 12;
    nvs_c_handle_t handle = NULL;

    //when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "test", NVS_C_READWRITE);
    nvs_c_write_uint8(handle, key, data);
    nvs_c_erase_key(handle, key);
    err = nvs_c_read_uint8(handle, key, &data);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(NVS_C_ERR_KEY_NOT_FOUND, err, "nvs_c_erase_key should delete key");

    //after
    nvs_c_close(&handle);
    nvs_c_deinit_default_partition();
}

void test_if_nvs_c_erase_namespace_returns_err_on_null_handle(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = 0;

    //when
    err = nvs_c_erase_namespace(NULL);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_erase_namespace should return err on NULL handle");
}

void test_if_nvs_c_erase_namespace_returns_err_on_read_only_namespace(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = 0;
    nvs_c_handle_t handle = NULL;

    //when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "testsdsda", NVS_C_READWRITE);
	nvs_c_close(&handle);
	nvs_c_open(&handle, "testsdsda", NVS_C_READONLY);
    err = nvs_c_erase_namespace(handle);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(NVS_C_ERR_WRONG_MODE, err, "nvs_c_erase_namespace should return err on read only namespace");

    //after
    nvs_c_deinit_default_partition();
}

void test_if_nvs_c_erase_namespace_returns_zero(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = 0;
    nvs_c_handle_t handle = NULL;

    //when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "test", NVS_C_READWRITE);
    err = nvs_c_erase_namespace(handle);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "nvs_c_erase_namespace should return zero");

    //after
    nvs_c_close(&handle);
    nvs_c_deinit_default_partition();
}

void test_if_nvs_c_erase_namespace_deletes_key(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = 0;
    const char* key = "asdwadq";
    uint8_t data = 12;
    nvs_c_handle_t handle = NULL;

    //when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "test", NVS_C_READWRITE);
    nvs_c_write_uint8(handle, key, data);
    nvs_c_erase_namespace(handle);
    err = nvs_c_read_uint8(handle, key, &data);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(NVS_C_ERR_KEY_NOT_FOUND, err, "nvs_c_erase_namespace should delete key from namespace");

    //after
    nvs_c_close(&handle);
    nvs_c_deinit_default_partition();
}

void test_if_nvs_c_erase_default_partition_returns_zero(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = 0;

    //when
    nvs_c_init_default_partition();
    err = nvs_c_erase_default_partition();

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "nvs_c_erase_default_partition should return zero");

    //after
    nvs_c_deinit_default_partition();
}

void test_if_nvs_c_erase_default_partition_deletes_namespace(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = 0;
    nvs_c_handle_t handle = NULL;

    //when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "test", NVS_C_READWRITE);

    nvs_c_erase_default_partition();

    nvs_c_init_default_partition();
    err = nvs_c_open(&handle, "test", NVS_C_READONLY);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(NVS_C_ERR_NAMESPACE_NOT_FOUND, err, "nvs_c_erase_default_partition should delete namespace");

    //after
    //nvs_c_close(&handle);
    nvs_c_deinit_default_partition();
}

void test_if_nvs_c_erase_default_partition_deletes_key(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = 0;
    const char* key = "asdwadq";
    uint8_t data = 12;
    nvs_c_handle_t handle = NULL;

    //when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "test", NVS_C_READWRITE);
    nvs_c_write_uint8(handle, key, data);
    nvs_c_erase_default_partition();

    nvs_c_init_default_partition();
    nvs_c_open(&handle, "test", NVS_C_READWRITE);
    err = nvs_c_read_uint8(handle, key, &data);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(NVS_C_ERR_KEY_NOT_FOUND, err, "nvs_c_erase_default_partition should delete key from namespace");

    //after
    nvs_c_close(&handle);
    nvs_c_deinit_default_partition();
}

void test_if_nvs_c_erase_partition_returns_err_on_null_partition_label(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = 0;

    //when
    err = nvs_c_erase_partition(NULL);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_erase_partition should return err on NULL partition_label");
}

void test_if_nvs_c_erase_partition_returns_zero(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = 0;
    const char* partition_label = NVS_C_DEFAULT_NVS_PART_NAME;

    //when
    nvs_c_init_partition(partition_label);
    err = nvs_c_erase_partition(partition_label);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "nvs_c_erase_partition should return zero");
}

void test_if_nvs_c_erase_partition_deletes_namespace(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = 0;
    const char* partition_label = NVS_C_DEFAULT_NVS_PART_NAME;
    nvs_c_handle_t handle = NULL;

    //when
    nvs_c_init_partition(partition_label);
    nvs_c_open(&handle, "test", NVS_C_READWRITE);

    nvs_c_erase_partition(partition_label);

    nvs_c_init_partition(partition_label);
    err = nvs_c_open(&handle, "test", NVS_C_READONLY);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(NVS_C_ERR_NAMESPACE_NOT_FOUND, err, "nvs_c_erase_partition should delete namespace");

    //after
	
}

void test_if_nvs_c_erase_partition_deletes_key(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = 0;
    const char* partition_label = NVS_C_DEFAULT_NVS_PART_NAME;
    const char* key = "asdwadq";
    uint8_t data = 12;
    nvs_c_handle_t handle = NULL;

    //when
    nvs_c_init_partition(partition_label);
    nvs_c_open(&handle, "test", NVS_C_READWRITE);
    nvs_c_write_uint8(handle, key, data);
    nvs_c_erase_partition(partition_label);

    nvs_c_init_partition(partition_label);
    nvs_c_open(&handle, "test", NVS_C_READWRITE);
    err = nvs_c_read_uint8(handle, key, &data);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(NVS_C_ERR_KEY_NOT_FOUND, err, "nvs_c_erase_partition should delete key from namespace");

    //after
    nvs_c_close(&handle);
}

void test_if_nvs_c_erase_partition_returns_err_on_not_known_partition(void)
{
    // before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = 0;
    const char* partition_label = "asdasdwqe2";

    //when
    err = nvs_c_erase_partition(partition_label);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(NVS_C_ERR_PARTITION_NOT_FOUND, err, "nvs_c_erase_partition should return err when partition not found");
}


int runUnityTests(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_if_nvs_c_erase_partition_returns_err_on_not_known_partition);
    RUN_TEST(test_if_nvs_c_erase_partition_deletes_key);
    RUN_TEST(test_if_nvs_c_erase_partition_deletes_namespace);
    RUN_TEST(test_if_nvs_c_erase_partition_returns_zero);
    RUN_TEST(test_if_nvs_c_erase_partition_returns_err_on_null_partition_label);
    RUN_TEST(test_if_nvs_c_erase_default_partition_deletes_key);
    RUN_TEST(test_if_nvs_c_erase_default_partition_deletes_namespace);
    RUN_TEST(test_if_nvs_c_erase_default_partition_returns_zero);
    RUN_TEST(test_if_nvs_c_erase_namespace_deletes_key);
    RUN_TEST(test_if_nvs_c_erase_namespace_returns_zero);
    RUN_TEST(test_if_nvs_c_erase_namespace_returns_err_on_read_only_namespace);
    RUN_TEST(test_if_nvs_c_erase_namespace_returns_err_on_null_handle);
    RUN_TEST(test_if_nvs_c_erase_key_deletes_key);
    RUN_TEST(test_if_nvs_c_erase_key_returns_zero);
    RUN_TEST(test_if_nvs_c_erase_key_returns_err_on_read_only_namespace);
    RUN_TEST(test_if_nvs_c_erase_key_returns_err_on_null_key);
    RUN_TEST(test_if_nvs_c_erase_key_returns_err_on_null_handle);
    RUN_TEST(test_if_nvs_c_read_blob_length_returns_written_blob_size);
    RUN_TEST(test_if_nvs_c_read_blob_length_returns_zero);
    RUN_TEST(test_if_nvs_c_read_blob_length_should_return_err_on_null_size);
    RUN_TEST(test_if_nvs_c_read_blob_length_should_return_err_on_null_key);
    RUN_TEST(test_if_nvs_c_read_blob_length_should_return_err_on_null_handle);
    RUN_TEST(test_if_nvs_c_read_string_length_returns_written_string_size);
    RUN_TEST(test_if_nvs_c_read_string_length_returns_zero);
    RUN_TEST(test_if_nvs_c_read_string_length_should_return_err_on_null_size);
    RUN_TEST(test_if_nvs_c_read_string_length_should_return_err_on_null_key);
    RUN_TEST(test_if_nvs_c_read_string_length_should_return_err_on_null_handle);
    RUN_TEST(test_if_nvs_c_read_blob_returns_err_on_too_small_buffer);
    RUN_TEST(test_if_nvs_c_read_string_returns_err_on_too_small_buffer);
    RUN_TEST(test_if_nvs_c_read_blob_should_return_err_on_null_buffer);
    RUN_TEST(test_if_nvs_c_read_blob_should_return_err_on_null_key);
    RUN_TEST(test_if_nvs_c_read_blob_should_return_err_on_null_handle);
    RUN_TEST(test_if_nvs_c_read_blob_returns_zero);
    RUN_TEST(test_if_written_blob_can_be_read);
    RUN_TEST(test_if_nvs_c_write_blob_should_return_zero);
    RUN_TEST(test_if_nvs_c_write_blob_should_return_err_read_only_namespace);
    RUN_TEST(test_if_nvs_c_write_blob_should_return_err_on_null_buffer);
    RUN_TEST(test_if_nvs_c_write_blob_should_return_err_on_null_key);
    RUN_TEST(test_if_nvs_c_write_blob_should_return_err_on_null_handle);
    RUN_TEST(test_if_nvs_c_write_uint64_writes_data);
    RUN_TEST(test_if_nvs_c_read_uint64_returns_zero);
    RUN_TEST(test_if_nvs_c_read_uint64_return_err_on_null_buffer);
    RUN_TEST(test_if_nvs_c_read_uint64_should_return_err_on_null_key);
    RUN_TEST(test_if_nvs_c_read_uint64_should_return_err_on_null_handle);
    RUN_TEST(test_if_nvs_c_write_uint64_returns_zero);
    RUN_TEST(test_if_nvs_c_write_uint64_should_return_err_on_null_key);
    RUN_TEST(test_if_nvs_c_write_uint64_should_return_err_on_null_handle);
    RUN_TEST(test_if_nvs_c_write_uint32_writes_data);
    RUN_TEST(test_if_nvs_c_read_uint32_returns_zero);
    RUN_TEST(test_if_nvs_c_read_uint32_return_err_on_null_buffer);
    RUN_TEST(test_if_nvs_c_read_uint32_should_return_err_on_null_key);
    RUN_TEST(test_if_nvs_c_read_uint32_should_return_err_on_null_handle);
    RUN_TEST(test_if_nvs_c_write_uint32_returns_zero);
    RUN_TEST(test_if_nvs_c_write_uint32_should_return_err_on_null_key);
    RUN_TEST(test_if_nvs_c_write_uint32_should_return_err_on_null_handle);
    RUN_TEST(test_if_nvs_c_write_uint16_writes_data);
    RUN_TEST(test_if_nvs_c_read_uint16_returns_zero);
    RUN_TEST(test_if_nvs_c_read_uint16_return_err_on_null_buffer);
    RUN_TEST(test_if_nvs_c_read_uint16_should_return_err_on_null_key);
    RUN_TEST(test_if_nvs_c_read_uint16_should_return_err_on_null_handle);
    RUN_TEST(test_if_nvs_c_write_uint16_returns_zero);
    RUN_TEST(test_if_nvs_c_write_uint16_should_return_err_on_null_key);
    RUN_TEST(test_if_nvs_c_write_uint16_should_return_err_on_null_handle);
    RUN_TEST(test_if_nvs_c_write_uint8_writes_data);
    RUN_TEST(test_if_nvs_c_read_uint8_returns_zero);
    RUN_TEST(test_if_nvs_c_read_uint8_return_err_on_null_buffer);
    RUN_TEST(test_if_nvs_c_read_uint8_should_return_err_on_null_key);
    RUN_TEST(test_if_nvs_c_read_uint8_should_return_err_on_null_handle);
    RUN_TEST(test_if_nvs_c_write_uint8_returns_zero);
    RUN_TEST(test_if_nvs_c_write_uint8_should_return_err_on_null_key);
    RUN_TEST(test_if_nvs_c_write_uint8_should_return_err_on_null_handle);
    RUN_TEST(test_if_nvs_c_write_int64_writes_data);
    RUN_TEST(test_if_nvs_c_read_int64_returns_zero);
    RUN_TEST(test_if_nvs_c_read_int64_return_err_on_null_buffer);
    RUN_TEST(test_if_nvs_c_read_int64_should_return_err_on_null_key);
    RUN_TEST(test_if_nvs_c_read_int64_should_return_err_on_null_handle);
    RUN_TEST(test_if_nvs_c_write_int64_returns_zero);
    RUN_TEST(test_if_nvs_c_write_int64_should_return_err_on_null_key);
    RUN_TEST(test_if_nvs_c_write_int64_should_return_err_on_null_handle);
    RUN_TEST(test_if_nvs_c_write_int32_writes_data);
    RUN_TEST(test_if_nvs_c_read_int32_returns_zero);
    RUN_TEST(test_if_nvs_c_read_int32_return_err_on_null_buffer);
    RUN_TEST(test_if_nvs_c_read_int32_should_return_err_on_null_key);
    RUN_TEST(test_if_nvs_c_read_int32_should_return_err_on_null_handle);
    RUN_TEST(test_if_nvs_c_write_int32_returns_zero);
    RUN_TEST(test_if_nvs_c_write_int32_should_return_err_on_null_key);
    RUN_TEST(test_if_nvs_c_write_int32_should_return_err_on_null_handle);
    RUN_TEST(test_if_nvs_c_write_int16_writes_data);
    RUN_TEST(test_if_nvs_c_read_int16_returns_zero);
    RUN_TEST(test_if_nvs_c_read_int16_return_err_on_null_buffer);
    RUN_TEST(test_if_nvs_c_read_int16_should_return_err_on_null_key);
    RUN_TEST(test_if_nvs_c_read_int16_should_return_err_on_null_handle);
    RUN_TEST(test_if_nvs_c_write_int16_returns_zero);
    RUN_TEST(test_if_nvs_c_write_int16_should_return_err_on_null_key);
    RUN_TEST(test_if_nvs_c_write_int16_should_return_err_on_null_handle);
    RUN_TEST(test_if_nvs_c_write_int8_writes_data);
    RUN_TEST(test_if_nvs_c_read_int8_returns_zero);
    RUN_TEST(test_if_nvs_c_read_int8_return_err_on_null_buffer);
    RUN_TEST(test_if_nvs_c_read_int8_should_return_err_on_null_key);
    RUN_TEST(test_if_nvs_c_read_int8_should_return_err_on_null_handle);
    RUN_TEST(test_if_nvs_c_write_int8_returns_zero);
    RUN_TEST(test_if_nvs_c_write_int8_should_return_err_on_null_key);
    RUN_TEST(test_if_nvs_c_write_int8_should_return_err_on_null_handle);
    RUN_TEST(test_if_nvs_c_deinit_partition_should_return_zero);
    RUN_TEST(test_if_nvs_c_deinit_default_partition_should_return_zero);
    RUN_TEST(test_if_nvs_c_deinit_partition_should_return_err_on_not_init_partition);
    RUN_TEST(test_if_nvs_c_deinit_default_partition_should_return_err_on_not_init_partition);
    RUN_TEST(test_if_nvs_c_erase_namespace_should_err_on_read_only_namespace);
    RUN_TEST(test_if_nvs_close_should_free_handle);
    RUN_TEST(test_if_nvs_c_erase_namespace_should_return_zero);
    RUN_TEST(test_if_nvs_c_erase_namespace_should_return_err_on_null_handle);
    RUN_TEST(test_if_nvs_c_read_string_should_return_err_on_null_buffer);
    RUN_TEST(test_if_nvs_c_read_string_should_return_err_on_null_key);
    RUN_TEST(test_if_nvs_c_read_string_should_return_err_on_null_handle);
    RUN_TEST(test_if_nvs_c_read_string_returns_zero);
    RUN_TEST(test_if_written_string_can_be_read);
    RUN_TEST(test_if_nvs_c_write_string_should_return_zero);
    RUN_TEST(test_if_nvs_c_write_string_should_return_err_read_only_namespace);
    RUN_TEST(test_if_nvs_c_write_string_should_return_err_on_null_buffer);
    RUN_TEST(test_if_nvs_c_write_string_should_return_err_on_null_key);
    RUN_TEST(test_if_nvs_c_write_string_should_return_err_on_null_handle);
    RUN_TEST(test_if_nvs_close_should_return_zero);
    RUN_TEST(test_if_nvs_close_should_return_err_on_null_handle);
    RUN_TEST(test_if_nvs_open_from_partition_read_only_returns_err_on_partition_not_found);
    RUN_TEST(test_if_nvs_init_partition_returns_wrong_args_err_on_too_long_partition_name);
    RUN_TEST(test_if_nvs_init_partition_returns_wrong_args_err_name_on_too_long_partition_name);
    RUN_TEST(test_if_nvs_open_from_partition_stores_correct_namespace_name_in_handle);
    RUN_TEST(test_if_nvs_open_from_partition_stores_correct_partition_name_in_handle);
    RUN_TEST(test_if_nvs_open_stores_correct_namespace_name_in_handle);
    RUN_TEST(test_if_nvs_open_stores_correct_partition_name_in_handle);
    RUN_TEST(test_if_nvs_open_returns_err_when_partition_not_init);
    RUN_TEST(test_if_nvs_open_from_partition_returns_err_on_null_partition_label);
    RUN_TEST(test_if_nvs_open_from_partition_returns_err_on_null_namespace);
    RUN_TEST(test_if_nvs_open_from_partition_returns_err_on_null_handle);
    RUN_TEST(test_if_nvs_open_from_partition_returns_err_when_partition_not_init);
    RUN_TEST(test_if_nvs_open_returns_err_when_partition_not_init);
    RUN_TEST(test_if_err_to_name_returns_string_on_nvs_err_key_not_found);
    RUN_TEST(test_if_err_to_name_returns_string_on_nvs_err_namespace_not_found);
    RUN_TEST(test_if_nvs_err_nvs_key_not_found_is_defined);
    RUN_TEST(test_if_nvs_err_nvs_namespace_not_found_is_defined);
    RUN_TEST(test_if_nvs_open_returns_zero);
    RUN_TEST(test_if_nvs_open_returns_err_on_null_namespace);
    RUN_TEST(test_if_nvs_open_returns_err_on_null_handle);
    RUN_TEST(test_if_err_to_name_returns_string_on_nvs_err_not_init);
    RUN_TEST(test_if_err_to_name_returns_string_on_nvs_err_wrong_mode);
    RUN_TEST(test_if_err_to_name_returns_string_on_nvs_err_partition_not_found);
    RUN_TEST(test_if_nvs_err_nvs_partition_not_found_is_defined);
    RUN_TEST(test_if_nvs_err_nvs_not_init_is_defined);
    RUN_TEST(test_if_nvs_err_wrong_mode_is_defined);
    RUN_TEST(test_if_nvs_err_base_is_defined);
    RUN_TEST(test_if_nvs_init_partition_returns_err_on_null_label);
    RUN_TEST(test_if_nvs_init_partition_returns_zero);
    RUN_TEST(test_if_nvs_init_returns_zero);
    RUN_TEST(test_if_nvs_init_partition_returns_err_when_partition_not_found);
    return UNITY_END();
}

/**
 * For ESP-IDF framework
 */
void app_main(void)
{
    runUnityTests();
    vTaskDelay(200);
    fflush(stdout);
    esp_restart();
}
