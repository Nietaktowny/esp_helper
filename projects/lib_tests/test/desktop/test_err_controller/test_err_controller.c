#include "unity.h"
#include <stdio.h>

//Change fatal color to purple, to use it to log test names.
#include "ansi_colors.h"
#define FATAL_COLOR PURPLE_BOLD

#include "err_controller.h"
#include "errors_list.h"
#include "logger.h"

//EXAMPLE ERRORS TO CHECK
#define ERR_TCP_INVALID_PORT    0x0024 + 0x0523              ///< Invalid port number.

void setUp(void) {
  // set stuff up here
  logger_init();
  logger_set_log_level(LOG_LEVEL_VERBOSE);
}

void tearDown(void) {
  // clean stuff up here
}

void test_if_check_c_err_is_defined(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    #ifndef ERR_C_CHECK_ERROR
    TEST_FAIL_MESSAGE("ERR_C_CHECK_ERROR should be defined");
    #endif
}

void test_if_check_mem_alloc_is_defined(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    #ifndef ERR_C_CHECK_MEM_ALLOC
    TEST_FAIL_MESSAGE("ERR_C_CHECK_MEM_ALLOC should be defined");
    #endif
}

void test_if_check_null_ptr_is_defined(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    #ifndef ERR_C_CHECK_NULL_PTR
    TEST_FAIL_MESSAGE("ERR_C_CHECK_NULL_PTR should be defined");
    #endif
}

void test_if_check_set_and_throw_is_defined(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    #ifndef ERR_C_SET_AND_THROW_ERR
    TEST_FAIL_MESSAGE("ERR_C_SET_AND_THROW_ERR should be defined");
    #endif
}

void test_if_check_log_and_throw_is_defined(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    #ifndef ERR_C_LOG_AND_THROW_ERR
    TEST_FAIL_MESSAGE("ERR_C_LOG_AND_THROW_ERR should be defined");
    #endif
}

void test_if_check_and_throw_is_defined(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    #ifndef ERR_C_CHECK_AND_THROW_ERR
    TEST_FAIL_MESSAGE("ERR_C_CHECK_AND_THROW_ERR should be defined");
    #endif
}

void test_if_err_to_name_translates_example_err(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  const char* expected_err_name = "Invalid port number.";
  const char* ret_err_name = NULL;

  //when
  ret_err_name = error_to_name(ERR_TCP_INVALID_PORT);

  //then
  TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_err_name, ret_err_name, "example error name different from expected");
}

int runUnityTests(void) {
  UNITY_BEGIN();
  RUN_TEST(test_if_err_to_name_translates_example_err);
  RUN_TEST(test_if_check_and_throw_is_defined);
  RUN_TEST(test_if_check_log_and_throw_is_defined);
  RUN_TEST(test_if_check_set_and_throw_is_defined);
  RUN_TEST(test_if_check_null_ptr_is_defined);
  RUN_TEST(test_if_check_mem_alloc_is_defined);
  RUN_TEST(test_if_check_c_err_is_defined);
  return UNITY_END();
}


/**
  * For native platform.
  */
int main(void) {
  runUnityTests();
}