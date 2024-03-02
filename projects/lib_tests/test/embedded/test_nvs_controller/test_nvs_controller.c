#include "unity.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

//Change fatal color to purple, to use it to log test names.
#include "ansi_colors.h"
#define FATAL_COLOR PURPLE_BOLD

#include "nvs_controller.h"
#include "logger.h"
#include "err_controller.h"

#include "nvs.h"
#include "nvs_flash.h"


void setUp(void) {
  // set stuff up here
  logger_init();
  logger_set_log_level(LOG_LEVEL_VERBOSE);
}

void tearDown(void) {
  // clean stuff up here
}

void test_if_nvs_init_returns_zero(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = 0;

    //when
    err = nvs_c_init_default_partition();

    //then
    TEST_ASSERT_EQUAL_MESSAGE(0, err, "nvs_c_init_default_partition should return 0");

    //after
    nvs_c_deinit_default_partition();
}

void test_if_nvs_init_partition_returns_err_when_partition_not_found(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = 0;

    //when
    err = nvs_c_init_partition("partition");

    //then
    TEST_ASSERT_EQUAL_MESSAGE(NVS_C_ERR_PARTITION_NOT_FOUND, err, "nvs_c_init_partition should return NVS_C_ERR_NOT_FOUND when partition not found");
}

void test_if_nvs_init_partition_returns_zero(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = 0;

    //when
    err = nvs_c_init_partition("nvs");

    //then
    TEST_ASSERT_EQUAL_MESSAGE(0, err, "nvs_c_init_partition should return zero");

    //after
    nvs_c_deinit_default_partition();
}

void test_if_nvs_init_partition_returns_err_on_null_label(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = 0;

    //when
    err = nvs_c_init_partition(NULL);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_init_partition should return err on null label");
}

void test_if_nvs_err_base_is_defined(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //then
    #ifndef NVS_C_ERR_BASE
    TEST_FAIL_MESSAGE("NVS_C_ERR_BASE should be defined");
    #endif
}   

void test_if_nvs_err_wrong_mode_is_defined(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //then
    #ifndef NVS_C_ERR_WRONG_MODE
    TEST_FAIL_MESSAGE("NVS_C_ERR_WRONG_MODE should be defined");
    #endif
}  

void test_if_nvs_err_nvs_not_init_is_defined(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //then
    #ifndef NVS_C_ERR_NOT_INIT
    TEST_FAIL_MESSAGE("NVS_C_ERR_NOT_INIT should be defined");
    #endif
}

void test_if_nvs_err_nvs_partition_not_found_is_defined(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //then
    #ifndef NVS_C_ERR_PARTITION_NOT_FOUND
    TEST_FAIL_MESSAGE("NVS_C_ERR_PARTITION_NOT_FOUND should be defined");
    #endif
}

void test_if_err_to_name_returns_string_on_nvs_err_partition_not_found(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //when
    const char* exp_name = "NVS partition was not found.";
    const char* error_name = error_to_name(NVS_C_ERR_PARTITION_NOT_FOUND);

    //then
    TEST_ASSERT_EQUAL_STRING_MESSAGE(exp_name, error_name, "error_to_name error string different from expected");
}

void test_if_err_to_name_returns_string_on_nvs_err_wrong_mode(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //when
    const char* exp_name = "NVS namespace was opened in read only mode.";
    const char* error_name = error_to_name(NVS_C_ERR_WRONG_MODE);

    //then
    TEST_ASSERT_EQUAL_STRING_MESSAGE(exp_name, error_name, "error_to_name error string different from expected");
}

void test_if_err_to_name_returns_string_on_nvs_err_not_init(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //when
    const char* exp_name = "NVS was not init.";
    const char* error_name = error_to_name(NVS_C_ERR_NOT_INIT);

    //then
    TEST_ASSERT_EQUAL_STRING_MESSAGE(exp_name, error_name, "error_to_name error string different from expected");
}

