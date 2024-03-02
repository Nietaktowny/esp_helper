#include "unity.h"
#include <stdio.h>

//Change fatal color to purple, to use it to log test names.
#include "ansi_colors.h"
#define FATAL_COLOR PURPLE_BOLD

#include "err_controller.h"
#include "logger.h"
#include "nvs_c_errors.h"
#include "nvs_controller.h"


#include <stdlib.h>

void setUp(void) {
  // set stuff up here
  logger_init();
  logger_set_log_level(LOG_LEVEL_VERBOSE);
}

void tearDown(void) {
  // clean stuff up here
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

void test_if_nvs_c_mode_t_readonly_exists(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(0, NVS_C_READONLY, "nvs_c_mode_t NVS_C_READONLY value should be 0");
}

void test_if_nvs_c_mode_t_read_and_write_exists(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(1, NVS_C_READWRITE, "nvs_c_mode_t NVS_C_READWRITE value should be 0");
}

void test_if_struct_nvs_c_obj_and_nvs_handle_are_defined(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //when
    nvs_c_handle_t handle = malloc(sizeof(struct nvs_c_handle_obj));

    //then
    TEST_ASSERT_NOT_NULL_MESSAGE(handle, "nvs_c_handle_t cannot be NULL");
}

void test_if_struct_nvs_c_obj_contains_nvs_mode(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //when
    struct nvs_c_handle_obj nvs_obj = {
        .mode = NVS_C_READONLY,
    };
    nvs_c_handle_t handle = &nvs_obj;

    //then
    TEST_ASSERT_EQUAL_MESSAGE(NVS_C_READONLY, handle->mode, "handle should contain NVS open mode");
}

void test_if_struct_nvs_c_obj_contains_nvs_partition_name(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //when
    struct nvs_c_handle_obj nvs_obj = {
        .partition = "nvs",
    };
    nvs_c_handle_t handle = &nvs_obj;

    //then
    TEST_ASSERT_EQUAL_STRING_MESSAGE("nvs", handle->partition, "handle should contain NVS partition name");
}

void test_if_struct_nvs_c_obj_contains_nvs_namespace_name(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    //when
    struct nvs_c_handle_obj nvs_obj = {
        .namespace = "nvs",
    };
    nvs_c_handle_t handle = &nvs_obj;

    //then
    TEST_ASSERT_EQUAL_STRING_MESSAGE("nvs", handle->namespace, "handle should contain NVS namespace name");
}

int runUnityTests(void) {
  UNITY_BEGIN();
  RUN_TEST(test_if_struct_nvs_c_obj_contains_nvs_namespace_name);
  RUN_TEST(test_if_struct_nvs_c_obj_contains_nvs_partition_name);
  RUN_TEST(test_if_struct_nvs_c_obj_and_nvs_handle_are_defined);
  RUN_TEST(test_if_nvs_c_mode_t_read_and_write_exists);
  RUN_TEST(test_if_nvs_c_mode_t_readonly_exists);
  RUN_TEST(test_if_err_to_name_returns_string_on_nvs_err_key_not_found);
  RUN_TEST(test_if_err_to_name_returns_string_on_nvs_err_namespace_not_found);
  RUN_TEST(test_if_nvs_err_nvs_key_not_found_is_defined);
  RUN_TEST(test_if_nvs_err_nvs_namespace_not_found_is_defined);
  RUN_TEST(test_if_err_to_name_returns_string_on_nvs_err_not_init);
  RUN_TEST(test_if_err_to_name_returns_string_on_nvs_err_wrong_mode);
  RUN_TEST(test_if_err_to_name_returns_string_on_nvs_err_partition_not_found);
  RUN_TEST(test_if_nvs_err_nvs_partition_not_found_is_defined);
  RUN_TEST(test_if_nvs_err_nvs_not_init_is_defined);
  RUN_TEST(test_if_nvs_err_wrong_mode_is_defined);
  RUN_TEST(test_if_nvs_err_base_is_defined);
  return UNITY_END();
}

/**
  * For native platform.
  */
int main(void) {
  runUnityTests();
}