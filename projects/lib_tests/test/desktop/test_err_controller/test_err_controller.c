#include "unity.h"
#include <stdio.h>

//Change fatal color to purple, to use it to log test names.
#include "ansi_colors.h"
#define FATAL_COLOR PURPLE_BOLD

#include "err_controller.h"
#include "logger.h"

int null_ptr_test_helper(const void* ptr) {
  ERR_C_CHECK_NULL_PTR(ptr, LOG_ERROR("null ptr"));
  return 0;
}

int mem_alloc_test_helper(const void* ptr) {
  ERR_C_CHECK_MEM_ALLOC(ptr, LOG_ERROR("memory alloc fail"));
  return 0;
}

int check_err_test_helper(err_c_t err) {
  ERR_C_CHECK_ERROR(err, LOG_ERROR("error"));
  return 0;
}

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

void test_if_err_to_name_translates_err_null_pointer(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  const char* expected_err_name = "Null pointer was passed or dereferenced.";
  const char* ret_err_name = NULL;

  //when
  ret_err_name = error_to_name(ERR_C_NULL_POINTER);

  //then
  TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_err_name, ret_err_name, "ERR_C_NULL_POINTER error name different from expected");
}

void test_if_err_controller_base_is_defined(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    #ifndef ERR_C_ERR_BASE
    TEST_FAIL_MESSAGE("ERR_C_ERR_BASEs should be defined");
    #endif
}

void test_if_err_ok_is_defined(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    #ifndef ERR_C_OK
    TEST_FAIL_MESSAGE("ERR_C_OK should be defined");
    #endif
}

void test_if_err_null_pointer_is_defined(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    #ifndef ERR_C_NULL_POINTER
    TEST_FAIL_MESSAGE("ERR_C_NULL_POINTER should be defined");
    #endif
}

void test_if_err_no_memory_is_defined(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    #ifndef ERR_C_NO_MEMORY
    TEST_FAIL_MESSAGE("ERR_C_NO_MEMORY should be defined");
    #endif
}

void test_if_err_memory_err_is_defined(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    #ifndef ERR_C_MEMORY_ERR
    TEST_FAIL_MESSAGE("ERR_C_MEMORY_ERR should be defined");
    #endif
}

void test_if_err_wrong_args_is_defined(void) {
    //before
    LOG_FATAL("RUNNING: %s", __func__);

    #ifndef ERR_C_WRONG_ARGS
    TEST_FAIL_MESSAGE("ERR_C_WRONG_ARGS should be defined");
    #endif
}

void test_if_err_to_name_translates_err_ok(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  const char* expected_err_name = "No error.";
  const char* ret_err_name = NULL;

  //when
  ret_err_name = error_to_name(ERR_C_OK);

  //then
  TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_err_name, ret_err_name, "ERR_C_OK error name different from expected");
}

void test_if_err_to_name_translates_err_no_memory(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  const char* expected_err_name = "Memory allocation failed.";
  const char* ret_err_name = NULL;

  //when
  ret_err_name = error_to_name(ERR_C_NO_MEMORY);

  //then
  TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_err_name, ret_err_name, "ERR_C_NO_MEMORY error name different from expected");
}

void test_if_err_to_name_translates_err_memory_err(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  const char* expected_err_name = "Memory error occurred.";
  const char* ret_err_name = NULL;

  //when
  ret_err_name = error_to_name(ERR_C_MEMORY_ERR);

  //then
  TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_err_name, ret_err_name, "ERR_C_MEMORY_ERR error name different from expected");
}

void test_if_err_to_name_translates_err_wrongs_args(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  const char* expected_err_name = "Wrong arguments were used.";
  const char* ret_err_name = NULL;

  //when
  ret_err_name = error_to_name(ERR_C_WRONG_ARGS);

  //then
  TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_err_name, ret_err_name, "ERR_C_WRONG_ARGS error name different from expected");
}

void test_if_err_controller_base_is_zero(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //then
  TEST_ASSERT_EQUAL_MESSAGE(0, ERR_C_ERR_BASE, "ERR_C_ERR_BASE should be 0");
}

void test_if_err_controller_ok_is_zero(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //then
  TEST_ASSERT_EQUAL_MESSAGE(0, ERR_C_OK, "ERR_C_OK should be 0");
}

void test_if_err_throw_err_throws_err(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  volatile err_c_t err = 333;

  //when
  Try {
    ERR_C_CHECK_AND_THROW_ERR(err);
  } Catch(err) {
    //then
    TEST_ASSERT_EQUAL_MESSAGE(333, err, "ERR_C_CHECK_AND_THROW_ERR should throw error");
  }
}

void test_if_err_throw_err_not_throws_err_c_ok(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  volatile err_c_t err = ERR_C_OK;

  //when
  Try {
    ERR_C_CHECK_AND_THROW_ERR(err);
  } Catch(err) {
    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "ERR_C_CHECK_AND_THROW_ERR should not throw error when error is ERR_C_OK");
  }
}

void test_if_err_throw_and_log_err_throws_err(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  volatile err_c_t err = 333;

  //when
  Try {
    ERR_C_LOG_AND_THROW_ERR(err, LOG_ERROR("error"));
  } Catch(err) {
    //then
    TEST_ASSERT_EQUAL_MESSAGE(333, err, "ERR_C_LOG_AND_THROW_ERR should throw error");
  }
}

void test_if_err_throw_and_log_err_not_throws_err_c_ok(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  volatile err_c_t err = ERR_C_OK;

  //when
  Try {
    ERR_C_LOG_AND_THROW_ERR(err, LOG_ERROR("no error"));
  } Catch(err) {
    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "ERR_C_LOG_AND_THROW_ERR should not throw error when error is ERR_C_OK");
  }
}