void test_if_nvs_open_returns_err_on_null_handle(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = ERR_C_OK;
    const char* namespace = "namespace";

    //when
    err = nvs_c_open(NULL, namespace, NVS_C_READWRITE);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_open should return err on NULL handle");
}

void test_if_nvs_open_returns_err_on_null_namespace(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = ERR_C_OK;
    nvs_c_handle_t handle = NULL;

    //when
    err = nvs_c_open(&handle, NULL, NVS_C_READWRITE);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_open should return err on NULL namespace");
}

void test_if_nvs_open_returns_zero(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = ERR_C_OK;
    nvs_c_handle_t handle = NULL;
    const char* namespace = "nvs";

    //when
    nvs_c_init_default_partition();
    err = nvs_c_open(&handle, namespace, NVS_C_READWRITE);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "nvs_c_open should return zero");

    //after
    nvs_c_deinit_default_partition();
}

void test_if_nvs_err_nvs_namespace_not_found_is_defined(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //then
    #ifndef NVS_C_ERR_NAMESPACE_NOT_FOUND
    TEST_FAIL_MESSAGE("NVS_C_ERR_NAMESPACE_NOT_FOUND should be defined");
    #endif
}

void test_if_nvs_err_nvs_key_not_found_is_defined(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //then
    #ifndef NVS_C_ERR_KEY_NOT_FOUND
    TEST_FAIL_MESSAGE("NVS_C_ERR_KEY_NOT_FOUND should be defined");
    #endif
}

void test_if_err_to_name_returns_string_on_nvs_err_namespace_not_found(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //when
    const char* exp_name = "NVS namespace was not found in partition.";
    const char* error_name = error_to_name(NVS_C_ERR_NAMESPACE_NOT_FOUND);

    //then
    TEST_ASSERT_EQUAL_STRING_MESSAGE(exp_name, error_name, "error_to_name error string different from expected");
}

void test_if_err_to_name_returns_string_on_nvs_err_key_not_found(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //when
    const char* exp_name = "NVS key was not found in namespace.";
    const char* error_name = error_to_name(NVS_C_ERR_KEY_NOT_FOUND);

    //then
    TEST_ASSERT_EQUAL_STRING_MESSAGE(exp_name, error_name, "error_to_name error string different from expected");
}

void test_if_nvs_open_returns_err_when_partition_not_init(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = ERR_C_OK;
    nvs_c_handle_t handle = NULL;
    const char* namespace = "nvs";

    //when
    err = nvs_c_open(&handle, namespace, NVS_C_READWRITE);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(NVS_C_ERR_NOT_INIT, err, "nvs_c_open should return err when default partition not init");
}

void test_if_nvs_open_from_partition_returns_err_when_partition_not_init(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = ERR_C_OK;
    nvs_c_handle_t handle = NULL;
    const char* namespace = "test";
    const char* partition_label = "nvs";

    //when
    err = nvs_c_open_from_partition(&handle, partition_label, namespace, NVS_C_READWRITE);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(NVS_C_ERR_NOT_INIT, err, "nvs_c_open_from_partition should return err when partition not init");
}

void test_if_nvs_open_from_partition_returns_zero(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = ERR_C_OK;
    nvs_c_handle_t handle = NULL;
    const char* namespace = "test";
    const char* partition_label = "nvs";

    //when
    nvs_c_init_partition(partition_label);
    err = nvs_c_open_from_partition(&handle, partition_label, namespace, NVS_C_READWRITE);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "nvs_c_open_from_partition should return zero");

    //after
    nvs_c_deinit_partition(partition_label);
}

void test_if_nvs_open_from_partition_returns_err_on_null_handle(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = ERR_C_OK;
    const char* namespace = "namespace";
    const char* partition_label = "nvs";

    //when
    err = nvs_c_open_from_partition(NULL, partition_label,namespace, NVS_C_READWRITE);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_open_from_partition should return err on NULL handle");
}

