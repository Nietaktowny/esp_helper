#include "unity.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include <stdio.h>

#include "spiffs_controller.h"
#include "logger.h"

struct logger_contex {
    FILE* logfiles[LOGGER_MAX_OUTPUT_FILES];
    SemaphoreHandle_t mutex;
    uint8_t level;
    uint8_t logfiles_num;
    uint8_t silent_mode;
};

#define LOG_TEST_NAME                                                                  \
    printf(LOG_VERBOSE_FORMAT, RED_BOLD_UNTERLINE, timenow(), "TEST_NAME", __FILE__, __LINE__, __func__, __LINE__); \
    printf("%s\n", RESET);                                                                         \
    fflush(stdout)

void setUp(void) {
  // set stuff up here
}

void tearDown(void) {
  // clean stuff up here
}

void test_if_log_verbose_format_is_defined(void) {
    //before
    LOG_TEST_NAME;

    //given
    const char* verbose_format_expected = "%s%s | %-5s | %-15s:%d | %s:%d | ";

    //then
    TEST_ASSERT_EQUAL_STRING_MESSAGE(verbose_format_expected, LOG_VERBOSE_FORMAT, "LOG_VERBOSE_FORMAT different from expected.");
}

void test_if_log_strict_format_is_defined(void) {
    //before
    LOG_TEST_NAME;

    //given
    const char* verbose_format_expected = "%s%s | %-5s | %s:%d | ";

    //then
    TEST_ASSERT_EQUAL_STRING_MESSAGE(verbose_format_expected, LOG_STRICT_FORMAT, "LOG_STRICT_FORMAT different from expected.");
}

void test_if_logger_create_semphr_returns_zero(void) {
    //before
    LOG_TEST_NAME;

    //given
    int err = -1;

    //when
    err = logger_create_semphr();

    //then
    TEST_ASSERT_EQUAL_MESSAGE(0, err , "logger_create_semphr should return 0 on success");
}

void test_if_logger_create_semphr_creates_mutex(void) {
    //before
    LOG_TEST_NAME;

    //given
    SemaphoreHandle_t mutex = NULL;

    //when
    logger_create_semphr();
    mutex = (SemaphoreHandle_t)logger_get_log_mutex();

    //then
    TEST_ASSERT_NOT_NULL_MESSAGE(mutex, "logger mutex cannot be NULL after logger_create_semphr");
}

void test_if_logger_delete_semphr_deletes_mutex(void) {
    //before
    LOG_TEST_NAME;

    //given
    logger_create_semphr();
    SemaphoreHandle_t mutex = (SemaphoreHandle_t)logger_get_log_mutex();
    TEST_ASSERT_NOT_NULL_MESSAGE(mutex, "logger mutex cannot be NULL");

    //when
    logger_delete_semphr();
    mutex = (SemaphoreHandle_t)logger_get_log_mutex();

    //then
    TEST_ASSERT_NULL_MESSAGE(mutex, "logger mutex should be NULL after logger_delete_semphr");
}

void test_if_logger_init_returns_zero(void) {
    //before
    LOG_TEST_NAME;

    //given
    int err = 0;

    //when
    err = logger_init();

    //then
    TEST_ASSERT_EQUAL_MESSAGE(0, err, "logger_init should return 0");    
}

void test_if_after_logger_init_log_level_is_debug(void) {
    //before
    LOG_TEST_NAME;

    //given
    struct logger_contex* logger = NULL;
    uint8_t debug_level = -1;

    //when
    logger_init();
    logger = logger_get_logger_contex();
    debug_level = logger->level;

    //then
    TEST_ASSERT_EQUAL_MESSAGE(LOG_LEVEL_DEBUG, debug_level, "log level should be set to LOG_LEVEL_DEBUG after logger init");    
}

void test_if_after_logger_init_silent_mode_is_disabled(void) {
    //before
    LOG_TEST_NAME;

    //given
    struct logger_contex* logger = NULL;
    uint8_t silent_mode = -1;

    //when
    logger_init();
    logger = logger_get_logger_contex();
    silent_mode = logger->silent_mode;

    //then
    TEST_ASSERT_EQUAL_MESSAGE(0, silent_mode, "silent mode should be disabled after logger init");    
}

void test_if_after_logger_add_file_appends_file(void) {
    //before
    LOG_TEST_NAME;

    //given
    logger_create_semphr();
    struct logger_contex* logger = NULL;
    FILE* file = fopen("new_file.txt", "w");

    //when
    logger_init();
    logger_add_log_file(file);
    logger = logger_get_logger_contex();

    //then
    TEST_ASSERT_EQUAL_MESSAGE(file, logger->logfiles[0], "log file should be added after logger_add_log_file");    
}

int runUnityTests(void) {
  UNITY_BEGIN();
  RUN_TEST(test_if_after_logger_add_file_appends_file);
  RUN_TEST(test_if_after_logger_init_silent_mode_is_disabled);
  RUN_TEST(test_if_after_logger_init_log_level_is_debug);
  RUN_TEST(test_if_logger_init_returns_zero);
  RUN_TEST(test_if_log_verbose_format_is_defined);
  RUN_TEST(test_if_log_strict_format_is_defined);
  RUN_TEST(test_if_logger_create_semphr_returns_zero);
  RUN_TEST(test_if_logger_create_semphr_creates_mutex);
  RUN_TEST(test_if_logger_delete_semphr_deletes_mutex);
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