void test_if_err_c_set_and_throw_err_should_throw_error(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  volatile err_c_t err = ERR_C_OK;

  //when
  Try {
    ERR_C_SET_AND_THROW_ERR(err, 333);
  } Catch(err) {
    //then
    TEST_ASSERT_EQUAL_MESSAGE(333, err, "ERR_C_SET_AND_THROW_ERR should throw error");
  }
}

void test_if_err_c_check_null_ptr_returns_err_on_null_ptr(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  const void* const null_ptr = NULL;

  //when
  TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, null_ptr_test_helper(null_ptr), "ERR_C_CHECK_NULL_PTR should return err on null pointer");
}

void test_if_err_c_check_null_ptr_not_returns_err_on_non_null_ptr(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  int data = 213;
  const void* const ptr = &data;

  //then
  TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, null_ptr_test_helper(ptr), "ERR_C_CHECK_NULL_PTR should not return err on non null pointer");
}

void test_if_err_c_check_mem_alloc_returns_err_on_null_ptr(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  const void* const null_ptr = NULL;

  //then
  TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NO_MEMORY, mem_alloc_test_helper(null_ptr), "ERR_C_CHECK_MEM_ALLOC should return err on null pointer");
}

void test_if_err_c_check_mem_alloc_not_returns_err_on_non_null_ptr(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  int data = 213;
  const void* const ptr = &data;

  //then
  TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, mem_alloc_test_helper(ptr), "ERR_C_CHECK_MEM_ALLOC should not return err on non null pointer");
}

void test_if_err_c_check_err_returns_err(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  int err = 3374;

  //then
  TEST_ASSERT_EQUAL_MESSAGE(err, check_err_test_helper(err), "ERR_C_CHECK_ERROR should return err.");
}

void test_if_err_c_check_err_not_returns_err_when_ok(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  int err = ERR_C_OK;

  //then
  TEST_ASSERT_EQUAL_MESSAGE(0, check_err_test_helper(err), "ERR_C_CHECK_ERROR should return not err when err code is 0");
}

void test_if_err_c_check_err_returns_err_on_minus_value(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  int err = -144;

  //then
  TEST_ASSERT_EQUAL_MESSAGE(err, check_err_test_helper(err), "ERR_C_CHECK_ERROR should return err on minus value");
}

void test_if_err_c_check_bitmask_returns_err_ok_on_correct_bitmask(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  uint32_t value = 0x0001;
  uint32_t mask = 0x0001;
  err_c_t err = -1;

  //when
  err = err_check_bitmask(mask, value);

  //then
  TEST_ASSERT_EQUAL_MESSAGE(0, err, "err_check_bitmask should return ERR_C_OK on correct bitmask.");
}

void test_if_err_c_check_bitmask_returns_err_on_wrong_bitmask(void) {
  //before
  LOG_FATAL("RUNNING: %s", __func__);

  //given
  uint32_t value = 0x0001;
  uint32_t mask = 0x1000;
  err_c_t err = -1;

  //when
  err = err_check_bitmask(mask, value);

  //then
  TEST_ASSERT_NOT_EQUAL_MESSAGE(0, err, "err_check_bitmask should return non zero value on wrong bitmask.");
}

int runUnityTests(void) {
  UNITY_BEGIN();
  RUN_TEST(test_if_err_c_check_bitmask_returns_err_on_wrong_bitmask);
  RUN_TEST(test_if_err_c_check_bitmask_returns_err_ok_on_correct_bitmask);
  RUN_TEST(test_if_err_c_check_err_returns_err_on_minus_value);
  RUN_TEST(test_if_err_c_check_err_not_returns_err_when_ok);
  RUN_TEST(test_if_err_c_check_err_returns_err);
  RUN_TEST(test_if_err_c_check_mem_alloc_not_returns_err_on_non_null_ptr);
  RUN_TEST(test_if_err_c_check_mem_alloc_returns_err_on_null_ptr);
  RUN_TEST(test_if_err_c_check_null_ptr_not_returns_err_on_non_null_ptr);
  RUN_TEST(test_if_err_c_check_null_ptr_returns_err_on_null_ptr);
  RUN_TEST(test_if_err_c_set_and_throw_err_should_throw_error);
  RUN_TEST(test_if_err_throw_and_log_err_not_throws_err_c_ok);
  RUN_TEST(test_if_err_throw_and_log_err_throws_err);
  RUN_TEST(test_if_err_throw_err_not_throws_err_c_ok);
  RUN_TEST(test_if_err_throw_err_throws_err);
  RUN_TEST(test_if_err_controller_ok_is_zero);
  RUN_TEST(test_if_err_controller_base_is_zero);
  RUN_TEST(test_if_err_to_name_translates_err_wrongs_args);
  RUN_TEST(test_if_err_to_name_translates_err_memory_err);
  RUN_TEST(test_if_err_to_name_translates_err_no_memory);
  RUN_TEST(test_if_err_to_name_translates_err_ok);
  RUN_TEST(test_if_err_wrong_args_is_defined);
  RUN_TEST(test_if_err_memory_err_is_defined);
  RUN_TEST(test_if_err_no_memory_is_defined);
  RUN_TEST(test_if_err_null_pointer_is_defined);
  RUN_TEST(test_if_err_ok_is_defined);
  RUN_TEST(test_if_err_controller_base_is_defined);
  RUN_TEST(test_if_err_to_name_translates_err_null_pointer);
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