void test_if_nvs_open_from_partition_returns_err_on_null_namespace(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = ERR_C_OK;
    nvs_c_handle_t handle = NULL;
    const char* partition_label = "nvs";

    //when
    err = nvs_c_open_from_partition(&handle, partition_label, NULL, NVS_C_READWRITE);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_open_from_partition should return err on NULL namespace");
}

void test_if_nvs_open_from_partition_returns_err_on_null_partition_label(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = ERR_C_OK;
    nvs_c_handle_t handle = NULL;
    const char* namespace = "nvs";

    //when
    err = nvs_c_open_from_partition(&handle, NULL, namespace, NVS_C_READWRITE);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_open_from_partition should return err on NULL partition label");
}

void test_if_nvs_open_stores_correct_partition_name_in_handle(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    nvs_c_handle_t handle = NULL;
    const char* namespace = "nvs";

    //when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, namespace, NVS_C_READWRITE);

    //then
    TEST_ASSERT_EQUAL_STRING_MESSAGE(NVS_C_DEFAULT_NVS_PART_NAME, (struct nvs_c_handle_obj*)handle->partition, "nvs_c_open should store correct default nvs partition name");

    //after
    nvs_c_deinit_default_partition();
}

void test_if_nvs_open_stores_correct_namespace_name_in_handle(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    nvs_c_handle_t handle = NULL;
    const char* namespace = "nvs";

    //when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, namespace, NVS_C_READWRITE);

    //then
    TEST_ASSERT_EQUAL_STRING_MESSAGE(namespace, (struct nvs_c_handle_obj*)handle->namespace, "nvs_c_open should store correct nvs namespace name");

    //after
    nvs_c_deinit_default_partition();
}

void test_if_nvs_open_from_partition_stores_correct_partition_name_in_handle(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    nvs_c_handle_t handle = NULL;
    const char* namespace = "namespace";
    const char* partition_label = "nvs";

    //when
    nvs_c_init_partition(partition_label);
    TEST_ASSERT_FALSE(nvs_c_open_from_partition(&handle, partition_label, namespace, NVS_C_READWRITE));

    //then
    TEST_ASSERT_EQUAL_STRING_MESSAGE(partition_label, (struct nvs_c_handle_obj*)handle->partition, "nvs_c_open_from_partition should store correct nvs partition name");

    //after
    nvs_c_deinit_partition(partition_label);
}

void test_if_nvs_open_from_partition_stores_correct_namespace_name_in_handle(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    nvs_c_handle_t handle = NULL;
    const char* namespace = "namespace";
    const char* partition_label = "nvs";

    //when
    nvs_c_init_partition(partition_label);
    nvs_c_open_from_partition(&handle, partition_label, namespace, NVS_C_READWRITE);

    //then
    TEST_ASSERT_EQUAL_STRING_MESSAGE(namespace, (struct nvs_c_handle_obj*)handle->namespace, "nvs_c_open_from_partition should store correct nvs namespace name");

    //after
    nvs_c_deinit_partition(partition_label);
}

void test_if_nvs_init_partition_returns_wrong_args_err_name_on_too_long_partition_name(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = ERR_C_OK;
    const char* partition_label = "asdasdasdasdasdafafwqfqfasadsasdasd";
    const char* expected_error_name = error_to_name(ERR_C_WRONG_ARGS);

    //when
    err = nvs_c_init_partition(partition_label);

    //then
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_error_name, error_to_name(err), "nvs_c_init_partition should return correct error name on too long partition name");
}

void test_if_nvs_init_partition_returns_wrong_args_err_on_too_long_partition_name(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = ERR_C_OK;
    const char* partition_label = "asdasdasdasdasdafafwqfqfasadsasdasd";

    //when
    err = nvs_c_init_partition(partition_label);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_WRONG_ARGS, err, "nvs_c_init_partition should return ERR_C_WRONG_ARGS on too long partition name");
}

void test_if_nvs_open_from_partition_read_only_returns_err_on_partition_not_found(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = ERR_C_OK;
    nvs_c_handle_t handle = NULL;
    const char* namespace = "test";
    const char* partition_label = "nvs";

    //when
    nvs_c_init_partition(partition_label);
    err = nvs_c_open_from_partition(&handle, partition_label, namespace, NVS_C_READONLY);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(NVS_C_ERR_NAMESPACE_NOT_FOUND, err, "nvs_c_open_from_partition when read only mode is used should return err when namespace not found");

    //after
    nvs_c_deinit_partition(partition_label);
}

void test_if_nvs_close_should_return_err_on_null_handle(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = ERR_C_OK;

    //when
    err = nvs_c_close(NULL);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_close should return err on NULL handle");
}

void test_if_nvs_close_should_return_zero(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = ERR_C_OK;

    //when
    nvs_c_handle_t handle = malloc(sizeof(struct nvs_c_handle_obj));
    err = nvs_c_close(&handle);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "nvs_c_close should return zero");
}

void test_if_nvs_c_write_string_should_return_err_on_null_handle(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = ERR_C_OK;
    const char* key = "key";
    char buffer[10] = {0};

    //when
    err = nvs_c_write_string(NULL, key, buffer);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_write_string should return err on NULL handle");
}

void test_if_nvs_c_write_string_should_return_err_on_null_key(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = ERR_C_OK;
    char buffer[10] = {0};
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;

    //when
    err = nvs_c_write_string(handle, NULL, buffer);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_write_string should return err on NULL key");
}

void test_if_nvs_c_write_string_should_return_err_on_null_buffer(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = ERR_C_OK;
    const char* key = "key";
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;

    //when
    err = nvs_c_write_string(handle, key, NULL);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_write_string should return err on NULL buffer");
}

void test_if_nvs_c_write_string_should_return_err_read_only_namespace(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = ERR_C_OK;
    const char* key = "key";
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;
    char buffer[15] = {0};

    //when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "nvs", NVS_C_READWRITE);
    nvs_c_close(&handle);
    nvs_c_open(&handle, "nvs", NVS_C_READONLY);
    err = nvs_c_write_string(handle, key, buffer);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(NVS_C_ERR_WRONG_MODE, err, "nvs_c_write_string should return err on read only mode");

    //after
    nvs_c_deinit_default_partition();
}

void test_if_nvs_c_write_string_should_return_zero(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = ERR_C_OK;
    const char* key = "key";
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;
    char buffer[15] = "data";

    //when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "nvs", NVS_C_READWRITE);
    err = nvs_c_write_string(handle, key, buffer);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "nvs_c_write_string should return ERR_C_OK");

    //after
    nvs_c_deinit_default_partition();
}

void test_if_written_string_can_be_read(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    const char* key = "key";
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;
    char written[] = "data";
    char read[15] = {0};

    //when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "nvs", NVS_C_READWRITE);
    nvs_c_write_string(handle, key, written);
    nvs_c_read_string(handle, key, read, sizeof(read));
    nvs_c_close(&handle);

    //then
    TEST_ASSERT_EQUAL_STRING_MESSAGE(written, read, "nvs_c_write_string should return ERR_C_OK");

    //after
    nvs_c_deinit_default_partition();
}

void test_if_nvs_c_read_string_returns_zero(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = ERR_C_OK;
    const char* key = "key";
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;
    char written[] = "data";
    char read[15] = {0};

    //when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "nvs", NVS_C_READWRITE);
    nvs_c_write_string(handle, key, written);
    err = nvs_c_read_string(handle, key, read, sizeof(read));
    nvs_c_close(&handle);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "nvs_c_read_string should return ERR_C_OK");

    //after
    nvs_c_deinit_default_partition();
}

void test_if_nvs_c_read_string_should_return_err_on_null_handle(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = ERR_C_OK;
    const char* key = "key";
    char buffer[10] = {0};

    //when
    err = nvs_c_read_string(NULL, key, buffer, sizeof(buffer));

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_read_string should return err on NULL handle");
}

void test_if_nvs_c_read_string_should_return_err_on_null_key(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = ERR_C_OK;
    char buffer[10] = {0};
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;

    //when
    err = nvs_c_read_string(handle, NULL, buffer, sizeof(buffer));

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_read_string should return err on NULL key");
}

void test_if_nvs_c_read_string_should_return_err_on_null_buffer(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = ERR_C_OK;
    const char* key = "key";
    struct nvs_c_handle_obj nvs = {0};
    nvs_c_handle_t handle = &nvs;

    //when
    err = nvs_c_read_string(handle, key, NULL, 0);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_read_string should return err on NULL buffer");
}

void test_if_nvs_c_erase_namespace_should_return_err_on_null_handle(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = ERR_C_OK;

    //when
    err = nvs_c_erase_namespace(NULL);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "nvs_c_erase_namespace should return err on NULL handle");
}

void test_if_nvs_c_erase_namespace_should_return_zero(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = ERR_C_OK;
    nvs_c_handle_t handle = NULL;

    //when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "nvs", NVS_C_READWRITE);
    err = nvs_c_erase_namespace(handle);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "nvs_c_erase_namespace should return zero");

    //after
    nvs_c_deinit_default_partition();
}

void test_if_nvs_close_should_free_handle(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    nvs_c_handle_t handle = NULL;

    //when
    handle = malloc(sizeof(struct nvs_c_handle_obj));
    nvs_c_close(&handle);

    //then
    TEST_ASSERT_NULL_MESSAGE(handle, "nvs_c_close should free handle");
}

void test_if_nvs_c_erase_namespace_should_err_on_read_only_namespace(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = ERR_C_OK;
    nvs_c_handle_t handle = NULL;

    //when
    nvs_c_init_default_partition();
    nvs_c_open(&handle, "nvs", NVS_C_READWRITE);
    nvs_c_close(&handle);
    nvs_c_open(&handle, "nvs", NVS_C_READONLY);
    err = nvs_c_erase_namespace(handle);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(NVS_C_ERR_WRONG_MODE, err, "nvs_c_erase_namespace should return err on read only namespace");

    //after
    nvs_c_deinit_default_partition();
}

void test_if_nvs_c_deinit_default_partition_should_return_err_on_not_init_partition(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = ERR_C_OK;

    //when
    err = nvs_c_deinit_default_partition();

    //then
    TEST_ASSERT_EQUAL_MESSAGE(NVS_C_ERR_NOT_INIT, err, "nvs_c_deinit_default_partition should return err on not init nvs partition");
}

void test_if_nvs_c_deinit_partition_should_return_err_on_not_init_partition(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = ERR_C_OK;
    const char* partition_label = "nvs";

    //when
    err = nvs_c_deinit_partition(partition_label);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(NVS_C_ERR_NOT_INIT, err, "nvs_c_deinit_partition should return err on not init nvs partition");
}

void test_if_nvs_c_deinit_default_partition_should_return_zero(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = ERR_C_OK;

    //when
    nvs_c_init_default_partition();
    err = nvs_c_deinit_default_partition();

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "nvs_c_deinit_default_partition should return zero");
}

void test_if_nvs_c_deinit_partition_should_return_zero(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //given
    int err = ERR_C_OK;
    const char* partition_label = "nvs";

    //when
    nvs_c_init_partition(partition_label);
    err = nvs_c_deinit_partition(partition_label);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(NVS_C_ERR_NOT_INIT, err, "nvs_c_deinit_partition should return zero");
}

int runUnityTests(void) {
  UNITY_BEGIN();
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
void app_main(void) {
  runUnityTests();
  vTaskDelay(200);
  fflush(stdout);
  esp_restart();
}